#ifndef NODE_H
#define NODE_H

#include <QGraphicsObject>

class Node : public QGraphicsObject
{
public:
    Node(const QPointF& scenePos);

private:
    QRectF content;
    //QRectF collision;

    /* Graphics */
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    /* Mouse */
    //void mousePressEvent(QGraphicsSceneMouseEvent* evt) override;
    //void mouseMoveEvent(QGraphicsSceneMouseEvent* evt) override;
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent* evt) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* evt) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) override;
};

#endif // NODE_H
