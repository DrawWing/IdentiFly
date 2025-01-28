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
