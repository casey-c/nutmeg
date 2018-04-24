#include "cut.h"
#include "constants.h"

Cut::Cut()
{
        //QPointF br(pt.x() + qreal(EMPTY_CUT_SIZE),
                   //pt.y() + qreal(EMPTY_CUT_SIZE));
        //drawBox = QRectF(pt, br);

}


void Cut::paint(QPainter* painter,
           const QStyleOptionGraphicsItem* option,
                QWidget* widget) {
    if (mouseDown)
      painter->setBrush(QBrush(gradClicked));
    else if (highlighted)
      painter->setBrush(QBrush(gradHighlighted));
    else
      painter->setBrush(QBrush(gradDefault));

    painter->drawRoundedRect(drawBox, qreal(BORDER_RADIUS), qreal(BORDER_RADIUS));
}
