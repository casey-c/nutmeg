#ifndef STATEMENT_H
#define STATEMENT_H

#include "node2.h"
#include "visualnode.h"
#include <QPointF>

class Statement : public VisualNode
{
public:
    Statement();

    // Prevent statments from having children of their own
    Node2* addChildCut(QPointF) override { return nullptr; }
    Node2* addChildStatement(QPointF) override { return nullptr; }
    Node2* addChildPlaceholder(QPointF) override { return nullptr; }

private:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
};

#endif // STATEMENT_H
