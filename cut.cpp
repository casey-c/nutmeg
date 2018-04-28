#include "cut.h"
#include "constants.h"
#include <QDebug>

Cut::Cut(Canvas* canvas) {
    this->canvas = canvas;
}

void Cut::drawMeHere(QPointF scenePt) {
    // Need to map the scene pt into my parent's coords
    QPointF pt = mapToParent(mapFromScene(scenePt));

    QPointF br(pt.x() + qreal(EMPTY_CUT_SIZE),
               pt.y() + qreal(EMPTY_CUT_SIZE));
    drawBox = QRectF(pt, br);
    prepareGeometryChange();
}


void Cut::paint(QPainter* painter,
           const QStyleOptionGraphicsItem* option,
                QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    qDebug() << "painting cut " << myID;
    if (mouseDown)
      painter->setBrush(QBrush(gradClicked));
    else if (highlighted) {
      painter->setBrush(QBrush(gradHighlighted));
      qDebug() << "highlighted yes";
    }
    else {
        qDebug() << "grad default";
        painter->setBrush(QBrush(gradDefault));
    }


    qDebug() << "drawBox is ("
             << drawBox.topLeft().x() << ","
             << drawBox.topLeft().y() << ") ; ("
             << drawBox.bottomRight().x() << ","
             << drawBox.bottomRight().y() << ")";

    painter->drawRoundedRect(drawBox, qreal(BORDER_RADIUS), qreal(BORDER_RADIUS));
}
