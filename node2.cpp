#include "node2.h"
#include "constants.h"

//#include "cut.h"
//#include "statement.h"

bool Node2::canHaveKids() {
    return fertile;
}

void Node2::adoptChild(Node2 *babyboy) {

}

void Node2::updateTree() {

}

QPointF snapPoint(const QPointF &pt)
{
    int x = pt.x() - (GRID_SPACING / 2);
    int y = pt.y() - (GRID_SPACING / 2);

    // Workaround for negative points
    bool negX = false;
    bool negY = false;

    if (x < 0)
    {
        negX = true;
        x = -x;
    }
    if (y < 0)
    {
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


Node2::Node2() :
    fertile(true) {

}

#if 0
Node2* Node2::addChildCut(QPointF scenePt) {
    // Try to find a suitable point using a predictor algorithm
    //QList<QPointF> bloom = constructAddBloom(scenePt);
    //QPointF predictedScenePt = findPoint(bloom,
                                //qreal(EMPTY_CUT_SIZE),
                                //qreal(EMPTY_CUT_SIZE));
    //QPointF finalPt = mapFromScene(predictedScenePt);

    //Cut* cut = new Cut(this, canvas, finalPt);
    Cut* cut = new Cut(this, canvas);

    /// TODO: move the predictor function after the creation of the new cut
    /// i.e. in the visualnode's overriden function of this, predict its pos
    /// after its already been made and move it post-birth

    children.append(cut);

    /// TODO: put this in visual node (its graphics related)
    cut->setParentItem(this);

    return cut;
}

Node2* Node2::addChildStatement(QPointF scenePt, QString letter) {
    Statement* statement = new Statement(canvas, this, letter);
    children.append(statement);

    /// TODO: visual node it
    statement->setParentItem(this);

    return statement;
}
#endif

//Node2* Node2::addChildPlaceholder(QPointF scenePt){
    //return nullptr;
//}

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

/// TODO: integrate with the new Node2 class
/*
 * Tries to find a valid point for the add function. Bloom should be non-empty.
 *
 * If no valid point found, returns the first point in the bloom (which should
 * be the snapped mousePos in scene coords if the bloom was created
 * successfully).
 *
 * Otherwise, this function returns a valid topLeft position in scene coords
 * such that a node of size w by h would have:
 *   1. no collisions with direct siblings
 *   2. no growing of the parent
 *
 * TODO: extend to percolate upwards, just in case we have multiple points that
 * avoid collision with siblings, and potentially one (or more or none) would
 * cause a collision when the parent, grandparent, etc. grew.
 *
 * NOTE: this function is called on/by the parent of a new node. So the new
 * node's width and height are the params, and (this) is the parent of such a
 * node.
 */
#if 0
QPointF Node::findPoint(const QList<QPointF> &bloom, qreal w, qreal h, bool isStatement)
{
    Q_UNUSED(isStatement)

    QList<QPointF> collOnly; // parallel
    QList<qreal> growSizes;  // to this -- size of the grown parent (heuristic)

    QList<QPointF> growOnly;

    canvas->clearBounds();

    qDebug() << "--- Start for ---";
    for (QPointF pt : bloom)
    {
        bool collOkay = true;
        bool growOkay = true; // TODO: actual checking logic

        qreal growSize;

        QPointF bottomRight(pt.x() + w, pt.y() + h);
        QRectF potDraw = QRectF(pt, bottomRight);
        QRectF potColl = potDraw.marginsAdded(QMarginsF(COLLISION_OFFSET, COLLISION_OFFSET, COLLISION_OFFSET, COLLISION_OFFSET));

        qDebug() << "--- Checking point ---";
        // Collision Check
        for (Node* n : children)
        {
            // TODO: allow statements to be closer

            //QRectF otherColl;
            //QRectF newColl;
            //if (n->isStatement() && isStatement)
            //{
                //otherColl = n->sceneDrawBox();
                //newColl = potDraw;
            //}
            //else
            //{
                //otherColl = n->getSceneCollisionBox();
                //newColl = potColl;
            //}

            if (potColl.intersects(n->sceneCollisionBox()))
            {
                collOkay = false;
                break;
            }
        }

        // TODO: growth check
        if (!isRoot())
        {
            QRectF sceneDraw = sceneDrawBox();

            printPt("potDraw tl", potDraw.topLeft());
            printPt("sceneDraw tl", sceneDraw.topLeft());
            printPt("potDraw br", potDraw.bottomRight());
            printPt("sceneDraw br", sceneDraw.bottomRight());

            qreal minX = qMin(potDraw.left() - qreal(GRID_SPACING), sceneDraw.left());
            qreal minY = qMin(potDraw.top() - qreal(GRID_SPACING), sceneDraw.top());
            qreal maxX = qMax(potDraw.right() + qreal(GRID_SPACING), sceneDraw.right());
            qreal maxY = qMax(potDraw.bottom() + qreal(GRID_SPACING), sceneDraw.bottom());

            if ( minX != sceneDraw.left() ||
                 minY != sceneDraw.top() ||
                 maxX != sceneDraw.right() ||
                 maxY != sceneDraw.bottom() )
            {
                qDebug() << "Parent changed size";
                growOkay = false;
                growSize = (maxX - minX) * (maxY - minY);
                //QRectF toCollision(QRectF draw) const;
                //canvas->addRedBound(potDraw);
            }
            else
            {
                qDebug() << "parent ok";
                //canvas->addBlueBound(potDraw);
            }
        }

        if (collOkay && growOkay)
        {
            printPt("Returning", pt);
            return pt;
        }
        else if (collOkay)
        {
            // didn't collide with siblings, but the parent grew
            collOnly.append(pt);
            growSizes.append(growSize);
        }
        else if (growOkay)
        {
            // collided with siblings, but no parent growth
            // TODO: i'm thinking this heuristic is not desirable
            growOnly.append(pt);
        }
    }
    qDebug() << "--- end for ---";

    if (!collOnly.empty())
    {
        qDebug() << "Coll only not empty";
        //return collOnly.first();
        // Heuristic: pick the item that makes the smallest area
        QList<QPointF>::iterator itp = collOnly.begin();
        QList<qreal>::iterator ita = growSizes.begin(); // these are parallel

        QPointF ret = collOnly.first();
        qreal min = growSizes.first();

        for (; itp != collOnly.end(); ++itp, ++ita)
        {
            if ((*ita) < min)
            {
                ret = (*itp);
                min = (*ita);
            }
        }

        return ret;
    }
    if (!growOnly.empty())
    {
        qDebug() << "Grow only not empty";
        // TODO: get rid of this heuristic (don't think its useful)
    }
    // Nothing worked, so return snapped
    return bloom.first();
}

#endif
