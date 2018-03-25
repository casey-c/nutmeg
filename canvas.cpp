#include "canvas.h"
#include "node.h"

Canvas::Canvas() :
    QGraphicsView() {

    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200,-200,400,400);
    setScene(scene);

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(400, 400);
}

void Canvas::addNode(const QPointF& pt) {
    Node* node = new Node(pt);
    scene->addItem(node);
}
