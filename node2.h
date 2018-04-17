#ifndef NODE2_H
#define NODE2_H

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
class Node2
{
public:
    Node2();

    void adoptChild(Node2* child);
    //void moveMe(const QPointF& target); // TODO: put in VisualNode instead
    virtual void updateTree() {}

    // Highlight
    void setHighlight();
    void removeHighlight();

    // Tree relations
    Node2* getParent() const { return parent; }
    Node2* getRightSibling();
    Node2* getLeftSibling();
    Node2* getFirstChild();

    // Selection
    void toggleSelection();
    void setSelect();
    void setSelectAllKids();
    void removeSelect();

    static void setSelectionFromBox(Node* root, QRectF selBox);

    bool canHaveKids() { return fertile; }

protected:
    bool fertile;

private:
    static int globalID;
    int myID;

    Canvas* canvas;
    Node2* parent;
    QList<Node2*> children;

    bool highlighted, selected;
};

#endif // NODE2_H
