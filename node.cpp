#include "node.h"
#include "constants.h"

#include <QList>
#include <QDebug>

int Node::globalID = 0;

/*
 * Constructor for an arbitrary node
 */
Node::Node() :
    fertile(true),
    myID(globalID++)
{
}

/*
 * Adds the newest child to my children list and calls the recursive update
 * to fix the tree.
 */
void Node::adoptChild(Node* child) {
    if (!fertile){
      qDebug() << "WARNING: node"
               << myID
               << "is attempting to adopt a child it cannot have";
    }

    children.append(child);
    child->parent = this;
    child->updateTree();
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

// TODO: move to VisualNode

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
