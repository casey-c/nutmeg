#include "canvas.h"
#include "node.h"

#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtCore/QtMath>
#include <QDebug>
#include <QQueue>

#define GRID_SPACING 16
#define STROKE_ADJ 2
#define COLLISION_OFFSET (GRID_SPACING / 2) - 1

#define EMPTY_CUT_SIZE (4 * GRID_SPACING)
#define STATEMENT_SIZE (2 * GRID_SPACING)
#define BIG_NUMBER 999999999

#define Z_NORMAL 0
#define Z_RAISED 10

#define BORDER_RADIUS 10

// Forward declarations for helper functions (implementation located at end)
QPointF snapPoint(const QPointF &pt);
qreal dist(const QPointF &a, const QPointF &b);
bool rectsCollide(const QRectF &a, const QRectF &b);
void printPt(const QString &s, const QPointF &pt);
void printRect(const QString &s, const QRectF &r);
void printMinMax(qreal minX, qreal minY, qreal maxX, qreal maxY);
QList<QPointF> constructBloom(QPointF scenePos, QPointF sceneTarget);
bool pointInRect(const QPointF &pt, const QRectF &rect);
QList<QPointF> constructAddBloom(const QPointF &scenePos);
bool rectSurroundedBy(QRectF inside, QRectF outside);

// Static var intitial declaration
int Node::globalID = 0;

////////////////////
/// Construction ///
////////////////////

/*
 * (Static) Returns a new root node
 */
Node* Node::makeRoot(Canvas* can)
{
    return new Node(can, nullptr, Root, QPointF(0,0));
}

/*
 * Private constructor for all types of nodes
 */
Node::Node(Canvas* can, Node* par, NodeType t, QPointF pt) :
    myID(globalID++),
    canvas(can),
    parent(par),
    type(t),
    highlighted(false),
    mouseDown(false),
    mouseOffset(0, 0),
    selected(false),
    ghost(false)
{
    // Drop shadow on click and drag
    shadow = new QGraphicsDropShadowEffect(this);
    shadow->setEnabled(false);
    shadow->setBlurRadius(2);
    shadow->setOffset(2);
    this->setGraphicsEffect(shadow);

    if ( isRoot() )
    {

    }
    else if ( isCut() )
    {
        setFlag(ItemSendsGeometryChanges);
        setCacheMode(DeviceCoordinateCache);
        setAcceptHoverEvents(true);
        QPointF br(pt.x() + qreal(EMPTY_CUT_SIZE),
                   pt.y() + qreal(EMPTY_CUT_SIZE));
        drawBox = QRectF(pt, br);
    }

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

/* Statement constructor */
Node::Node(Canvas* can, Node* par, QString s, QPointF pt) :
    canvas(can),
    parent(par),
    type(Statement),
    highlighted(false),
    mouseDown(false),
    mouseOffset(0, 0),
    letter(s),
    selected(false),
    ghost(false)
{
    // Qt flags
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);

    // Shadow
    shadow = new QGraphicsDropShadowEffect(this);
    shadow->setEnabled(false);
    shadow->setBlurRadius(2);
    shadow->setOffset(2);
    this->setGraphicsEffect(shadow);

    // Draw box
    QPointF br(pt.x() + qreal(STATEMENT_SIZE),
               pt.y() + qreal(STATEMENT_SIZE));
    drawBox = QRectF(pt, br);

    // Color palette
    gradDefault = QRadialGradient( drawBox.x() + 3,
                                   drawBox.y() + 3,
                                   (dist(drawBox.topLeft(), drawBox.bottomRight()) * 2 ));
    gradDefault.setColorAt(0, QColor(0, 0, 0, 0));
    gradDefault.setColorAt(1, QColor(0, 0, 0, 0));

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

    // Statement font
    font = QFont();
    font.setPixelSize(GRID_SPACING * 2 - 6);
}

// Destructor
Node::~Node()
{
    qDebug() << "Calling destructor";
    if (parent != nullptr)
    {
        parent->children.removeOne(this);
        parent->updateAncestors();
    }

    for (Node* child : children)
        delete child;
}

Node* Node::addChildCut(QPointF pt)
{
    QList<QPointF> bloom = constructAddBloom(pt);
    QPointF finalPoint = findPoint(bloom,
                                   qreal(EMPTY_CUT_SIZE),
                                   qreal(EMPTY_CUT_SIZE));

    //Node* newChild = new Node(canvas, this, Cut, finalPoint);
    Node* newChild = new Node(canvas, this, Cut, mapFromScene(finalPoint));

    children.append(newChild);
    newChild->setParentItem(this);
    updateAncestors();

    return newChild;
}

Node* Node::addChildStatement(QPointF pt, QString t)
{
    QList<QPointF> bloom = constructAddBloom(pt);
    QPointF finalPoint = findPoint(bloom,
                                   qreal(STATEMENT_SIZE),
                                   qreal(STATEMENT_SIZE));

    Node* newChild = new Node(canvas, this, t, mapFromScene(finalPoint));
    children.append(newChild);
    newChild->setParentItem(this);
    //newChild->setPos(mapFromScene(finalPoint));
    //newChild->setPos(mapFromScene(QPointF(finalPoint.x() - qreal(STATEMENT_SIZE / 2),
                                          //finalPoint.y() - qreal(STATEMENT_SIZE / 2))));
    updateAncestors();
    return newChild;
}


/////////////////
/// Highlight ///
/////////////////

/*
 * Highlight this node
 */
void Node::setAsHighlight()
{
    highlighted = true;
    update();
}

/*
 * Clear highlighting from this node
 */
void Node::removeHighlight()
{
    highlighted = false;
    update();
}

/////////////////
/// Selection ///
/////////////////

/*
 * Select this node
 */
void Node::selectThis()
{
    selected = true;
    update();
}

/*
 * Deselect this node
 */
void Node::deselectThis()
{
    selected = false;
    update();
}

void Node::selectAllKids()
{
    for (Node* child : children)
        canvas->selectNode(child);
}

/*
 * Inverts whether this is selected or not
 */
void Node::toggleSelection()
{
    if (selected)
        canvas->deselectNode(this);
    else
        canvas->selectNode(this);
}

/*
 * (Static)
 * Working off a root node, (though technically can be any node), try and find
 * all decendents that are completely surrounded by the given selection box. All
 * these nodes then are selected automatically.
 *
 * If a node's drawBox is not completely surrounded, its children may have a
 * chance to be surrounded if it at least collides with the selBox. However, a
 * restriction is in place such that nodes closer to root have a higher priority
 * for selection than those deeper down. This will ensure that older nodes that
 * are closer to root aren't deselected in favor of decendents.
 *
 * There are some cases where it's still seemingly unintuitive, where selection
 * comes down to children list order, but these are unavoidable due to the
 * requirement that all selected nodes must share the same parent.
 */
void Node::setSelectionFromBox(Node* root, QRectF selBox)
{
    QList<Node*> queue;
    for (Node* c : root->children)
        queue.append(c);

    // For each "level" (i.e. nodes at the same depth that share a parent)
    while (true)
    {
        bool selOnThisLevel = false;
        QList<Node*> next;

        while (!queue.empty())
        {
            Node* curr = queue.first();
            queue.pop_front();

            // Try and find a node that is surrounded
            if (rectSurroundedBy(curr->getSceneDraw(), selBox))
            {
                selOnThisLevel = true;
                curr->canvas->selectNode(curr);
            }
            // If its not completely surrounded, but is colliding at least, then
            // its children need to be checked at the next level
            else if (rectsCollide(curr->getSceneDraw(), selBox))
                for (Node* c : curr->children)
                    next.append(c);
        }

        // Finished this level: if we haven't found any surrounded nodes but
        // found potential kids, select them for the next check
        if (!selOnThisLevel && !next.empty())
        {
            queue = next;
            next.clear();
        }
        else // Otherwise, we found a surrounded node or no kids, so quit
            return;
    }
}


////////////////
/// Graphics ///
////////////////

QRectF Node::boundingRect() const
{
    return drawBox;
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addRect(toCollision(drawBox));
    return path;
}

void Node::paint(QPainter* painter,
                 const QStyleOptionGraphicsItem* option,
                 QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (isRoot())
        return;

    if (isStatement())
        painter->setPen(QPen(QColor(0,0,0,0)));

    if (selected)
        painter->setBrush(QBrush(gradSelected));
    else
    {
        if (mouseDown)
            painter->setBrush(QBrush(gradClicked));
        else if (highlighted)
            painter->setBrush(QBrush(gradHighlighted));
        else
            painter->setBrush(QBrush(gradDefault));
    }

    if (ghost)
        setOpacity(0.5);
    else
        setOpacity(1.0);

    painter->drawRoundedRect(drawBox, qreal(BORDER_RADIUS), qreal(BORDER_RADIUS));

    if ( isStatement() )
    {
        painter->setPen(QPen(QColor(0,0,0,255)));
        painter->setFont(font);
        painter->drawText( drawBox, Qt::AlignCenter, letter );
    }
}

//////////////
/// Sizing ///
//////////////

/*
 * Converts the passed in QRectF (which should be a drawBox) to a collision box
 */
QRectF Node::toCollision(QRectF draw) const
{
    return QRectF( QPointF(draw.topLeft().x() - qreal(COLLISION_OFFSET),
                           draw.topLeft().y() - qreal(COLLISION_OFFSET)),
                   QPointF(draw.bottomRight().x() + qreal(COLLISION_OFFSET),
                           draw.bottomRight().y() + qreal(COLLISION_OFFSET)));
}

/*
 * Converts the passed in QRectF (which should be a collision box) to a drawBox
 */
QRectF Node::toDraw(QRectF coll) const
{
    return QRectF( QPointF(coll.topLeft().x() + qreal(COLLISION_OFFSET),
                           coll.topLeft().y() + qreal(COLLISION_OFFSET)),
                   QPointF(coll.bottomRight().x() - qreal(COLLISION_OFFSET),
                           coll.bottomRight().y() - qreal(COLLISION_OFFSET)));
}

/*
 * Returns a scene mapped collision box, offset by deltaX and deltaY if desired.
 *
 * deltaX and deltaY are given the default value of 0, so the params can be
 * ignored if only interested in the actual collision box and not a prediction
 */
QRectF Node::getSceneCollisionBox(qreal deltaX, qreal deltaY) const
{
    QRectF c = toCollision(drawBox);
    return QRectF(mapToScene( QPointF(c.topLeft().x() + deltaX,
                                      c.topLeft().y() + deltaY)),
                  mapToScene( QPointF(c.bottomRight().x() + deltaX,
                                      c.bottomRight().y() + deltaY )));
}

/*
 * Similar to getSceneCollisionBox, except for a drawBox instead. This isn't
 * super efficient implemented this way, but it's not a big deal and I'm lazy
 */
QRectF Node::getSceneDraw(qreal deltaX, qreal deltaY) const
{
    return toDraw(getSceneCollisionBox(deltaX, deltaY));
}

/*
 * Recursive function to update all draw boxes from child boxes.
 *
 * Call this on the parent of an added or deleted node, and it'll make sure to
 * update all the way up the tree
 */
void Node::updateAncestors()
{
    if (isRoot())
        return;

    QRectF myNewDrawBox = predictMySceneDraw(QList<Node*>(), QList<QRectF>());
    QRectF sceneDraw = getSceneDraw();

    canvas->addRedBound(myNewDrawBox);
    canvas->addBlueBound(sceneDraw);

    // Check if any changes occur
    if ( myNewDrawBox.left() != sceneDraw.left()   ||
         myNewDrawBox.top() != sceneDraw.top()     ||
         myNewDrawBox.right() != sceneDraw.right() ||
         myNewDrawBox.bottom() != sceneDraw.bottom() )
    {
        // New draw box, so we have to update it
        QPointF tl = mapFromScene(myNewDrawBox.topLeft());
        QPointF br = mapFromScene(myNewDrawBox.bottomRight());

        setDrawBoxFromPotential(QRectF(tl, br));
        parent->updateAncestors();
    }

}

//////////////////////////
/// Collision Checking ///
//////////////////////////

/*
 * Checks if the selection list sel can be moved by the relative delta given by
 * pt. If no collisions occur (including those potentially created if the parent
 * grows), then this function will update all the draw boxes of the parent,
 * grandparent, etc.
 *
 * Important note: this function will NOT alter the draw box of the selection
 * list. As these nodes are siblings and will not change in size, I believe it's
 * more efficient to simply move with the provided moveBy Qt function, rather
 * than needing to recalculate the whole draw box as is required for ancestor
 * nodes. This move should happen elsewhere, iff checkPotential returns true.
 *
 * Params:
 *   sel - the selection list (assumed to share a parent)
 *   pt - a QPointF storing a deltaX and deltaY value (in GRID_SPACING units)
 *
 * Returns:
 *   true - if no collisions occur; drawboxes are automatically updated, but the
 *          selection list is not moved
 *   false - if any collision occurs at any point traveling up the tree; in this
 *           case, no changes are made to any nodes
 */
bool Node::checkPotential(QList<Node*> changedNodes, QPointF pt)
{
    QList<QRectF> drawBoxes; // scene mapped

    for (Node* n : changedNodes)
        drawBoxes.append(n->getSceneDraw(pt.x(), pt.y()));

    QList<Node*> updateNodes;
    QList<QRectF> updateBoxes;

    if (changedNodes.empty() || changedNodes.first()->isRoot())
        return false;

    Node* parent = changedNodes.first()->parent;
    bool first = true;
    do
    {
        QList<QRectF> alteredDraws;

        // This is kind of confusing: basically, at each stage, check each of
        // the items in the changedNodes list against all of its siblings that
        // aren't also changed... i.e. check the moved nodes against the nodes
        // that don't move.
        QList<Node*>::iterator itn = changedNodes.begin();
        QList<QRectF>::iterator itr = drawBoxes.begin();

        for ( ; itn != changedNodes.end(); ++itn, ++itr)
        {
            Node* changed = (*itn);
            QRectF changedRect = (*itr);

            // Compare a changed node against each non changed node
            for (Node* n : parent->children)
            {
                if (!changedNodes.contains(n) )
                {
                    // Both statements, so only compare drawboxes (allows
                    // statements to be closer together visually)
                    if (changed->isStatement() && n->isStatement())
                    {
                        if (rectsCollide(n->getSceneDraw(),
                                         changedRect))
                            return false;
                    }
                    else
                    {
                        // Compare the collision boxes, since at least one of
                        // the nodes to compare is a cut
                        if (rectsCollide(n->getSceneCollisionBox(),
                                         changed->toCollision(changedRect)))
                            return false;

                    }
                }
            }

            // Store
            if (!first) // don't want to update the original nodes' drawBoxes
            {
                updateNodes.append(changed);
                updateBoxes.append(changedRect);
            }

            alteredDraws.append(changedRect);
        }

        // Quit early
        if (parent->isRoot())
            break;

        // Percolate up
        QRectF next = parent->predictMySceneDraw(changedNodes, alteredDraws);
        parent->canvas->addRedBound(next);

        changedNodes.clear();
        changedNodes.append(parent);

        drawBoxes.clear();
        drawBoxes.append(next);

        parent = parent->parent;
        first = false;
        // TODO: check if next rect is the same as its drawBox to save work
        // (honestly, it's probably not worth the effort to optimize here)
    }
    while (parent != nullptr);

    // Update all the nodes
    QList<Node*>::iterator itn = updateNodes.begin();
    QList<QRectF>::iterator itr = updateBoxes.begin();

    for (; itn != updateNodes.end(); ++itn, ++itr)
    {
        Node* n = (*itn);
        QRectF r = (*itr);
        n->setDrawBoxFromPotential(QRectF(n->mapFromScene(r.topLeft()),
                                          n->mapFromScene(r.bottomRight())));
    }

    return true;
}

/*
 * Takes in a set of nodes and their adjusted drawBoxes, and constructs my
 * resulting drawBox mapped to scene coords.
 *
 * Params:
 *   altNodes - nodes that have a different potential drawBox
 *   altDraws - parallel to altNodes, these are the scene-mapped potentials
 *
 * Returns:
 *   a QRectF of my drawBox, should those altNodes be moved to their altDraw
 *   locations
 *
 * NOTE/BUG:
 *   an empty children list is an unhandled bug -- no way currently to predict
 *   an empty cut if this is used for updating after a deletion of a node
 *    -- update: the edge case check should work to solve this now
 */
QRectF Node::predictMySceneDraw(QList<Node*> altNodes, QList<QRectF> altDraws)
{
    // Edge case
    if (children.empty() && isCut())
    {
        if (isCut())
        {
            // Return an empty cut
            QRectF sceneDraw = getSceneDraw();
            qreal cx = (sceneDraw.left() + sceneDraw.right()) / 2;
            qreal cy = (sceneDraw.top() + sceneDraw.bottom()) / 2;
            QPointF tl(cx - qreal(EMPTY_CUT_SIZE / 2),
                       cy - qreal(EMPTY_CUT_SIZE / 2));
            QPointF br(cx + qreal(EMPTY_CUT_SIZE / 2),
                       cy + qreal(EMPTY_CUT_SIZE / 2));
            return QRectF(tl, br);
        }
        else if (isStatement())
        {
            QRectF sceneDraw = getSceneDraw();
            qreal cx = (sceneDraw.left() + sceneDraw.right()) / 2;
            qreal cy = (sceneDraw.top() + sceneDraw.bottom()) / 2;
            QPointF tl(cx - qreal(STATEMENT_SIZE / 2),
                       cy - qreal(STATEMENT_SIZE / 2));
            QPointF br(cx + qreal(STATEMENT_SIZE / 2),
                       cy + qreal(STATEMENT_SIZE / 2));
            return QRectF(tl, br);
        }
    }


    qreal minX, minY, maxX, maxY;
    minX = minY = BIG_NUMBER;
    maxX = maxY = -BIG_NUMBER;

    for (Node* child : children)
    {
        QPointF tl, br;
        bool usedAlt = false;

        // Check if this child is in altNodes
        // NOTE: this is a bit convoluted because I used parallel lists. I
        // should have used a smarter data structure, or even a list of pairs
        // but oh well.
        QList<Node*>::iterator itn = altNodes.begin();
        QList<QRectF>::iterator itr = altDraws.begin();
        for (; itn != altNodes.end(); ++itn, ++itr)
        {
            Node* n = (*itn);
            QRectF r = (*itr);

            if (n == child)
            {
                usedAlt = true;
                tl = mapFromScene(r.topLeft());
                br = mapFromScene(r.bottomRight());
                break;
            }
        }

        // We didn't set tl & br yet, since this node wasn't an altNode,
        // therefore use the existing sceneDraw box for the coords instead
        if (!usedAlt)
        {
            tl = mapFromScene(child->getSceneDraw().topLeft());
            br = mapFromScene(child->getSceneDraw().bottomRight());
        }

        // Update the min and max values with these new points
        if (tl.x() < minX)
            minX = tl.x();
        if (tl.y() < minY)
            minY = tl.y();
        if (br.x() > maxX)
            maxX = br.x();
        if (br.y() > maxY)
            maxY = br.y();
    }

    // Calculated points are a draw box in parent coords
    QPointF tlp = QPointF(minX - qreal(GRID_SPACING),
                          minY - qreal(GRID_SPACING));
    QPointF brp = QPointF(maxX + qreal(GRID_SPACING),
                          maxY + qreal(GRID_SPACING));

    // Convert back to scene
    QPointF tls = mapToScene(tlp);
    QPointF brs = mapToScene(brp);
    return QRectF(tls, brs);
}

/*
 * If we determine a new drawBox is okay, call this function to set it for real.
 * This makes sure to update the bookkeeping required by Qt and schedule a
 * repaint.
 */
void Node::setDrawBoxFromPotential(QRectF potDraw)
{
    prepareGeometryChange();
    drawBox = potDraw;
}

/////////////
/// Mouse ///
/////////////

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (event->modifiers() & Qt::AltModifier)
        {
            ghost = true;
            //setZValue(Z_RAISED);
            raiseAllAncestors();
        }

        mouseOffset = event->pos();
        shadow->setEnabled(true);
        mouseDown = true;
        update();
    }
    else if (event->buttons() & Qt::RightButton)
    {
        //canvas->removeFromScene(this);
        canvas->selectNode(this);
        canvas->deleteSelection();
    }
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    mouseDown = false;
    shadow->setEnabled(false);

    canvas->clearBounds();
    canvas->clearDots();

    canvas->addRedBound(getSceneDraw());
    canvas->addBlackDot(scenePos());
    canvas->addBlackDot(getSceneDraw().topLeft());

    if (ghost)
    {
        ghost = false;
        lowerAllAncestors();

        if (newParent != parent)
        {
            // Remove us from the old
            parent->children.removeOne(this);
            qDebug() << "removed from old parent";
            parent->updateAncestors();

            // Put us in the new
            QRectF sceneDraw = getSceneDraw();
            canvas->addGreenBound(sceneDraw);
            parent = newParent;
            parent->children.append(this);

            if (parent->isRoot())
            {
                setParentItem(0); //this implicitly adds to scene
                // may be the source of the crash on exit
            }
            else
                setParentItem(parent);

            // Move us correctly
            QPointF tl, br;

            if (!parent->isRoot())
            {
                tl = parent->mapFromScene(sceneDraw.topLeft());
                br = parent->mapFromScene(sceneDraw.bottomRight());
            }
            else
            {
                tl = sceneDraw.topLeft();
                br = sceneDraw.bottomRight();
            }

            // Adjust for pos offset
            //qreal dx = tl.x() - scenePos().x();
            //qreal dy = tl.y() - scenePos().y();

            qreal dx = scenePos().x();
            qreal dy = scenePos().y();

            setPos(0, 0);

            dx -= scenePos().x();
            dy -= scenePos().y();

            qDebug() << "Pos changed by "
                     << dx
                     << ","
                     << dy;

            // This needs to percolate downward I think
            QQueue<Node*> updateQueue;
            for (Node* c : children)
                updateQueue.enqueue(c);

            while (!updateQueue.empty())
            {
                Node* curr = updateQueue.dequeue();
                qDebug() << "Adjusting node ";
                canvas->addBlueBound(curr->getSceneDraw());

                QPointF nPos = curr->scenePos();
                nPos.setX(nPos.x() + dx);
                nPos.setY(nPos.y() + dy);
                //curr->setPos(nPos);
                curr->setPos(curr->mapFromScene(nPos));

                for (Node* child : curr->children)
                    updateQueue.enqueue(child);

                canvas->addBlackBound(curr->getSceneDraw());
            }

            //tl.setX(tl.x() + dx);
            //tl.setY(tl.y() + dy);
            //br.setX(br.x() + dx);
            //br.setY(br.y() + dy);

            setDrawBoxFromPotential(QRectF(tl, br));
            //parent->update();
            //update();

            parent->updateAncestors();
        }
        else
        {
            qDebug() << "Moved around in same parent";
            parent->updateAncestors();
        }


    }

    //setZValue(Z_NORMAL);
    update();

    QGraphicsObject::mouseReleaseEvent(event);
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    canvas->setHighlight(this);
    QGraphicsObject::hoverEnterEvent(event);
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    canvas->setHighlight(parent);
    QGraphicsObject::hoverLeaveEvent(event);
}

/*
 * Dragging the mouse around will initiate the collision testing. If it finds a
 * valid target, it will visually move the node there.
 */
void Node::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (mouseDown)
    {
        // Adjusted with the mouseOffset, so that we are standardizing
        // calculations against the upper left corner
        qreal dx = mouseOffset.x();
        qreal dy = mouseOffset.y();

        // Work on this as a selected item
        QList<Node*> sel = canvas->selectionIncluding(this);
        QList<QPointF> scenePts;

        QPointF adj = QPointF(event->pos().x() - dx, event->pos().y() - dy);
        scenePts.append(mapToScene(adj));

        for (Node* n : sel)
        {
            if (n == this) // (covered above)
                continue;
            scenePts.append(n->getSceneDraw().topLeft());
        }

        // Construct potential points to check collision against
        QList<QPointF> bloom = constructBloom(scenePos(), mapToScene(adj));

        // No movement
        if (bloom.empty())
        {
            if (sel.size() == 1)
                canvas->clearSelection();
            return;
        }

        canvas->clearBounds();

        // Moved a lil bit at least, so lets check it
        for (QPointF pt : bloom)
        {
            // Check
            if (ghost)
            {
                Node* collider = determineNewParent(event->scenePos());
                newParent = collider;
                canvas->addBlueBound(collider->getSceneDraw());
                for (Node* n : sel)
                    n->moveBy(pt.x(), pt.y());
                if (sel.size() == 1)
                    canvas->clearSelection();
                return;
            }
            if (checkPotential(sel, pt))
            {
                for (Node* n : sel)
                    n->moveBy(pt.x(), pt.y());
                if (sel.size() == 1)
                    canvas->clearSelection();
                return;
            }
        }

        if (sel.size() == 1)
            canvas->clearSelection();
    }
}

/////////////////////////////
/// Change parent / ghost ///
/////////////////////////////

Node* Node::determineNewParent(QPointF pt)
{
    Node* root = canvas->getRoot();
    QList<Node*> pot = root->children;

    Node* collider = root;

    // TODO: rewrite this to avoid goto
loop:
    for (Node* n : pot)
    {
        if (n == this)
            continue;

        if (pointInRect(pt, n->getSceneDraw()))
        {
            collider = n;
            pot = collider->children;
            goto loop; // repeat loop with the new potential list
        }
    }

    return collider;
}

void Node::raiseAllAncestors()
{
    Node* curr = this;
    while (curr != nullptr && !curr->isRoot())
    {
        curr->setZValue(Z_RAISED);
        curr = curr->parent;
    }
}

void Node::lowerAllAncestors()
{
    Node* curr = this;
    while (curr != nullptr && !curr->isRoot())
    {
        curr->setZValue(Z_NORMAL);
        curr = curr->parent;
    }
}


///////////////
/// Helpers ///
///////////////

/*
 * Return the closest point on the snapping grid
 */
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

/*
 * Return the distance between two points
 */
qreal dist(const QPointF &a, const QPointF &b)
{
    return qSqrt( qPow( ( a.x() - b.x() ), 2) +
                  qPow( ( a.y() - b.y() ), 2) );
}

/*
 * Efficient way to determine if two rectangles collide. These rectangles must
 * be located
 */
bool rectsCollide(const QRectF &a, const QRectF &b)
{
    qreal ax1, ax2, ay1, ay2;
    a.getCoords(&ax1, &ay1, &ax2, &ay2);

    qreal bx1, bx2, by1, by2;
    b.getCoords(&bx1, &by1, &bx2, &by2);

    return ( ax1 < bx2 &&
             ax2 > bx1 &&
             ay1 < by2 &&
             ay2 > by1 );
}

/*
 * Debug function to print out (to qDebug) some text and a point's (x,y) coords
 */
void printPt(const QString &s, const QPointF &pt)
{
    qDebug() << s
             << pt.x()
             << ","
             << pt.y();
}


/*
 * Similar to printPt, but for rectangles
 */
void printRect(const QString &s, const QRectF &r)
{
    qDebug() << s
             << r.topLeft().x()
             << ","
             << r.topLeft().y()
             << ":"
             << r.bottomRight().x()
             << ","
             << r.bottomRight().y();
}

/*
 * Another print debug, but for four min max qreals
 */
void printMinMax(qreal minX, qreal minY, qreal maxX, qreal maxY)
{
    qDebug() << "min: ("
             << minX
             << ","
             << minY
             << ") | max: ("
             << maxX
             << ","
             << maxY
             << ")";
}


/*
 * scenePos: original / old position of the drawBox topleft point
 * sceneTarget: where the pos might move to (not yet snapped to grid)
 *
 * Note: both pos and target should be in scene coords
 *
 * Returns a list of "bloomed" potential points: basically giving the user a
 * little leeway in their target movement near collision boundaries. These
 * points are sorted such that the snapped target is first, but if that fails,
 * the rest of the bloomed points can be checked in a closest-to-original-pos
 * ordering.
 *
 * If the snapPoint is the same as the original pos, this function will return
 * an empty list (no movement).
 *
 * The points were originally absolute scene points (the contents of the bloom
 * list) -- however, with the multi-selection support, it is easier to
 * manipulate multiple items if they all work off deltas (the new relBloom list
 * that's returned).
 */
QList<QPointF> constructBloom(QPointF scenePos, QPointF sceneTarget)
{
    QPointF snapped = snapPoint(sceneTarget);

    QList<QPointF> bloom;

    if (snapped.x() == scenePos.x() && snapped.y() == scenePos.y())
        return bloom;

    bloom.append(QPointF(snapped.x() - qreal(GRID_SPACING), snapped.y()));
    bloom.append(QPointF(snapped.x(), snapped.y() - qreal(GRID_SPACING)));
    bloom.append(QPointF(snapped.x() + qreal(GRID_SPACING), snapped.y()));
    bloom.append(QPointF(snapped.x(), snapped.y() + qreal(GRID_SPACING)));

    std::sort(bloom.begin(), bloom.end(),
              [scenePos](const QPointF a, const QPointF b) ->
              bool { return dist(a, scenePos) < dist(b, scenePos); });

    bloom.prepend(snapped);

    // Convert to relative
    QList<QPointF> relBloom;
    for (QPointF pt : bloom)
        relBloom.append(QPointF(pt.x() - scenePos.x(),  pt.y() - scenePos.y()));

    return relBloom;
}


/*
 * Pt and rect should be in the same coordinate system
 */
bool pointInRect(const QPointF &pt, const QRectF &rect)
{
    return (pt.x() > rect.left() && pt.x() < rect.right() &&
            pt.y() > rect.top() && pt.y() < rect.bottom());
}

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
        QRectF potColl = toCollision(potDraw);

        qDebug() << "--- Checking point ---";
        // Collision Check
        for (Node* n : children)
        {
            // TODO: allow statements to be closer

            //QRectF otherColl;
            //QRectF newColl;
            //if (n->isStatement() && isStatement)
            //{
                //otherColl = n->getSceneDraw();
                //newColl = potDraw;
            //}
            //else
            //{
                //otherColl = n->getSceneCollisionBox();
                //newColl = potColl;
            //}

            if (rectsCollide(potColl, n->getSceneCollisionBox()))
            {
                collOkay = false;
                break;
            }
        }

        // TODO: growth check
        if (!isRoot())
        {
            QRectF sceneDraw = getSceneDraw();

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

// Assumed that they collide & share the same coord system
bool rectSurroundedBy(QRectF inside, QRectF outside)
{
    return  inside.left() >= outside.left() &&
            inside.top() >= outside.top() &&
            inside.right() <= outside.right() &&
            inside.bottom() <= outside.bottom();
}









































