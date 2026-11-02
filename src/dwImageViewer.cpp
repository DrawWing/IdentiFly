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

