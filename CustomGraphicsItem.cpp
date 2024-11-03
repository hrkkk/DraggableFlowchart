#include "CustomGraphicsItem.h"


CustomGraphicsItem::CustomGraphicsItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    // 初始化图形属性
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

// 重写绘图函数
void CustomGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(Qt::red);
    painter->drawRect(-50, -50, 100, 100);
}

// 重写边界矩形函数
QRectF CustomGraphicsItem::boundingRect() const
{
    return QRectF(-50, -50, 100, 100);
}

// 重写鼠标按下事件
void CustomGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // if (event->button() == Qt::LeftButton) {
    //     qDebug() << "Clicked!";
    // }
    QGraphicsItem::mousePressEvent(event);
}
