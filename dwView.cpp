#include "dwView.h"
#include "dwMarker.h"

View::View(QWidget *parent) 
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing, false);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    scene = new QGraphicsScene;
    setScene(scene);
    imgItem = scene->addPixmap(QPixmap::fromImage(QImage()));
    imgItem->setZValue(-2); //move to background
    overItem = scene->addPixmap(QPixmap::fromImage(QImage()));
    overItem->setZValue(-1); //move to background
    setMode(MoveMode);
    scaleBarItem = 0;

    setMouseTracking(true); // for position of pointer    
}

void View::setupMatrix(int value)
{
    qreal scale = value / 100.0;
    QTransform transform;
    transform.scale(scale, scale);
    setTransform(transform);

    //scale differences
    double width = 2.0/scale;
    QPen pen(Qt::yellow, width, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    for( unsigned i = 0; i < lineItems.size(); ++i)
    {
        QGraphicsLineItem * item = lineItems[i];
        item->setPen(pen);
    }
}

void View::setImg(const QImage &img)
{
    imgItem->setPixmap(QPixmap::fromImage(img));
    scene->setSceneRect(img.rect()); // in consequence sroll bars are of this size
}

void View::clearImg()
{
    imgItem->setPixmap(QPixmap::fromImage(QImage()));
}

QPixmap View::getPixmap()
{
    return imgItem->pixmap();
}

void View::setOverlay(const QImage &img)
{
    overItem->setPixmap(QPixmap::fromImage(img));
}

void View::clearOverlay()
{
    overItem->setPixmap(QPixmap::fromImage(QImage()));
}

// add lansmarks on the scene.
void View::setMarkers(const std::vector< Coord > & landmarks)
{
    clearMarkers();
    for(unsigned i  = 0; i < landmarks.size(); ++i)
    {
        QGraphicsObject * item = new Marker(i);
        itemsPtr.push_back(item);
        Coord pxl = landmarks[i];
        item->setPos(QPointF(pxl.dx(), pxl.dy()));
        if(theMode == AddMode)
            item->setFlags(QGraphicsItem::ItemIgnoresTransformations);
        else
            item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIgnoresTransformations);
        scene->addItem(item);
        connect(item, SIGNAL(xChanged()), this, SLOT(markerMoved()));
        connect(item, SIGNAL(yChanged()), this, SLOT(markerMoved()));
    }

}

void View::markerMoved()
{
    emit markersChanged();
}

void View::setMarkersSelectable(bool selectable)
{
    for(unsigned i = 0; i < itemsPtr.size(); ++i)
    {
        QGraphicsItem * item = itemsPtr[i];
        if(selectable)
            item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIgnoresTransformations);
        else
            item->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    }
}

void View::clearMarkers()
{
    while(itemsPtr.size())
    {
        QGraphicsItem * item = itemsPtr.back();
        scene->removeItem(item);
        itemsPtr.pop_back();
    }
}

void View::setDifferences(const dwRCoordList & inReference)
{
    dwRCoordList reference = inReference; //keep it constant
    clearDifferences();

    std::vector< Coord > conVec = landmarks();
    if(conVec.size() != reference.size())
        return;
    dwRCoordList configuration(conVec);
    reference.rotate2reference(configuration);
    realCoord conCentroid = configuration.centroid(); // before is modified by cs
    reference.scale(configuration.centroidSize()); //centers configuration
    reference.add(conCentroid);
    std::vector< realCoord > refVec = reference.list();

    for( unsigned i = 0; i < refVec.size(); ++i)
    {
        QGraphicsItem * landmark = itemsPtr[i];
        QPointF from = landmark->pos();
        realCoord pxl = refVec[i];
        QPointF to(pxl.dx(), pxl.dy());
        QLineF line(from, to);

        double scale = 2.0/transform().m11();
        QPen pen(Qt::yellow, scale, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
        QGraphicsLineItem * item = scene->addLine(line, pen);
        lineItems.push_back(item);
    }
}

void View::clearDifferences()
{
    while(lineItems.size())
    {
        QGraphicsLineItem * item = lineItems.back();
        scene->removeItem(item);
        lineItems.pop_back();
    }
}

void View::setMode(Mode newMode)
{
    theMode = newMode;
    if(theMode == AddMode)
    {
        setDragMode(QGraphicsView::NoDrag);
        setCursor(Qt::CrossCursor);
        scene->clearSelection();
        setMarkersSelectable(false);
    }
    else if(theMode == DragMode)
    {
        setDragMode(QGraphicsView::ScrollHandDrag);
        setMarkersSelectable(true);
    }
    else if(theMode == MoveMode)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
        setCursor(Qt::ArrowCursor);
        setMarkersSelectable(true);
    }
    else if(theMode == ViewMode) // for image viewer
    {
        setDragMode(QGraphicsView::NoDrag);
        setCursor(Qt::ArrowCursor);
        scene->clearSelection();
        setMarkersSelectable(false);
    }
}

void View::deleteSelected()
{
    QList<QGraphicsItem *> selected = scene->selectedItems ();
    if(selected.size() == 0)
        return;
    while(selected.size())
    {
        QGraphicsItem * item = selected.back();
        scene->removeItem(item);
        std::vector< QGraphicsItem * >::iterator iter;
        for(iter = itemsPtr.begin(); iter != itemsPtr.end(); iter++)
        {
            if(*iter == item)
            {
                itemsPtr.erase(iter);
                break;
            }
        }
        selected.pop_back();
    }
    //in order to renumber landmarks all are removed and remarked
    std::vector< Coord > newList = landmarks();
    clearMarkers();
    setMarkers(newList);
    emit markersChanged();
}

void View::addMarker(QPointF pos)
{
    QGraphicsItem * item = new Marker(itemsPtr.size());
    itemsPtr.push_back(item);
    item->setPos(pos);
    scene->addItem(item);
    emit markersChanged();
}

std::vector< Coord > View::landmarks() const
{
    std::vector< Coord > outList;
    for(unsigned i  = 0; i < itemsPtr.size(); ++i)
    {
        QGraphicsItem * item = itemsPtr[i];
        QPointF qPxl = item->pos();
        Coord pxl(qPxl.rx(), qPxl.ry());
        outList.push_back(pxl);
    }
    return outList;
}

void View::setScaleBar()
{
    if(scaleBarItem != 0)
        return; //scalr bar exists

    //find last two landmarks and convert them to scale
    // end scale point
    QGraphicsItem * item = itemsPtr.back();
    QPointF scaleEnd = item->pos();
    scene->removeItem(item);
    itemsPtr.pop_back();
    // start scale point
    item = itemsPtr.back();
    QPointF scaleStart = item->pos();
    scene->removeItem(item);
    itemsPtr.pop_back();
    // add line
    QLineF line(scaleStart, scaleEnd);
    QPen pen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    scaleBarItem = scene->addLine(line, pen);
    Coord endCoord(scaleEnd.x(), scaleEnd.y());
    Coord startCoord(scaleStart.x(), scaleStart.y());
    scaleBar.push_back(startCoord);
    scaleBar.push_back(endCoord);
}

void View::setScaleBar(std::vector< Coord > inVec)
{
    if(inVec.size()!=2)
        return;
    scaleBar = inVec;
    // start scale point
    Coord startCoord = inVec[0];
    QPointF scaleStart(startCoord.dx(), startCoord.dy());
    // end scale point
    Coord endCoord = inVec[1];
    QPointF scaleEnd(endCoord.dx(), endCoord.dy());
    // add line
    QLineF line(scaleStart, scaleEnd);
    QPen pen(Qt::red, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    scaleBarItem = scene->addLine(line, pen);
}

std::vector< Coord > View::getScaleBar() const
{
    return scaleBar;
}

void View::clearScaleBar()
{
    if(scaleBarItem == 0)
        return;
    scene->removeItem(scaleBarItem);
    scaleBarItem = 0;
    scaleBar.clear();
}

void View::clear()
{
    clearImg();
    clearOverlay();
    clearMarkers();
    clearDifferences();
    clearScaleBar();
}

void View::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    QPoint position = event->pos();
    QPointF positionF = mapToScene(position);
    if(theMode == AddMode)
        addMarker(positionF);
}

void View::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    QPoint position = event->pos();
    QPointF positionF = mapToScene(position);
    emit mouseMoved(positionF);
}
