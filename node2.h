#ifndef NODE2_H
#define NODE2_H

#include <QPointF>
#include "canvas.h"

/*
 * The new node class. This base class ensures that we can maintain the graph
 * tree hierarchy correctly, and reuse the highlight / selection information
 * for all subclasses (root, statement, cut)
 */
class Node2
{
public:
    Node2();

    // Add children
    //virtual Node2* addChildCut(QPointF scenePt);
    //virtual Node2* addChildStatement(QPointF scenePt);
    //virtual Node2* addChildPlaceholder(QPointF scenePt);

    void adoptChild(Node2* babyboy);
    void moveMe(const QPointF& target);
    void updateTree();

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

private:
    static int globalID;
    int myID;

    Canvas* canvas;
    Node2* parent;
    QList<Node*> children;

    bool highlighted, selected;
};

#endif // NODE2_H
