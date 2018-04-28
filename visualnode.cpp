#include "visualnode.h"

#include "constants.h"
#include "qmath.h"
#include <QDebug>

QPointF snapPoint(const QPointF &pt);
qreal dist(const QPointF &a, const QPointF &b);


VisualNode::VisualNode()
{
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);

    shadow = new QGraphicsDropShadowEffect(this);
    shadow->setEnabled(false);
    shadow->setBlurRadius(2);
    shadow->setOffset(2);
    this->setGraphicsEffect(shadow);

    // Colors
    gradDefault = QRadialGradient( drawBox.x() + 3,
                                   drawBox.y() + 3,
                                   (dist(drawBox.topLeft(), drawBox.bottomRight()) * 2 ));
    gradDefault.setColorAt(0, QColor(249, 249, 249));
    gradDefault.setColorAt(1, QColor(249, 249, 249));

    gradHighlighted = QRadialGradient( drawBox.x() + 3,
                                       drawBox.y() + 3,
                                       (dist(drawBox.topLeft(), drawBox.bottomRight()) * 2 ));
    gradHighlighted.setColorAt(0, QColor(240, 240, 240));
    gradHighlighted.setColorAt(1, QColor(210, 210, 210));

    gradClicked = QRadialGradient( drawBox.x() + 3,
                                   drawBox.y() + 3,
                                   (dist(drawBox.topLeft(), drawBox.bottomRight()) * 2 ));
    gradClicked.setColorAt(0, QColor(210, 210, 210));
    gradClicked.setColorAt(1, QColor(240, 240, 240));

    gradSelected = QRadialGradient( drawBox.x() + 3,
                                    drawBox.y() + 3,
                                    (dist(drawBox.topLeft(), drawBox.bottomRight()) * 2 ));
    gradSelected.setColorAt(0, QColor(110, 226, 218));
    gradSelected.setColorAt(1, QColor(0, 209, 140));
}

//void VisualNode::moveMeToScenePos(QPointF pos){}

void VisualNode::drawMeHere(QPointF scenePt) {
    // Need to map the scene pt into my parent's coords
    QPointF pt = mapToParent(mapFromScene(scenePt));

    qDebug() << "drawing SCENE" << myID << " at pos " << scenePt.x() << "," << scenePt.y();
    qDebug() << "drawing parent?" << myID << " at pos " << pt.x() << "," << pt.y();
    QPointF br(pt.x() + qreal(EMPTY_CUT_SIZE),
               pt.y() + qreal(EMPTY_CUT_SIZE));
    drawBox = QRectF(pt, br);
    prepareGeometryChange();
}

void VisualNode::redrawAncestors() {}

void VisualNode::mousePressEvent(QGraphicsSceneMouseEvent* evt) {}
void VisualNode::mouseMoveEvent(QGraphicsSceneMouseEvent* evt) {}
void VisualNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* evt) {}

void VisualNode::hoverEnterEvent(QGraphicsSceneHoverEvent* evt) {
    qDebug() << "hover entered node" << myID;
    canvas->setHighlight(this);
    QGraphicsItem::hoverEnterEvent(evt);
}
void VisualNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) {
    qDebug() << "hover left node" << myID;
    QGraphicsItem::hoverLeaveEvent(evt);
}

QRectF VisualNode::boundingRect() const { return drawBox; }

QPainterPath VisualNode::shape() const {
    QPainterPath path;
    path.addRect(drawBox);
    return path;
}

#if 0
Node2* VisualNode::addChildCut(QPointF scenePt) {
    Node2* child = Node2::addChildCut(scenePt);
    redrawAncestors();
    return child;
}
#endif

// TODO: Collision Checking
/*static bool VisualNode::checkPotential(QList<Node*> sel, QPointF pt);
QRectF predictMySceneDraw(QList<Node*> altNodes, QList<QRectF> altDraws) const;
void setDrawBoxFromPotential(QRectF potDraw);

// TODO: Prediction
QPointF findPoint(const QList<QPointF> &bloom, qreal w, qreal h);*/

/*
 * A second bloom. More naive (no distance checking or sorting), but adds a few
 * more points. Additionally, the returned QList will always be size 9, never
 * empty like the other bloom may return if no movement.
 *
 * scenePos is unsnapped and in scene coords, and is the top left point of a
 * newly added drawbox.
 *
 * The points in this bloom are ordered as follows:
 *
 *     1 2 4
 *     3 0 6     where 0 is the initial snapped point
 *     5 7 8     and other points are placed GRID_SPACING apart
 *
 *   9  10 12 14 16
 *   11 1  2  4  18
 *   13 3  0  6  20
 *   15 5  7  8  22
 *   17 19 21 23 24
 *
 *  // Added one more upper left just in case
 *   25 26 28 30 32 34
 *   27 9  10 12 14 16
 *   29 11 1  2  4  18
 *   31 13 3  0  6  20
 *   33 15 5  7  8  22
 *   35 17 19 21 23 24
 *
 * TODO: rework things so we can use just one bloom, since they're basically
 * identical anyway.
 *
 * TODO: come up with a smarter spiraling out algorithm so everything is cleaner
 * and not hardcoded
 */
QList<QPointF> constructAddBloom(const QPointF &scenePos)
{
    QPointF snapped = snapPoint(scenePos);
    QList<QPointF> bloom;

    bloom.append(snapped);
    bloom.append(QPointF(snapped.x() - qreal(GRID_SPACING), snapped.y() - qreal(GRID_SPACING)));
    bloom.append(QPointF(snapped.x(),  snapped.y() - qreal(GRID_SPACING)));

    bloom.append(QPointF(snapped.x() - qreal(GRID_SPACING), snapped.y()));
    bloom.append(QPointF(snapped.x() + qreal(GRID_SPACING), snapped.y() - qreal(GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(GRID_SPACING), snapped.y() + qreal(GRID_SPACING)));

    bloom.append(QPointF(snapped.x() + qreal(GRID_SPACING), snapped.y()));
    bloom.append(QPointF(snapped.x(),  snapped.y() + qreal(GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(GRID_SPACING), snapped.y() + qreal(GRID_SPACING)));

    // New 16 (starts at 9)
    bloom.append(QPointF(snapped.x() - qreal(2 * GRID_SPACING), snapped.y() - qreal(2 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(1 * GRID_SPACING), snapped.y() - qreal(2 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(2 * GRID_SPACING), snapped.y() - qreal(1 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(0 * GRID_SPACING), snapped.y() - qreal(2 * GRID_SPACING)));

    // 13
    bloom.append(QPointF(snapped.x() - qreal(2 * GRID_SPACING), snapped.y() - qreal(0 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(1 * GRID_SPACING), snapped.y() - qreal(2 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(2 * GRID_SPACING), snapped.y() + qreal(1 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(2 * GRID_SPACING), snapped.y() - qreal(2 * GRID_SPACING)));

    // 17
    bloom.append(QPointF(snapped.x() - qreal(2 * GRID_SPACING), snapped.y() + qreal(2 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(2 * GRID_SPACING), snapped.y() - qreal(1 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(1 * GRID_SPACING), snapped.y() + qreal(2 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(2 * GRID_SPACING), snapped.y() - qreal(0 * GRID_SPACING)));

    // 21
    bloom.append(QPointF(snapped.x() - qreal(0 * GRID_SPACING), snapped.y() + qreal(2 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(2 * GRID_SPACING), snapped.y() + qreal(1 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(1 * GRID_SPACING), snapped.y() + qreal(2 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(2 * GRID_SPACING), snapped.y() + qreal(2 * GRID_SPACING)));

    // 25
    bloom.append(QPointF(snapped.x() - qreal(3 * GRID_SPACING), snapped.y() - qreal(3 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(2 * GRID_SPACING), snapped.y() - qreal(3 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(3 * GRID_SPACING), snapped.y() - qreal(2 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(1 * GRID_SPACING), snapped.y() - qreal(3 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(3 * GRID_SPACING), snapped.y() - qreal(1 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(0 * GRID_SPACING), snapped.y() - qreal(3 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(3 * GRID_SPACING), snapped.y() - qreal(0 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(1 * GRID_SPACING), snapped.y() - qreal(3 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(3 * GRID_SPACING), snapped.y() + qreal(1 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(2 * GRID_SPACING), snapped.y() - qreal(3 * GRID_SPACING)));
    bloom.append(QPointF(snapped.x() - qreal(3 * GRID_SPACING), snapped.y() + qreal(2 * GRID_SPACING)));


    return bloom;
}

/*
 * Helper function:
 * Maps given scene coordinates onto the grid.
 *
 * QPointF pt: (scene coords) with arbitrary (x,y)
 * Returns: pt (x,y) in scene coords where x and y are multiples of GRID_SPACING
 */
QPointF snapPoint(const QPointF &pt) {
    int x = pt.x() - (GRID_SPACING / 2);
    int y = pt.y() - (GRID_SPACING / 2);

    // Workaround for negative points
    bool negX = false;
    bool negY = false;

    if (x < 0) {
        negX = true;
        x = -x;
    }
    if (y < 0) {
        negY = true;
        y = -y;
    }

    // Perform the snap
    x = ((x + GRID_SPACING / 2) / GRID_SPACING) * GRID_SPACING;
    y = ((y + GRID_SPACING / 2) / GRID_SPACING) * GRID_SPACING;

    // Revert negation
    if (negX)
        x = -x;
    if (negY)
        y = -y;

    return QPointF(x, y);
}

/*
 * Return the distance between two points
 */
qreal dist(const QPointF &a, const QPointF &b)
{
    return qSqrt( qPow( ( a.x() - b.x() ), 2) +
                  qPow( ( a.y() - b.y() ), 2) );
}
