/*
    Copyright (C) 2026 Adam Tofilski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

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