#ifndef MARKER_H
#define MARKER_H

#include <QtWidgets>

// class QGraphicsObject;

class Marker : public QGraphicsObject
{
public:
    Marker(int zValue);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
};

#endif
