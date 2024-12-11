#include "CustomGraphicsItem.h"

CustomGraphicsItem::CustomGraphicsItem(const QString& name, const QString& text, QGraphicsItem *parent) :
    QGraphicsItem(parent), m_name(name), m_text(text)
{
    // 初始化图形属性
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);

    m_rect = QRect(-50, -50, 250, 60);

    // 设置字体样式
    m_textFont = QFont(u8"黑体", 12, QFont::Bold);
    QFontMetrics fm(m_textFont); // 获取字体度量信息
    // 计算文本的起始坐标，使其居中
    m_textPosX = m_rect.x() + (m_rect.width() - fm.width(m_text)) / 2;
    m_textPosY = m_rect.y() + (m_rect.height() - fm.height()) / 2 + fm.ascent(); // 调整y坐标，使得基线对齐
}

// 重写绘图函数
void CustomGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing); // 设置抗锯齿，使文本更平滑
    painter->setBrush(QColor("#F6A04D"));
    painter->setPen(Qt::NoPen);

    painter->drawRoundedRect(m_rect, 15, 15);

    // 在控件中间位置显示控件名称
    painter->setFont(m_textFont);
    painter->setPen(Qt::black);
    painter->drawText(m_textPosX, m_textPosY, m_text);
}

// 重写边界矩形函数
QRectF CustomGraphicsItem::boundingRect() const
{
    return m_rect;
}

// 重写鼠标按下事件
void CustomGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

QString CustomGraphicsItem::getWidgetName() const
{
    return m_name;
}
