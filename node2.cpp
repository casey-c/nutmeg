#include "node2.h"
#include "cut.h"

Node2::Node2()
{

}

Node2* Node2::addChildCut(QPointF scenePt) {
    // Try to find a suitable point using a predictor algorithm
    QList<QPointF> bloom = constructAddBloom(scenePt);
    QPointF predictedScenePt = findPoint(bloom,
                                qreal(EMPTY_CUT_SIZE),
                                qreal(EMPTY_CUT_SIZE));
    QPointF finalPt = mapFromScene(predictedScenePt);

    Cut* cut = new Cut(this, canvas, finalPt);

    children.append(cut);
    cut->setParentItem(this);

    return cut;
}

Node2* Node2::addChildStatement(QPointF scenePt){
    return nullptr;
}

Node2* Node2::addChildPlaceholder(QPointF scenePt){
    return nullptr;
}
