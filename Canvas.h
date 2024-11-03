#pragma once

#include <QGraphicsWidget>
#include <QObject>
#include <QWidget>
#include <QDrag>
#include <QGraphicsSceneEvent>
#include <QMimeData>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QList>

struct ItemConnection {
    QGraphicsItemGroup* arrowLine;
    QGraphicsItem* srcItem;
    QGraphicsItem* dstItem;
};

class Canvas : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Canvas(QObject * parent = nullptr);
    ~Canvas();

private:
    QGraphicsItemGroup* createConnection(QGraphicsItem* item1, QGraphicsItem* item2);
    void detachConnection(QGraphicsItem* item1, QGraphicsItem* item2);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QList<ItemConnection> m_connections;
};
