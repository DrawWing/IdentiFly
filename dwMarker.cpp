#include "dwMarker.h"

Marker::Marker(int zValue)
{
    setZValue(zValue);
    setFlags(ItemIgnoresTransformations); // it is selectable and movable only in select and drag modes
}

QRectF Marker::boundingRect() const
{
    return QRectF(-10, -10, 50, 20);
}

void Marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);

    QColor fillColor = (option->state & QStyle::State_Selected) ? Qt::red : Qt::black;
    QColor crossColor = Qt::blue;

	painter->setPen(QPen(fillColor, 0));
    painter->drawEllipse(-10, -10, 20, 20);
    painter->setPen(QPen(crossColor, 0));
    painter->drawLine(0, -10, 0, 10);
	painter->drawLine(-10, 0, 10, 0);

    QFont painterFont("Helvetica", 16);
	painter->setFont(painterFont);
	QRect txtRect(12, -10, 30, 20);
	QString txt;
	int intZvalue = zValue();
    ++intZvalue; // values start with 1 not 0
	txt.setNum(intZvalue);
	painter->drawText(txtRect, Qt::AlignLeft | Qt::AlignVCenter, txt);
}
