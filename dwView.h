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
    std::vector< QGraphicsItem * > itemsPtr; //duplikuje scene->items  byc moze niepotrzebne
    std::vector< QGraphicsLineItem * > lineItems;
    QGraphicsLineItem * scaleBarItem;
    std::vector< Coord > scaleBar;
    Mode theMode;
};

#endif
