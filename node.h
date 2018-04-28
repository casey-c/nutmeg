#ifndef NODE_H
#define NODE_H

#include <QPointF>
#include "canvas.h"

/*
 * The new node class. This base class ensures that we can maintain the graph
 * tree hierarchy correctly, and reuse the highlight / selection information
 * for all subclasses (root, statement, cut).
 *
 * Creation of nodes is handled by the static methods of NodeFactory. This class
 * contains some basic boilerplate functions for updating that the factory and
 * VisualNodes can hook into for redrawing.
 */
class Node
{
public:
    Node();

    void adoptChild(Node* child);
    //void moveMe(const QPointF& target); // TODO: put in VisualNode instead
    virtual void updateTree() {}

    // Highlight
    void setHighlight();
    void removeHighlight();

    // Tree relations
    Node* getParent() const { return parent; }
    Node* getRightSibling();
    Node* getLeftSibling();
    Node* getFirstChild();

    int getID() { return myID; }


    static void setSelectionFromBox(Node* root, QRectF selBox);

    bool canHaveKids() { return fertile; }

protected:
    bool fertile;
    Canvas* canvas;
    bool highlighted, selected;
    int myID;

private:
    static int globalID;

    Node* parent;
    QList<Node*> children;
};

#endif // NODE_H
