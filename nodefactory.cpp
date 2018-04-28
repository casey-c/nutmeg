#include "nodefactory.h"
#include "root.h"
#include "statement.h"
#include "cut.h"

NodeFactory::NodeFactory() {

}

Node* NodeFactory::makeRoot(Canvas* canvas) {
    return new Root(canvas);
}

Node* NodeFactory::addChildCut(Canvas* canvas, Node* targetParent) {
    if (!targetParent->canHaveKids())
      return nullptr;

    Node* cut = new Cut(canvas);
    targetParent->adoptChild(cut);
    //targetParent->updateTree();

    return cut;
}

Node* NodeFactory::addChildStatement(Node* targetParent, QString s) {
    if (!targetParent->canHaveKids())
      return nullptr;

    Node* statement = new Statement(s);
    targetParent->adoptChild(statement);
    //targetParent->updateTree();

    return statement;
}
