#include "dwImageViewer.h"
#include "winginfo.h"

dwImageViewer::dwImageViewer(const QImage & inImg, QWidget *parent)
    : QMainWindow(parent)
{
    view = new View();
    view->setImg(inImg);

    WingInfo info;
    info.fromImg(inImg);
    view->setMarkers(info.getLandmarks());
    view->setMode(View::ViewMode);

    zoomInAct = new QAction(tr("Zoom &in"), this);
    zoomInAct->setShortcut(tr("Ctrl+Up"));
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
    addAction(zoomInAct);

    zoomOutAct = new QAction(tr("Zoom &out"), this);
    zoomOutAct->setShortcut(tr("Ctrl+Down"));
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
    addAction(zoomOutAct);

    setCentralWidget(view);
    adjustSize();
    scale = 1.0;
}

void dwImageViewer::zoomIn()
{
    scale*=1.1;
    QTransform transform;
    transform.scale(scale, scale);
    view->setTransform(transform);
}

void dwImageViewer::zoomOut()
{
    scale*=0.9;
    QTransform transform;
    transform.scale(scale, scale);
    view->setTransform(transform);
}


