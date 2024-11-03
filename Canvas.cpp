#include "Canvas.h"

#include "CustomWidget.h"
#include <QDebug>
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <qmath.h>
#include "CustomGraphicsItem.h"

Canvas::Canvas(QObject* parent) : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 800, 600);
    // setAcceptDrops(true);
}

Canvas::~Canvas()
{
    // foreach (QGraphicsItem* item, this->items()) {
    //     this->removeItem(item);
    // }
}

void Canvas::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->hasFormat("x1")) {
        event->acceptProposedAction();
    }
}

void Canvas::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("x1")) {
        event->acceptProposedAction();
    }
}

void Canvas::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->hasFormat("x1")) {
        CustomGraphicsItem* item = new CustomGraphicsItem();
        item->setPos(event->scenePos());
        this->addItem(item);

        event->acceptProposedAction();
    }
}

QVector<QPointF> calculateArrowHead(const QPointF& start, const QPointF& end, double arrowLength, double arrowWidth) {
    // 线段的起点和终点坐标

    // 计算方向向量
    QPointF directionVector(end.x() - start.x(), end.y() - start.y());
    // 计算线段长度
    double length = qSqrt(directionVector.x() * directionVector.x() + directionVector.y() * directionVector.y());
    // 计算单位向量
    QPointF unitVector(directionVector.x() / length, directionVector.y() / length);
    // 计算垂直向量
    QPointF perpendicularVector(-unitVector.y(), unitVector.x());
    // 箭头宽度系数
    double arrowWidthCoefficient = 10.0;
    // 箭头的深度系数（即箭头向内的长度）
    double arrowDepthCoefficient = 20.0;
    // 计算箭头两侧的点
    QPointF arrowLeft = end + QPointF(
                            perpendicularVector.x() * arrowWidthCoefficient,
                            perpendicularVector.y() * arrowWidthCoefficient
                            ) - (unitVector * arrowDepthCoefficient);
    QPointF arrowRight = end + QPointF(
                             -perpendicularVector.x() * arrowWidthCoefficient,
                             -perpendicularVector.y() * arrowWidthCoefficient
                             ) - (unitVector * arrowDepthCoefficient);

    // 返回箭头的三个点
    return {arrowLeft, end, arrowRight};
}

bool isPressed = false;
QGraphicsItem* g_firstSelectedItem = nullptr;
QGraphicsItem* g_secondSelectedItem = nullptr;
void Canvas::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);

    // 获取点击的item控件
    if (event->button() == Qt::LeftButton) {
        // 鼠标左键移动控件
        isPressed = true;
        // 移动控件时删除与其相连的所有连线
        QGraphicsItem* item = this->itemAt(event->scenePos(), QTransform());
        if (item) {
            for (auto& entry : m_connections) {
                if (entry.srcItem == item || entry.dstItem == item) {
                    this->removeItem(entry.arrowLine);
                    entry.arrowLine = nullptr;
                }
            }
        }
    } else if (event->button() == Qt::RightButton) {
        // 鼠标右键连接控件
        QGraphicsItem* item = this->itemAt(event->scenePos(), QTransform());
        if (item) {
            if (!g_firstSelectedItem) {
                g_firstSelectedItem = item;
            } else {
                g_secondSelectedItem = item;

                if (g_firstSelectedItem && g_secondSelectedItem) {
                    QGraphicsItemGroup* group = createConnection(g_firstSelectedItem, g_secondSelectedItem);
                    this->m_connections.append({group, g_firstSelectedItem, g_secondSelectedItem});
                }

                g_firstSelectedItem = nullptr;  // 重置第一个item，以便下一次连接
            }
        }
    }
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);

    if ((event->button() == Qt::LeftButton) && isPressed) {
        auto item = this->itemAt(event->scenePos(), QTransform());
        if (item) {
            item->setPos(event->scenePos());
        }
    }
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton) {
        isPressed = false;
        // 移动完成后为其重新添加连线
        QGraphicsItem* item = this->itemAt(event->scenePos(), QTransform());
        if (item) {
            for (auto& entry : m_connections) {
                if (entry.arrowLine == nullptr) {
                    entry.arrowLine = createConnection(entry.srcItem, entry.dstItem);
                }
            }
        }
    }
}

QGraphicsItemGroup* Canvas::createConnection(QGraphicsItem *item1, QGraphicsItem *item2)
{
    QRectF firstRect = item1->sceneBoundingRect();
    QRectF secondRect = item2->sceneBoundingRect();

    float horizontalSpacing = abs(firstRect.center().x() - secondRect.center().x());
    float verticalSpacing = abs(firstRect.center().y() - secondRect.center().y());

    QPointF start;
    QPointF mid1;
    QPointF mid2;
    QPointF end;
    if (horizontalSpacing > verticalSpacing) {
        // 如果水平间隔大于垂直间隔
        if (secondRect.center().x() > firstRect.center().x()) {
            // 如果第二个控件在第一个控件的右边
            start = QPointF(firstRect.right(), firstRect.center().y());
            mid1 = QPointF((firstRect.center().x() + secondRect.center().x()) / 2, firstRect.center().y());
            mid2 = QPointF((firstRect.center().x() + secondRect.center().x()) / 2, secondRect.center().y());
            end = QPointF(secondRect.left(), secondRect.center().y());
        } else {
            // 如果第二个控件在第一个控件的左边
            start = QPointF(firstRect.left(), firstRect.center().y());
            mid1 = QPointF((firstRect.center().x() + secondRect.center().x()) / 2, firstRect.center().y());
            mid2 = QPointF((firstRect.center().x() + secondRect.center().x()) / 2, secondRect.center().y());
            end = QPointF(secondRect.right(), secondRect.center().y());
        }
    } else {
        // 如果垂直间隔大于水平间隔
        if (secondRect.center().y() > firstRect.center().y()) {
            // 如果第二个控件在第一个控件的下边
            start = QPointF(firstRect.center().x(), firstRect.bottom());
            mid1 = QPointF(firstRect.center().x(), (firstRect.center().y() + secondRect.center().y()) / 2);
            mid2 = QPointF(secondRect.center().x(), (firstRect.center().y() + secondRect.center().y()) / 2);
            end = QPointF(secondRect.center().x(), secondRect.top());
        } else {
            // 如果第二个控件在第一个控件的上边
            start = QPointF(firstRect.center().x(), firstRect.top());
            mid1 = QPointF(firstRect.center().x(), (firstRect.center().y() + secondRect.center().y()) / 2);
            mid2 = QPointF(secondRect.center().x(), (firstRect.center().y() + secondRect.center().y()) / 2);
            end = QPointF(secondRect.center().x(), secondRect.bottom());
        }
    }

    // 创建一条折线
    QPainterPath path;
    path.moveTo(start.x(), start.y());
    path.lineTo(mid1.x(), mid1.y());
    path.lineTo(mid2.x(), mid2.y());
    path.lineTo(end.x(), end.y());
    QGraphicsPathItem* line = this->addPath(path, QPen(Qt::cyan, 2));

    // 在线段的起点处创建一个圆点
    QGraphicsEllipseItem* dot = this->addEllipse(start.x() - 5, start.y() - 5, 10, 10, QPen(Qt::cyan, 2), QBrush(Qt::cyan));

    // 在线段的终点处创建一个三角形箭头
    QVector<QPointF> arrowPoints = calculateArrowHead(mid2, end, 10.0, 5.0);
    QGraphicsPolygonItem* arrow = this->addPolygon(QPolygonF(arrowPoints), QPen(Qt::cyan, 2), QBrush(Qt::cyan));

    QGraphicsItemGroup* group = new QGraphicsItemGroup();
    group->addToGroup(line);
    group->addToGroup(dot);
    group->addToGroup(arrow);

    this->addItem(group);

    return group;
}

void Canvas::detachConnection(QGraphicsItem *item1, QGraphicsItem *item2)
{
    // for (auto& entry : m_connections) {
    //     if (entry.srcItem == item1 && entry.dstItem == item2) {
    //         this->removeItem(entry.arrowLine);
    //         // m_connections.erase(entry);
    //     }
    // }
}
