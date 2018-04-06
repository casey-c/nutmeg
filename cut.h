#ifndef CUT_H
#define CUT_H

#include "visualnode.h"

class Cut : VisualNode
{
public:
    Cut();

private:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
};

#endif // CUT_H
