#include "visualnode.h"

VisualNode::VisualNode()
{

}

void VisualNode::redrawAncestors() {}

void VisualNode::mousePressEvent(QGraphicsSceneMouseEvent* evt) {}
void VisualNode::mouseMoveEvent(QGraphicsSceneMouseEvent* evt) {}
void VisualNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* evt) {}

void VisualNode::hoverEnterEvent(QGraphicsSceneHoverEvent* evt) {}
void VisualNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) {}

#if 0
Node2* VisualNode::addChildCut(QPointF scenePt) {
    Node2* child = Node2::addChildCut(scenePt);
    redrawAncestors();
    return child;
}
#endif

// TODO: Collision Checking
/*static bool VisualNode::checkPotential(QList<Node*> sel, QPointF pt);
QRectF predictMySceneDraw(QList<Node*> altNodes, QList<QRectF> altDraws) const;
void setDrawBoxFromPotential(QRectF potDraw);

// TODO: Prediction
QPointF findPoint(const QList<QPointF> &bloom, qreal w, qreal h);*/
