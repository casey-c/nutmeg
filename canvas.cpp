#include "canvas.h"
#include "node.h"

#include <QMouseEvent>
#include <QDebug>

Canvas::Canvas(QWidget* parent) :
    QGraphicsView(parent) {

    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200,-200,400,400);
    setScene(scene);

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(400, 400);
    setMouseTracking(true);
}

void Canvas::addNode(const QPointF& pt) {
    Node* node = new Node(pt);
    scene->addItem(node);
}

void Canvas::mouseMoveEvent(QMouseEvent* evt) {
    mousePos = mapToScene(evt->pos());
    QGraphicsView::mouseMoveEvent(evt);
}

void Canvas::keyPressEvent(QKeyEvent* evt) {
    if (evt->modifiers() & Qt::ShiftModifier) {
        switch (evt->key()) {
        case Qt::Key_X:
          qDebug() << "mouse is at " << mousePos.x() << "," << mousePos.y();
          scene->addItem(new Node(mousePos));
          break;
        }
    }
}
