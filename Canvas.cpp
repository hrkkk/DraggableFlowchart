#include "Canvas.h"

Canvas::Canvas(QObject* parent) : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 800, 600);
}

Canvas::~Canvas()
{}

// 拖动进入了canvas控件
void Canvas::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-qliststring")) {
        event->acceptProposedAction();
    }
}

// 拖动中移动
void Canvas::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-qliststring")) {
        event->acceptProposedAction();
    }
}

// 拖动放下
void Canvas::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->hasFormat("application/x-qliststring")) {
        // 获取拖拽进入的控件名
        QByteArray encoded = event->mimeData()->data("application/x-qliststring");
        QStringList strings = QString::fromUtf8(encoded).split("\n", QString::SkipEmptyParts);
        QString widgetName = strings[0];
        QString widgetText = strings[1];
        // 检查重名
        int count = 0;
        foreach (auto& name, m_graph.m_nodes) {
            if (QString::fromStdString(name.first).startsWith(widgetName)) {
                count++;
            }
        }
        widgetName.append(QString("_%1").arg(count));
        // 创建一个新的控件
        CustomGraphicsItem* item = new CustomGraphicsItem(widgetName, widgetText);
        // 设置控件位置为拖动放下时鼠标的位置
        item->setPos(event->scenePos());
        // 将该控件添加到 Canvas 中
        this->addItem(item);
        // 更新图数据结构
        m_graph.addNode(widgetName.toStdString(), widgetText.toStdString(), event->scenePos().x(), event->scenePos().y());

        event->acceptProposedAction();
    }
}

// 滚轮缩放画布
void Canvas::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    float zoomFactor = 1.15; // 缩放因子
    if (event->delta() > 0) {
        // 放大
        emit sig_scaleScene(zoomFactor, zoomFactor);
    } else {
        // 缩小
        emit sig_scaleScene(1.0 / zoomFactor, 1.0 / zoomFactor);
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

CustomGraphicsItem* g_firstSelectedItem = nullptr;
CustomGraphicsItem* g_secondSelectedItem = nullptr;
void Canvas::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);

    // 获取点击的item控件
    if (event->button() == Qt::LeftButton) {
        // 鼠标左键移动控件
        // isPressed = true;
        // 移动控件时删除与其相连的所有连线
        CustomGraphicsItem* item = static_cast<CustomGraphicsItem*>(this->itemAt(event->scenePos(), QTransform()));
        if (item) {
            for (auto& entry : m_connections) {
                if (entry.srcItem == item || entry.dstItem == item) {
                    // 在画布中移除两个控件之间的连线
                    this->removeItem(entry.arrowLine);
                    entry.arrowLine = nullptr;
                    // 更新图数据结构
                    m_graph.removeEdge(entry.srcItem->getWidgetName().toStdString(), entry.dstItem->getWidgetName().toStdString());
                }
            }
        }
    } else if (event->button() == Qt::RightButton) {
        // 鼠标右键连接控件
        CustomGraphicsItem* item = static_cast<CustomGraphicsItem*>(this->itemAt(event->scenePos(), QTransform()));
        if (item) {
            if (!g_firstSelectedItem) {
                // 获取第一次点击的控件
                g_firstSelectedItem = item;
            } else {
                // 获取第二次点击的控件
                g_secondSelectedItem = item;

                if (g_firstSelectedItem && g_secondSelectedItem) {
                    // 在画布上为两个控件添加连线
                    QGraphicsItemGroup* group = createConnection(g_firstSelectedItem, g_secondSelectedItem);
                    this->m_connections.append({group, g_firstSelectedItem, g_secondSelectedItem});
                    // 更新图数据结构
                    m_graph.addEdge(g_firstSelectedItem->getWidgetName().toStdString(), g_secondSelectedItem->getWidgetName().toStdString());
                }

                g_firstSelectedItem = nullptr;  // 重置第一个item，以便下一次连接
            }
        }
    }
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton) {
        // 移动完成后为其重新添加连线
        CustomGraphicsItem* item = static_cast<CustomGraphicsItem*>(this->itemAt(event->scenePos(), QTransform()));
        if (item) {
            for (auto& entry : m_connections) {
                if (entry.arrowLine == nullptr) {
                    // 在画布中为两个控件添加连线
                    entry.arrowLine = createConnection(entry.srcItem, entry.dstItem);
                    // 更新图数据结构
                    m_graph.addEdge(entry.srcItem->getWidgetName().toStdString(), entry.dstItem->getWidgetName().toStdString());
                }
            }
            m_graph.updateNode(item->getWidgetName().toStdString(), event->scenePos().x(), event->scenePos().y());
        }
    }
}

QGraphicsItemGroup* Canvas::createConnection(CustomGraphicsItem *item1, CustomGraphicsItem *item2)
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
    QGraphicsPathItem* line = this->addPath(path, QPen(Qt::white, 2));

    // 在线段的起点处创建一个圆点
    QGraphicsEllipseItem* dot = this->addEllipse(start.x() - 5, start.y() - 5, 10, 10, QPen(Qt::white, 2), QBrush(Qt::white));

    // 在线段的终点处创建一个三角形箭头
    QVector<QPointF> arrowPoints = calculateArrowHead(mid2, end, 10.0, 5.0);
    QGraphicsPolygonItem* arrow = this->addPolygon(QPolygonF(arrowPoints), QPen(Qt::white, 2), QBrush(Qt::white));

    QGraphicsItemGroup* group = new QGraphicsItemGroup();
    group->addToGroup(line);
    group->addToGroup(dot);
    group->addToGroup(arrow);

    this->addItem(group);

    return group;
}

void Canvas::detachConnection(CustomGraphicsItem *item1, CustomGraphicsItem *item2)
{
    // for (auto& entry : m_connections) {
    //     if (entry.srcItem == item1 && entry.dstItem == item2) {
    //         this->removeItem(entry.arrowLine);
    //         // m_connections.erase(entry);
    //     }
    // }
}

void Canvas::clearCanvas()
{
    this->clear();
    m_connections.clear();
    m_graph.resetGraph();
}

void Canvas::saveRecipe(const QString& filename)
{
    m_graph.saveToFile(filename.toStdString());
}

void Canvas::loadRecipe(const QString& filename)
{
    // 清空画布
    clearCanvas();
    // 加载文件内容
    m_graph.loadFromFile(filename.toStdString());
    // 将流程图渲染到画布上
    for (auto& node : m_graph.m_nodes) {
        // 创建一个新的控件
        CustomGraphicsItem* item = new CustomGraphicsItem(QString::fromStdString(node.first), QString::fromStdString(node.second.text));
        // 设置控件位置为拖动放下时鼠标的位置
        item->setPos(node.second.x, node.second.y);
        // 将该控件添加到 Canvas 中
        this->addItem(item);
    }
    for (auto& item : m_graph.m_connections) {
        for (auto& dstWidget : item.second) {
            // 在画布上为两个控件添加连线
            CustomGraphicsItem* src = static_cast<CustomGraphicsItem*>(this->itemAt(m_graph.getNodePosition(item.first).first,
                                                                                     m_graph.getNodePosition(item.first).second, QTransform()));
            CustomGraphicsItem* dst = static_cast<CustomGraphicsItem*>(this->itemAt(m_graph.getNodePosition(dstWidget).first,
                                                                                     m_graph.getNodePosition(dstWidget).second, QTransform()));
            QGraphicsItemGroup* group = createConnection(src, dst);
            this->m_connections.append({group, src, dst});
        }
    }
}
