#include "visualnode.h"

VisualNode::VisualNode()
{

}

void VisualNode::mousePressEvent(QGraphicsSceneMouseEvent* evt) {}
void VisualNode::mouseMoveEvent(QGraphicsSceneMouseEvent* evt) {}
void VisualNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* evt) {}

void VisualNode::hoverEnterEvent(QGraphicsSceneHoverEvent* evt) {}
void VisualNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) {}

Node2* VisualNode::addChildCut(QPointF scenePt) {
    Node2* child = Node2::addChildCut(scenePt);
    redrawAncestors();
    return child;
}
