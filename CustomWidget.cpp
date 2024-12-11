#include "CustomWidget.h"
#include <QDebug>

CustomWidget::CustomWidget(const QString& name, const QString& text, const QString& iconUrl, QWidget *parent)
    : QWidget{parent}, m_name(name), m_text(text), m_iconUrl(iconUrl)
{
    setAcceptDrops(true);

    this->setFixedSize(400, 60);

    m_rect = rect();
    // 设置字体样式
    m_textFont = QFont(u8"黑体", 12, QFont::Bold);
    QFontMetrics fm(m_textFont); // 获取字体度量信息
    // 计算文本的起始坐标，使其居中
    m_textPosX = m_rect.x() + (m_rect.width() - fm.width(m_text)) / 2;
    m_textPosY = m_rect.y() + (m_rect.height() - fm.height()) / 2 + fm.ascent(); // 调整y坐标，使得基线对齐
}

void CustomWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // 绘制外边框
    // p.drawRect(rect());
    // 绘制 Icon
    QRect iconRect = QRect(rect().x(), rect().y(), rect().height(), rect().height());
    p.drawPixmap(iconRect, QPixmap(m_iconUrl));
    // 绘制文字
    p.setFont(m_textFont);
    p.setPen(Qt::white);
    p.drawText(iconRect.right() + 20, m_textPosY, m_text);
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
        // 传递数据
        QString strings;
        strings.append(m_name);
        strings.append("\n");
        strings.append(m_text);
        mimeData->setData("application/x-qliststring", strings.toUtf8());

        QDrag* drag = new QDrag(this);
        drag->setMimeData(mimeData);

        // 开始拖拽
        drag->exec();
    }
}

void CustomWidget::mouseReleaseEvent(QMouseEvent* event)
{

}
