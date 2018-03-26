#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <QColor>
#include <QBrush>

class ColorPalette
{
public:
    ColorPalette(QString bg1, QString bg2,
                 QString high1, QString high2,
                 QString stk, QString fnt);

    QBrush getBackgroundBrushNormal(QPointF center, qreal radius);
    QBrush getBackgroundBrushHighlight(QPointF center, qreal radius);

    const QColor& getStrokeColor();
    const QColor& getFontColor();

private:
    QColor bg1, bg2, high1, high2, stroke, font;
};

#endif // COLORPALETTE_H
