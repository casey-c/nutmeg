#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>

class Node;

class Canvas : public QGraphicsView
{
public:
    Canvas();
private:
    QGraphicsScene* scene;
};

#endif // CANVAS_H
