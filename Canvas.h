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
#include <QDebug>
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <qmath.h>

#include "CustomGraphicsItem.h"
#include "Graph.h"

struct ItemConnection {
    QGraphicsItemGroup* arrowLine;
    CustomGraphicsItem* srcItem;
    CustomGraphicsItem* dstItem;
};

class Canvas : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Canvas(QObject * parent = nullptr);
    ~Canvas();

    void saveRecipe(const QString& filename);
    void loadRecipe(const QString& filename);
    void clearCanvas();

signals:
    void sig_scaleScene(float x, float y);

private:
    QGraphicsItemGroup* createConnection(CustomGraphicsItem* item1, CustomGraphicsItem* item2);
    void detachConnection(CustomGraphicsItem* item1, CustomGraphicsItem* item2);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    QList<ItemConnection> m_connections;
    Graph m_graph;
};
