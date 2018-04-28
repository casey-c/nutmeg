#include "nodefactory.h"
#include "root.h"
#include "statement.h"
#include "cut.h"

NodeFactory::NodeFactory() {

}

Root* NodeFactory::makeRoot(Canvas* canvas) {
    return new Root(canvas);
}

Cut* NodeFactory::addChildCut(Canvas* canvas, Node* targetParent) {
    if (!targetParent->canHaveKids())
      return nullptr;

    Cut* cut = new Cut(canvas);
    targetParent->adoptChild(cut);
    //targetParent->updateTree();

    return cut;
}

Statement* NodeFactory::addChildStatement(Canvas* canvas, Node* targetParent, QString s) {
    if (!targetParent->canHaveKids())
      return nullptr;

    Statement* statement = new Statement(canvas, s);
    targetParent->adoptChild(statement);
    //targetParent->updateTree();

    return statement;
}
