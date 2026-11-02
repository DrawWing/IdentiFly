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

#ifndef DWIMAGEVIEWER_H
#define DWIMAGEVIEWER_H

#include <QtWidgets>
#include <QMainWindow>

#include "dwView.h"

class dwImageViewer : public QMainWindow
{
    Q_OBJECT
public:
    explicit dwImageViewer(const QImage & inImg, QWidget *parent = 0);

signals:

public slots:
    void zoomIn();
    void zoomOut();

private:
    QAction *zoomInAct;
    QAction *zoomOutAct;

    View *view;
    qreal scale;
};

#endif // DWIMAGEVIEWER_H