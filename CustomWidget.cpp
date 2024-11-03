#include "CustomWidget.h"
#include <QDebug>

CustomWidget::CustomWidget(QWidget *parent)
    : QWidget{parent}
{
    setAcceptDrops(true);

    this->setFixedSize(200, 100);
    this->setStyleSheet("background-color: red;");
}

void CustomWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CustomWidget::mousePressEvent(QMouseEvent* event)
{
    m_dragStartPos = event->pos();
    event->accept(); // 阻止事件继续传播
}

void CustomWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        // 创建拖拽对象
        QMimeData* mimeData = new QMimeData();
        mimeData->setData("x1", "hello world");
        QDrag* drag = new QDrag(this);
        drag->setMimeData(mimeData);

        // 开始拖拽
        drag->exec();
    }
}

void CustomWidget::mouseReleaseEvent(QMouseEvent* event)
{

}
