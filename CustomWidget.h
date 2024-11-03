#pragma once

#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>

class CustomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent*) override;

private:
    QPoint m_dragStartPos;
};
