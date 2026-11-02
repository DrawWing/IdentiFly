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

#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>

#include "coord.h"
#include "dwRCoordList.h"

class View : public QGraphicsView
{
    Q_OBJECT
public:
    enum Mode{MoveMode, DragMode, AddMode, ViewMode};
    View(QWidget *parent = 0);
    void setImg(const QImage &);
    void clearImg();
    QPixmap getPixmap();
    void setOverlay(const QImage &);
    void clearOverlay();
    void setMarkers(const std::vector< Coord > &);
    void clearMarkers();
    void setDifferences(const dwRCoordList & inReference);
    void clearDifferences();
    void setMode(Mode);
    void deleteSelected();
    std::vector< Coord > landmarks() const; //change to getMarkers
    void setScaleBar();
    void setScaleBar(std::vector< Coord > inVec);
    std::vector< Coord > getScaleBar() const;
    void clearScaleBar();
    void clear();

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

public slots:
    void setupMatrix(int); //for zoom

private slots:
    void markerMoved();

signals:
    void mouseMoved(QPointF);
    void markersChanged();

private:
    void addMarker(QPointF);
    void setMarkersSelectable(bool selectable);

    QGraphicsScene *scene;
    QGraphicsPixmapItem * imgItem;
    QGraphicsPixmapItem * overItem;
    std::vector< QGraphicsItem * > itemsPtr; //duplicates scene->items; may be unnecessary
    std::vector< QGraphicsLineItem * > lineItems;
    QGraphicsLineItem * scaleBarItem;
    std::vector< Coord > scaleBar;
    Mode theMode;
};

#endif