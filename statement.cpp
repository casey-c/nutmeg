#include "statement.h"

Statement::Statement(QString s)
{
    fertile = false;
}

//Node2* Statement::addChildCut(QPointF scenePt) { return nullptr; }
//Node2* Statement::addChildStatement(QPointF scenePt) { return nullptr; }
//Node2* Statement::addChildPlaceholder(QPointF scenePt) { return nullptr; }

void Statement::paint(QPainter* painter,
           const QStyleOptionGraphicsItem* option,
           QWidget* widget) {}
