#include "nodefactory.h"
#include "root.h"

NodeFactory::NodeFactory() {

}

Node2* NodeFactory::makeRoot() {
    //return new Root(canvas);
    return nullptr;
}

Node2* NodeFactory::addChildCut(Node2* targetParent) {
    return nullptr;
}

Node2* NodeFactory::addChildStatement(Node2* targetParent, QString s) {
    return nullptr;
}
