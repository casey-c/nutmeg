#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>

class Node;

class Canvas : public QGraphicsView
{
public:
    Canvas();

    void addNode(const QPointF& pt);
private:
    QGraphicsScene* scene;
};

#endif // CANVAS_H
