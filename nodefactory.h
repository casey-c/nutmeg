#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include "node.h"

/*
 * This class is responsible for managing the creation of nodes.
 */
class NodeFactory
{
public:
    NodeFactory();

    static Node* makeRoot(Canvas* canvas);

    static Node* addChildCut(Canvas* canvas, Node* targetParent);
    static Node* addChildStatement(Node* targetParent, QString s);

    static Node* surroundWithCut(Node* targetParent, QList<Node*> targets);
    static Node* surroundWithDoubleCut(Node* targetParent, QList<Node*> targets);
};

#endif // NODEFACTORY_H
