#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QString>


class CustomGraphicsItem : public QGraphicsItem
{
public:
    explicit CustomGraphicsItem(const QString& name, const QString& text, QGraphicsItem *parent = nullptr);
    QString getWidgetName() const;

protected:
    // 重写绘图函数
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    // 重写边界矩形函数
    QRectF boundingRect() const override;
    // 重写鼠标按下事件
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString m_name;
    QString m_text;
    QRect m_rect;
    int m_textPosX;
    int m_textPosY;
    QFont m_textFont;
};
