#include "visualnode.h"

#include "constants.h"

QPointF snapPoint(const QPointF &pt);


VisualNode::VisualNode()
{

}

void VisualNode::redrawAncestors() {}

void VisualNode::mousePressEvent(QGraphicsSceneMouseEvent* evt) {}
void VisualNode::mouseMoveEvent(QGraphicsSceneMouseEvent* evt) {}
void VisualNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* evt) {}

void VisualNode::hoverEnterEvent(QGraphicsSceneHoverEvent* evt) {}
void VisualNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) {}

QRectF VisualNode::boundingRect() const { return QRectF(0,0,0,0); }

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
