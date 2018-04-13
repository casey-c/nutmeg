#ifndef STATEMENT_H
#define STATEMENT_H

#include "visualnode.h"
#include <QPointF>

class Statement : public VisualNode
{
public:
    Statement();

    // Prevent statments from having children of their own
    //Node2* addChildCut(QPointF scenePt) override;
    //Node2* addChildStatement(QPointF scenePt) override;
    //Node2* addChildPlaceholder(QPointF scenePt) override;

private:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
};

#endif // STATEMENT_H
