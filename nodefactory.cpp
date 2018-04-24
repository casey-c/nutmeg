#include "nodefactory.h"
#include "root.h"
#include "statement.h"
#include "cut.h"

NodeFactory::NodeFactory() {

}

Node* NodeFactory::makeRoot() {
    //return new Root(canvas);
    return nullptr;
}

Node* NodeFactory::addChildCut(Node* targetParent) {
    if (!targetParent->canHaveKids())
      return nullptr;

    Node* cut = new Cut();
    targetParent->adoptChild(cut);
    targetParent->updateTree();

    return cut;
}

Node* NodeFactory::addChildStatement(Node* targetParent, QString s) {
    if (!targetParent->canHaveKids())
      return nullptr;

    Node* statement = new Statement(s);
    targetParent->adoptChild(statement);
    targetParent->updateTree();

    return statement;
}
