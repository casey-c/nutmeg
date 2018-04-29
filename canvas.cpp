#include "canvas.h"
//#include "node.h"
#include <QKeyEvent>
#include <QDebug>
//#include <QGraphicsRectItem>
#include "nodefactory.h"
#include "visualnode.h"
#include "cut.h"

#include "nodefactory.h"

#define SEL_BOX_Z 10

Canvas::Canvas(QWidget* parent) :
    QGraphicsView(parent),
    mouseShiftPress(false),
    noMouseMovement(false),
    showBounds(true)
{
    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(400, 400);

    root = NodeFactory::makeRoot(this);
    highlighted = nullptr;

    // Selection box
    selBox = scene->addRect(QRectF(QPointF(0,0), QSizeF(0,0)));
    selBox->setZValue(SEL_BOX_Z);
    selBox->setVisible(false);
}

void Canvas::drawBackground(QPainter* painter, const QRectF &rect) {
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    //QRectF sceneRect = this->sceneRect();

    //QLinearGradient gradient(sceneRect.topLeft(),
    //sceneRect.bottomRight());
    //gradient.setColorAt(0, Qt::white);
    //gradient.setColorAt(1, QColor(Qt::lightGray).lighter(150));

    //painter->fillRect(rect.intersected(sceneRect),
    //gradient);

    //painter->setBrush(Qt::NoBrush);
    //painter->drawRect(sceneRect);
}

void Canvas::keyPressEvent(QKeyEvent* event) {
    QGraphicsView::keyPressEvent(event);

    if (event->modifiers() & Qt::ShiftModifier) {
        switch(event->key()) {
        case Qt::Key_X:
            addCut();
            break;
        }
    }
    else if (event->modifiers() & Qt::ControlModifier) {
        switch(event->key()) {
        case Qt::Key_A:
            //highlighted->selectAllKids();
            break;
        case Qt::Key_B: {
            showBounds = !showBounds;
            qDebug() << "toggle showBounds to" << showBounds;
            if (!showBounds)
                clearBounds();
            break;
            }
        case Qt::Key_D:
            //clearSelection();
            break;
        }
    }

    else {
        switch(event->key()) {
        case Qt::Key_A: addStatement("A"); break;
        case Qt::Key_B: addStatement("B"); break;
        case Qt::Key_C: addStatement("C"); break;
        case Qt::Key_D: addStatement("D"); break;
        case Qt::Key_E: addStatement("E"); break;
        case Qt::Key_F: addStatement("F"); break;
        case Qt::Key_G: addStatement("G"); break;
        case Qt::Key_H: addStatement("H"); break;
        case Qt::Key_I: addStatement("I"); break;
        case Qt::Key_J: addStatement("J"); break;
        case Qt::Key_K: addStatement("K"); break;
        case Qt::Key_L: addStatement("L"); break;
        case Qt::Key_M: addStatement("M"); break;
        case Qt::Key_N: addStatement("N"); break;
        case Qt::Key_O: addStatement("O"); break;
        case Qt::Key_P: addStatement("P"); break;
        case Qt::Key_Q: addStatement("Q"); break;
        case Qt::Key_R: addStatement("R"); break;
        case Qt::Key_S: addStatement("S"); break;
        case Qt::Key_T: addStatement("T"); break;
        case Qt::Key_U: addStatement("U"); break;
        case Qt::Key_V: addStatement("V"); break;
        case Qt::Key_W: addStatement("W"); break;
        case Qt::Key_X: addStatement("X"); break;
        case Qt::Key_Y: addStatement("Y"); break;
        case Qt::Key_Z: addStatement("Z"); break;
        //case Qt::Key_4: addPlaceholder(); break;
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (mouseShiftPress) {
        QPointF pt = mapToScene(event->pos());

        qreal minX = qMin(pt.x(), selStart.x());
        qreal minY = qMin(pt.y(), selStart.y());

        qreal width = qAbs(pt.x() - selStart.x());
        qreal height = qAbs(pt.y() - selStart.y());

        // Give a little leeway for clicking
        if (width > 2 || height > 2)
            noMouseMovement = false;

        selBox->setRect(QRectF(QPointF(minX, minY), QSize(width, height)));

        selBox->setVisible(true);
    }
    else
        lastMousePos = mapToScene(event->pos());

    QGraphicsView::mouseMoveEvent(event);
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (event->modifiers() & Qt::ShiftModifier) {
        mouseShiftPress = true;
        noMouseMovement = true;
        selStart = mapToScene(event->pos());
    }
    else {
        //if (highlighted == root)
        //clearSelection();
        QGraphicsView::mousePressEvent(event);
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
#if 0
    if (mouseShiftPress)
    {
        mouseShiftPress = false;
        selBox->setVisible(false);

        if (noMouseMovement)
        {
            qDebug() << "No mouse movement, handle as click";
            if (!highlighted->isRoot())
                highlighted->toggleSelection();
        }
        else
        {
            qDebug() << "Need to determine selection";
            Node::setSelectionFromBox(root, selBox->rect());
        }

    }

#endif
    QGraphicsView::mouseReleaseEvent(event);
}

void Canvas::setHighlight(VisualNode* node) {
    removeHighlight();

    highlighted = node;
    highlighted->setHighlight();
    highlighted->update();
}

void Canvas::removeHighlight(){
    qDebug() << "canvas clearing highlight";
    if (highlighted != nullptr) {
        highlighted->removeHighlight();
        highlighted->update();
    }
    highlighted = nullptr;
}

void Canvas::addCut() {
    qDebug() << "attempting to add cut";

    Cut* cut;
    if (highlighted == nullptr)
        cut = NodeFactory::addChildCut(this, root);
    else
        cut = NodeFactory::addChildCut(this, highlighted);

    // Not possible to create
    if (cut == nullptr)
        return;

    // Perform move
    cut->drawMeHere(lastMousePos);

    if (highlighted == nullptr)
        scene->addItem(cut);
    else {
        qDebug() << "parent NOT root";
        // TODO: race condition - highlighted may have changed since cut built
        cut->setParentItem(highlighted);
        // percolate changes
        highlighted->updateTree();
    }

    //setHighlight(cut);
}

void Canvas::addStatement(QString s) {
    qDebug() << "attempting to add statement";

    Statement* statement;
    if (highlighted == nullptr)
        statement = NodeFactory::addChildStatement(this, root, s);
    else
        statement = NodeFactory::addChildStatement(this, highlighted, s);

    // Not possible to create
    if (statement == nullptr)
        return;

    statement->drawMeHere(lastMousePos);

    if (statement->getParent() == root)
        scene->addItem(statement);
    else { // potential race condition, see above
        statement->setParentItem(highlighted);
        highlighted->updateTree();
    }

}

void Canvas::addPlaceholder() {
#if 0
    Node* n = highlighted->addChildStatement(lastMousePos, "");
    if (n == nullptr)
        return;

    if (n->getParent() == root)
        scene->addItem(n);

    setHighlight(n);
#endif
}





// Selection
/*
void Canvas::clearSelection()
{
for (Node* n : selectedNodes)
n->removeSelect();

selectedNodes.clear();
}

void Canvas::selectNode(Node* n)
{
// Make sure we don't add the same node twice
if (selectedNodes.contains(n))
return;

// Ensure all nodes in the selection share the same parent
if (!selectedNodes.empty())
{
Node* parent = selectedNodes.first()->getParent();
if (n->getParent() != parent)
clearSelection();
}

n->setSelect();
selectedNodes.append(n);
}

void Canvas::deselectNode(Node* n)
{
selectedNodes.removeOne(n);
n->removeSelect();
}

QList<Node*> Canvas::getSelectedNodes()
{
return selectedNodes;
}

QList<Node*> Canvas::selectionIncluding(Node* n)
{
if (selectedNodes.contains(n))
return selectedNodes;

clearSelection();
selectNode(n);
return selectedNodes;
}

bool Canvas::hasAnySelectedNodes()
{
return !selectedNodes.empty();
}
*/


void Canvas::removeFromScene(Node* n) {
    Q_UNUSED(n)
#if 0
    if (n->isRoot())
        return;

    scene->removeItem(n);
    highlighted = n->getParent();
    delete n;
#endif
}

/*
void Canvas::deleteSelection()
{
#if 0
if (selectedNodes.empty() || selectedNodes.first()->isRoot() )
return;

// Update highlight
highlighted = selectedNodes.first()->getParent();

// Delete everything
for (Node* n : selectedNodes)
{
scene->removeItem(n);
delete n;
}
selectedNodes.clear();
#endif
}
*/
