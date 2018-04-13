#include "nodefactory.h"
#include "root.h"
#include "statement.h"
#include "cut.h"

NodeFactory::NodeFactory() {

}

Node2* NodeFactory::makeRoot() {
    //return new Root(canvas);
    return nullptr;
}

Node2* NodeFactory::addChildCut(Node2* targetParent) {
    if (!targetParent->canHaveKids())
      return nullptr;

    Node2* cut = new Cut();
    targetParent->adoptChild(cut);
    targetParent->updateTree();

    return cut;
}

Node2* NodeFactory::addChildStatement(Node2* targetParent, QString s) {
    if (!targetParent->canHaveKids())
      return nullptr;

    Node2* statement = new Statement(s);
    targetParent->adoptChild(statement);
    targetParent->updateTree();

    return statement;
}
