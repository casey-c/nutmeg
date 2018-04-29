#include "statement.h"
#include "constants.h"
#include <QDebug>

Statement::Statement(Canvas* canvas, QString s) {
    fertile = false;
    letter = s;
    font = QFont();
    font.setPixelSize(GRID_SPACING);
    this->canvas = canvas;
}

void Statement::drawMeHere(QPointF scenePt) {
    // Need to map the scene pt into my parent's coords
    QPointF pt = mapToParent(mapFromScene(scenePt));

    QPointF br(pt.x() + qreal(STATEMENT_SIZE),
               pt.y() + qreal(STATEMENT_SIZE));
    drawBox = QRectF(pt, br);
    prepareGeometryChange();
}

void Statement::paint(QPainter* painter,
           const QStyleOptionGraphicsItem* option,
           QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    qDebug() << "painting statement " << myID;

    painter->setPen(QPen(QColor(0,0,0,0)));

    if (selected) {
        qDebug() << "selected";
        painter->setBrush(QBrush(gradSelected));
    }
    else if (mouseDown) {
        qDebug() << "clicked";
        painter->setBrush(QBrush(gradClicked));
    }
    else if (highlighted) {
        qDebug() << "highlighted";
        painter->setBrush(QBrush(gradHighlighted));
    }
    else {
        qDebug() << "default";
        painter->setBrush(QBrush(gradDefault));
    }

    painter->drawRoundedRect(drawBox, qreal(BORDER_RADIUS), qreal(BORDER_RADIUS));

    painter->setPen(QPen(QColor(0,0,0,255)));
    painter->setFont(font);
    painter->drawText( drawBox, Qt::AlignCenter, letter );
}
