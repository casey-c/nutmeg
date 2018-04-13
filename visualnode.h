#ifndef VISUALNODE_H
#define VISUALNODE_H

#include <QGraphicsObject>
#include "node2.h"

/*
 * This is the base class for any nodes that need to be drawn / interacted with
 * i.e. statements and cuts. This enables us to reuse a lot of the collision
 * and mouse control logic but prevent root nodes from using any of that info
 */
class VisualNode : public Node2, public QGraphicsObject
{
public:
    VisualNode();

private:
    QRectF drawBox;
    bool mouseDown;

    // Boxes
    QRectF sceneCollisionBox() const;
    QRectF localCollisionBox() const;
    QRectF sceneDrawBox() const;

    // Graphics
    //Node2* addChildCut(QPointF scenePt) override;
    void redrawAncestors();

    // Mouse Interaction
    void mousePressEvent(QGraphicsSceneMouseEvent* evt) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* evt) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* evt) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* evt) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) override;

    // TODO: Collision Checking
    static bool checkPotential(QList<Node*> sel, QPointF pt);
    QRectF predictMySceneDraw(QList<Node*> altNodes, QList<QRectF> altDraws) const;
    void setDrawBoxFromPotential(QRectF potDraw);

    // TODO: Prediction
    QPointF findPoint(const QList<QPointF> &bloom, qreal w, qreal h);

    QRectF boundingRect() const override;
};

#endif // VISUALNODE_H
