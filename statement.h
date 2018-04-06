#ifndef STATEMENT_H
#define STATEMENT_H


class Statement
{
public:
    Statement();

    // Prevent statments from having children of their own
    Node* addChildCut(QPointF scenePt) override { return nullptr; }
    Node* addChildStatement(QPointF scenePt) override { return nullptr; }
    Node* addChildPlaceholder(QPointF scenePt) override { return nullptr; }

private:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
};

#endif // STATEMENT_H
