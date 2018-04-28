#ifndef CUT_H
#define CUT_H

#include "visualnode.h"

class Cut : public VisualNode
{
public:
    Cut(Canvas* canvas);

    void drawMeHere(QPointF scenePt) override;

private:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
};

#endif // CUT_H
