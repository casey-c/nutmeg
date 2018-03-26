#include "node.h"
#include <QPainter>
#include <QDebug>

#define GRID_SPACING   16
#define STATEMENT_SIZE 32
#define EMPTY_CUT_SIZE 64

#define BORDER_RADIUS 10

Node::Node(const QPointF& scenePos) {
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);

    content = QRectF( QPointF(0,0), QPointF(qreal(EMPTY_CUT_SIZE),
                                            qreal(EMPTY_CUT_SIZE)) );

    setPos(scenePos);
}

QRectF Node::boundingRect() const {
    return content;
}

QPainterPath Node::shape() const {
    QPainterPath path;
    path.addRect(content);
    return path;
}

void Node::paint(QPainter* painter,
                 const QStyleOptionGraphicsItem* option,
                 QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setBrush(QBrush(QColor(45,47,72,240)));
    painter->drawRoundedRect(content, qreal(BORDER_RADIUS), qreal(BORDER_RADIUS));
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent* evt) {
    qDebug() << "entered node";
    QGraphicsObject::hoverEnterEvent(evt);
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) {
    qDebug() << "left node";
    QGraphicsObject::hoverEnterEvent(evt);
}
