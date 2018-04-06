#ifndef NODE2_H
#define NODE2_H

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
    Node* addChildCut(QPointF scenePt);
    Node* addChildStatement(QPointF scenePt);
    Node* addChildPlaceholder(QPointF scenePt);

    // Highlight
    void setHighlight();
    void removeHighlight();

    // Tree relations
    Node* getParent() const { return parent; }
    Node* getRightSibling();
    Node* getLeftSibling();
    Node* getFirstChild();

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
    Node* parent;
    QList<Node*> children;

    bool highlighted, selected;
};

#endif // NODE2_H
