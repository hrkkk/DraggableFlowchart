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

class CustomGraphicsItem : public QGraphicsItem
{
public:
    CustomGraphicsItem(QGraphicsItem *parent = nullptr);

protected:
    // 重写绘图函数
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    // 重写边界矩形函数
    QRectF boundingRect() const override;

    // 重写鼠标按下事件
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};
