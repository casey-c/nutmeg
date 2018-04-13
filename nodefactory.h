#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include "node2.h"

/*
 * This class is responsible for managing the creation of nodes.
 */
class NodeFactory
{
public:
    NodeFactory();

    static Node2* makeRoot();

    static Node2* addChildCut(Node2* targetParent);
    static Node2* addChildStatement(Node2* targetParent, QString s);

    static Node2* surroundWithCut(Node2* targetParent, QList<Node2*> targets);
    static Node2* surroundWithDoubleCut(Node2* targetParent, QList<Node2*> targets);
};

#endif // NODEFACTORY_H
