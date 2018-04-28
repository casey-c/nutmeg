#include "canvas.h"
#include <QGraphicsRectItem>

////////////////////////////////////////////////////////////////////////////////
/// This file stores some of the debug functions for canvas. It's temporary  ///
/// until we have better organization and implementation. This is just to    ///
/// get it out of the way until we get to it.                                ///
////////////////////////////////////////////////////////////////////////////////

void Canvas::clearBounds() {
    for(QGraphicsRectItem* item : blueBounds)
        scene->removeItem(item);
    for(QGraphicsRectItem* item : blackBounds)
        scene->removeItem(item);
    for(QGraphicsRectItem* item : redBounds)
        scene->removeItem(item);

    blueBounds.clear();
    blackBounds.clear();
    redBounds.clear();
}

void Canvas::clearDots() {
    for (QGraphicsEllipseItem* item : blackDots)
        scene->removeItem(item);

    blackDots.clear();
}

void Canvas::addBlackDot(QPointF pt) {
    if (showBounds)
        blackDots.append(scene->addEllipse(pt.x() - 1,
                                           pt.y() - 1,
                                           2,
                                           2,
                                           QPen(QColor(0,0,0)),
                                           QBrush(QColor(0,0,0))));
}

void Canvas::addRedBound(QRectF rect) {
    if(showBounds)
        redBounds.append(scene->addRect(rect, QPen(QColor(255, 0, 0))));
}

void Canvas::addBlackBound(QRectF rect) {
    if(showBounds)
        blackBounds.append(scene->addRect(rect, QPen(QColor(0, 0, 0))));
}

void Canvas::addBlueBound(QRectF rect) {
    if(showBounds)
        blueBounds.append(scene->addRect(rect, QPen(QColor(0, 0, 255))));
}

void Canvas::addGreenBound(QRectF rect) {
    if(showBounds)
        blueBounds.append(scene->addRect(rect, QPen(QColor(0, 255, 0))));
}
