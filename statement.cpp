#include "statement.h"
#include "constants.h"

Statement::Statement(QString s)
{
    fertile = false;
    letter = s;
    font = QFont();
}

//Node2* Statement::addChildCut(QPointF scenePt) { return nullptr; }
//Node2* Statement::addChildStatement(QPointF scenePt) { return nullptr; }
//Node2* Statement::addChildPlaceholder(QPointF scenePt) { return nullptr; }

void Statement::paint(QPainter* painter,
           const QStyleOptionGraphicsItem* option,
           QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(QPen(QColor(0,0,0,0)));

    if (selected)
        painter->setBrush(QBrush(gradSelected));
    else
    {
        if (mouseDown)
            painter->setBrush(QBrush(gradClicked));
        else if (highlighted)
            painter->setBrush(QBrush(gradHighlighted));
        else
            painter->setBrush(QBrush(gradDefault));
    }

    painter->drawRoundedRect(drawBox, qreal(BORDER_RADIUS), qreal(BORDER_RADIUS));

    painter->setPen(QPen(QColor(0,0,0,255)));
    painter->setFont(font);
    painter->drawText( drawBox, Qt::AlignCenter, letter );
}
