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
#include <QString>

class CustomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomWidget(const QString& name, const QString& text, const QString& iconUrl, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent*) override;

private:
    QPoint m_dragStartPos;
    QString m_name;
    QString m_text;
    QString m_iconUrl;
    QRect m_rect;
    int m_textPosX;
    int m_textPosY;
    QFont m_textFont;
};
