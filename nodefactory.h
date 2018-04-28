#ifndef NODEFACTORY_H
#define NODEFACTORY_H

//#include "node.h"
#include "root.h"
#include "cut.h"
#include "statement.h"

/*
 * This class is responsible for managing the creation of nodes.
 */
class NodeFactory
{
public:
    NodeFactory();

    //static Node* makeRoot(Canvas* canvas);
    //static Node* addChildStatement(Node* targetParent, QString s);
    static Root* makeRoot(Canvas* canvas);
    static Cut* addChildCut(Canvas* canvas, Node* targetParent);
    static Statement* addChildStatement(Canvas* canvas, Node* targetParent, QString s);

    //static Node* surroundWithCut(Node* targetParent, QList<Node*> targets);
    //static Node* surroundWithDoubleCut(Node* targetParent, QList<Node*> targets);
};

#endif // NODEFACTORY_H
