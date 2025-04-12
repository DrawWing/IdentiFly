#include <QApplication>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>

#include <QDebug> // qDebug

#include "dwOptions.h"
#include "dwRclList.h"
#include "dwView.h"
#include "mainwindow.h"
#include "outline.h" // crop
#include "thresholdDialog.h" // crop
#include "TPS.h"
#include "txtwindow.h"
#include "dwAdjust.h"
#include "dwPairLR.h"
#include "dwldagm.h"
#include "dwImageViewer.h"
#include "dwFileRename.h"

#include "dwSLAO.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    view = new View(this);
    setCentralWidget(view);

    createActions();
    createMenus();
    createToolBars();
    readSettings(); // has to be after createActions
    createStatusBar();
    moveMode();
    clear();

    setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow() {}

// clear main window
void MainWindow::clear()
{
    reference.clear(); //reset the reference, do not show lines
    differencesLR = false; // do not show differences between left adn right
    screenImg = QImage();
    overImg = QImage();
    inFileInfo = QFileInfo();
    fileInfoList.clear();
    view->clear();
    setWindowTitle("");
    resetActions();
}

void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/open-green.png"), tr("&Open"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    openDirAct = new QAction(tr("Open &directory"), this);
    openDirAct->setShortcut(tr("Ctrl+D"));
    connect(openDirAct, SIGNAL(triggered()), this, SLOT(openDir()));

    openNextAct = new QAction(QIcon(":/next-green.png"), tr("Open &next file"), this);
    openNextAct->setShortcut(tr("Ctrl+Right"));
    connect(openNextAct, SIGNAL(triggered()), this, SLOT(openNext()));
    openNextAct->setEnabled(false);

    openPrevAct = new QAction(QIcon(":/prev-green.png"), tr("Open &previous file"), this);
    openPrevAct->setShortcut(tr("Ctrl+Left"));
    connect(openPrevAct, SIGNAL(triggered()), this, SLOT(openPrev()));
    openPrevAct->setEnabled(false);

    saveAct = new QAction(QIcon(":/save-green.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
    connect(view, SIGNAL(markersChanged()), this, SLOT(saveOn()));

    saveAsAct = new QAction(tr("Save &as"), this);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    removeFileAct = new QAction(QIcon(":/trash-green.png"), tr("&Move to trash"), this);
    removeFileAct->setShortcut(tr("Ctrl+T"));
    connect(removeFileAct, SIGNAL(triggered()), this, SLOT(removeFile()));

    importTpsAct = new QAction(tr("&Import TPS"), this);
    connect(importTpsAct, SIGNAL(triggered()), this, SLOT(importTps()));

    exportDataAct = new QAction(tr("&Export"), this);
    connect(exportDataAct, SIGNAL(triggered()), this, SLOT(exportData()));

    exportDataDirAct = new QAction(tr("Export di&rectory"), this);
    connect(exportDataDirAct, SIGNAL(triggered()), this, SLOT(exportDataDir()));

    preferencesAct = new QAction(tr("&Preferences"), this);
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(preferences()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    //connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    addModeAct = new QAction(QIcon(":/arrowplus-green.png"), tr("&Add mode"), this);
    connect(addModeAct, SIGNAL(triggered()), this, SLOT(addMode()));
    addModeAct->setEnabled(true);
    addModeAct->setCheckable(true); //works only when action is anabled

    dragModeAct = new QAction(QIcon(":/arrowhand-green.png"), tr("&Drag mode"), this);
    connect(dragModeAct, SIGNAL(triggered()), this, SLOT(dragMode()));
    dragModeAct->setEnabled(true);
    dragModeAct->setCheckable(true); //works only when action is anabled

    moveModeAct = new QAction(QIcon(":/arrow-green.png"), tr("&Select mode"), this);
    connect(moveModeAct, SIGNAL(triggered()), this, SLOT(moveMode()));
    moveModeAct->setEnabled(true);
    moveModeAct->setCheckable(true); //works only when action is anabled

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    //
    flipHorAct = new QAction(tr("Flip &horizontally"), this);
    flipHorAct->setShortcut(tr("Ctrl+H"));
    connect(flipHorAct, SIGNAL(triggered()), this, SLOT(flipHor()));

    flipVerAct = new QAction(tr("Flip &vertically"), this);
    flipVerAct->setShortcut(tr("Ctrl+V"));
    connect(flipVerAct, SIGNAL(triggered()), this, SLOT(flipVer()));

    rotateAct = new QAction(tr("&Rotate"), this);
    connect(rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));

    grayscaleAct = new QAction(tr("Convert to &grayscale"), this);
    grayscaleAct->setShortcut(tr("Ctrl+G"));
    connect(grayscaleAct, SIGNAL(triggered()), this, SLOT(grayscale()));

    cropAct = new QAction(tr("&Crop"), this);
    connect(cropAct, SIGNAL(triggered()), this, SLOT(crop()));

    grayscaleDirAct = new QAction(tr("Convert to grayscale &directory"), this);
    connect(grayscaleDirAct, SIGNAL(triggered()), this, SLOT(grayscaleDir()));

    cropDirAct = new QAction(tr("Crop d&irectory"), this);
    connect(cropDirAct, SIGNAL(triggered()), this, SLOT(cropDir()));

    scaleDirAct = new QAction(tr("Scale director&y"), this);
    connect(scaleDirAct, SIGNAL(triggered()), this, SLOT(scaleDir()));

    zoomInAct = new QAction(QIcon(":/zoomin-green.png"), tr("Zoom &in"), this);
    zoomInAct->setShortcut(tr("Ctrl+Up"));
    zoomInAct->setAutoRepeat(true);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(QIcon(":/zoomout-green.png"), tr("Zoom &out"), this);
    zoomOutAct->setShortcut(tr("Ctrl+Down"));
    zoomOutAct->setAutoRepeat(true);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    zoomFitAct = new QAction(tr("Zoom to &fit window"), this);
    zoomFitAct->setShortcut(tr("Ctrl+0"));
    connect(zoomFitAct, SIGNAL(triggered()), this, SLOT(zoomFit()));

    zoomActualAct = new QAction(tr("Zoom to &actual size"), this);
    zoomActualAct->setShortcut(tr("Ctrl+1"));
    connect(zoomActualAct, SIGNAL(triggered()), this, SLOT(zoomActual()));

    zoomLandmarksAct = new QAction(tr("Zoom &landmarks"), this);
    zoomLandmarksAct->setShortcut(tr("Ctrl+2"));
    connect(zoomLandmarksAct, SIGNAL(triggered()), this, SLOT(zoomLandmarks()));

    zoom3Act = new QAction(tr("Zoom &3"), this);
    zoom3Act->setShortcut(tr("Ctrl+3"));
    connect(zoom3Act, SIGNAL(triggered()), this, SLOT(zoom3()));

    zoom4Act = new QAction(tr("Zoom &4"), this);
    zoom4Act->setShortcut(tr("Ctrl+4"));
    connect(zoom4Act, SIGNAL(triggered()), this, SLOT(zoom4()));

    // edgeAct = new QAction(tr("&Edge"), this);
    // edgeAct->setShortcut(tr("Ctrl+E"));
    // connect(edgeAct, SIGNAL(triggered()), this, SLOT(addOverlay()));

    dataLmakAct = new QAction(tr("&Landmarks"), this);
    connect(dataLmakAct, SIGNAL(triggered()), this, SLOT(dataLmak()));

    dataCsAct = new QAction(tr("Centroid &size"), this);
    connect(dataCsAct, SIGNAL(triggered()), this, SLOT(dataCs()));

    dataDistAct = new QAction(tr("&Distance"), this);
    connect(dataDistAct, SIGNAL(triggered()), this, SLOT(dataDist()));

    lmarkDelAct = new QAction(QIcon(":/arrowminus-green.png"), tr("&Delete landmark"), this);
    lmarkDelAct->setShortcut(tr("Del"));
    connect(lmarkDelAct, SIGNAL(triggered()), this, SLOT(lmarkDel()));

    lmarkResetAct = new QAction(tr("&Reset"), this);
    lmarkResetAct->setShortcut(tr("Ctrl+R"));
    connect(lmarkResetAct, SIGNAL(triggered()), this, SLOT(lmarkReset()));

    lmarkResetDirAct = new QAction(tr("&Reset - directory"), this);
    connect(lmarkResetDirAct, SIGNAL(triggered()), this, SLOT(lmarkResetDir()));

    lmarkFitAct = new QAction(tr("&Fit"), this);
    lmarkFitAct->setShortcut(tr("Ctrl+F"));
    connect(lmarkFitAct, SIGNAL(triggered()), this, SLOT(lmarkFit()));

    lmarkFitDirAct = new QAction(tr("&Fit - directory"), this);
    connect(lmarkFitDirAct, SIGNAL(triggered()), this, SLOT(lmarkFitDir()));

    lmarkOutAct = new QAction(tr("&Outliers"), this);
    connect(lmarkOutAct, SIGNAL(triggered()), this, SLOT(lmarkOut()));

    lmarkOneOutAct = new QAction(tr("&One outlier"), this);
    connect(lmarkOneOutAct, SIGNAL(triggered()), this, SLOT(lmarkOneOut()));

    lmarkPairOutAct = new QAction(tr("&Pair outliers"), this);
    connect(lmarkPairOutAct, SIGNAL(triggered()), this, SLOT(lmarkPairOut()));

    protoSetAct = new QAction(tr("&Set prototype"), this);
    connect(protoSetAct, SIGNAL(triggered()), this, SLOT(protoSet()));

    protoViewAct = new QAction(tr("&View prototype"), this);
    connect(protoViewAct, SIGNAL(triggered()), this, SLOT( protoView() ));

    semiOutlineAct = new QAction(tr("&Longest outline"), this);
    connect(semiOutlineAct, SIGNAL(triggered()), this, SLOT(semiOutline()));

    semiOutlineDirAct = new QAction(tr("&Longest outline - directory"), this);
    connect(semiOutlineDirAct, SIGNAL(triggered()), this, SLOT(semiOutlineDir()));

    semiLandAct = new QAction(tr("Semilandmarks &1 point"), this);
    connect(semiLandAct, SIGNAL(triggered()), this, SLOT(semiLand1()));

    semiLand2Act = new QAction(tr("Semilandmarks &2 points"), this);
    connect(semiLand2Act, SIGNAL(triggered()), this, SLOT(semiLand2()));

    semiLand2DirAct = new QAction(tr("Semilandmarks &2 points - directory"), this);
    connect(semiLand2DirAct, SIGNAL(triggered()), this, SLOT(semiLand2Dir()));

    semiAlignAct = new QAction(tr("Align semilandmarks"), this);
    connect(semiAlignAct, SIGNAL(triggered()), this, SLOT(semiAlign()));

    setScaleBarAct = new QAction(tr("&Set scale bar"), this);
    connect(setScaleBarAct, SIGNAL(triggered()), this, SLOT(setScaleBar()));

    clearScaleBarAct = new QAction(tr("&Clear scale bar"), this);
    connect(clearScaleBarAct, SIGNAL(triggered()), this, SLOT(clearScaleBar()));

    setResolutionAct = new QAction(tr("Set &resolution"), this);
    connect(setResolutionAct, SIGNAL(triggered()), this, SLOT(setResolution()));

    setResolutionDirAct = new QAction(tr("Set resolution to &directory"), this);
    connect(setResolutionDirAct, SIGNAL(triggered()), this, SLOT(setResolutionDir()));

    scaleInfoAct = new QAction(tr("Scale &information"), this);
    connect(scaleInfoAct, SIGNAL(triggered()), this, SLOT(scaleInfo()));

    idFileAct = new QAction(tr("&Set classification"), this);
    connect(idFileAct, SIGNAL(triggered()), this, SLOT( idFile() ));

    idAct = new QAction(tr("&Classify "), this);
    connect(idAct, SIGNAL(triggered()), this, SLOT( id() ));

    idDirAct = new QAction(tr("Classify &directory"), this);
    connect(idDirAct, SIGNAL(triggered()), this, SLOT( idDir() ));

    idDirAllAct = new QAction(tr("Classify files in directory"), this);
    connect(idDirAllAct, SIGNAL(triggered()), this, SLOT( idDirAll() ));

    idDirDirAct = new QAction(tr("Classify directories in directory"), this);
    connect(idDirDirAct, SIGNAL(triggered()), this, SLOT( idDirDir() ));

    idDataAct = new QAction(tr("Classify data"), this);
    connect(idDataAct, SIGNAL(triggered()), this, SLOT( idData() ));

    QString aboutStr = tr("&About %1").arg(QGuiApplication::applicationDisplayName());
    aboutDwAct = new QAction(aboutStr, this);
    connect(aboutDwAct, SIGNAL(triggered()), this, SLOT(about()));

    getImageTextAct = new QAction(tr("&Image text"), this);
    connect(getImageTextAct, SIGNAL(triggered()), this, SLOT(getImageText()));

    setImageTextAct = new QAction(tr("Set image &text"), this);
    connect(setImageTextAct, SIGNAL(triggered()), this, SLOT(setImageText()));

    convertCsvTpsAct = new QAction(tr("&Convert CSV to TPS"), this);
    connect(convertCsvTpsAct, SIGNAL(triggered()), this, SLOT(convertCsvTps()));

    reconfigAct = new QAction(tr("&Reconfiguration"), this);
    connect(reconfigAct, SIGNAL(triggered()), this, SLOT(reconfig()));

    renameAct = new QAction(tr("Rename"), this);
    connect(renameAct, SIGNAL(triggered()), this, SLOT(rename()));

    addDirPrefixAct = new QAction(tr("Add Directory Prefix"), this);
    connect(addDirPrefixAct, SIGNAL(triggered()), this, SLOT(addDirPrefix()));

    testAct = new QAction("Test", this);
    connect(testAct, SIGNAL(triggered()), this, SLOT(test()));

}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(openDirAct);
    fileMenu->addAction(openPrevAct);
    fileMenu->addAction(openNextAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(removeFileAct);

    fileMenu->addAction(importTpsAct);
    fileMenu->addAction(exportDataAct);
    fileMenu->addAction(exportDataDirAct);
    fileMenu->addAction(preferencesAct);

    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);

    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    modeMenu = new QMenu(tr("&Mode"), this);
    modeMenu->addAction(moveModeAct);
    modeMenu->addAction(dragModeAct);
    modeMenu->addAction(addModeAct);

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(flipHorAct);
    editMenu->addAction(flipVerAct);
    editMenu->addAction(rotateAct);
    editMenu->addAction(grayscaleAct);
    editMenu->addAction(cropAct);
    editMenu->addSeparator();
    editMenu->addAction(grayscaleDirAct);
    editMenu->addAction(cropDirAct);
    editMenu->addAction(scaleDirAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(zoomFitAct);
    viewMenu->addAction(zoomActualAct);
    viewMenu->addAction(zoomLandmarksAct);
    viewMenu->addAction(zoom3Act);
    viewMenu->addAction(zoom4Act);
    // viewMenu->addAction(edgeAct);

    dataMenu = new QMenu(tr("&Data"), this);
    dataMenu->addAction(dataLmakAct);
    dataMenu->addAction(dataCsAct);
    dataMenu->addAction(dataDistAct);

    lmarkMenu = new QMenu(tr("&Landmarks"), this);
    lmarkMenu->addAction(lmarkDelAct);
    lmarkMenu->addAction(lmarkResetAct);
    lmarkMenu->addAction(lmarkFitAct);
    lmarkMenu->addAction(lmarkOutAct);
    lmarkMenu->addAction(lmarkOneOutAct);
    lmarkMenu->addAction(lmarkPairOutAct);
    // //    lmarkMenu->addAction(lmarkAdjustAct);
    lmarkMenu->addSeparator();
    lmarkMenu->addAction(lmarkResetDirAct);
    lmarkMenu->addAction(lmarkFitDirAct);

    lmarkMenu->addSeparator();
    lmarkMenu->addAction(protoSetAct);
    lmarkMenu->addAction(protoViewAct);

    semiMenu = new QMenu(tr("Sem&ilandmarks"), this);
    semiMenu->addAction(semiOutlineAct);
    semiMenu->addAction(semiLandAct);
    semiMenu->addAction(semiLand2Act);
    semiMenu->addAction(semiAlignAct);
    semiMenu->addSeparator();
    semiMenu->addAction(semiOutlineDirAct);
    semiMenu->addAction(semiLand2DirAct);

    scaleMenu = new QMenu(tr("&Scale"), this);
    scaleMenu->addAction(setScaleBarAct);
    scaleMenu->addAction(clearScaleBarAct);
    scaleMenu->addAction(setResolutionAct);
    scaleMenu->addAction(setResolutionDirAct);
    scaleMenu->addAction(scaleInfoAct);

    idMenu = new QMenu(tr("&Classification"), this);
    idMenu->addAction(idFileAct);
    idMenu->addAction(idAct);
    idMenu->addAction(idDirAct);
    // //    idMenu->addAction(getIdInfoAct);
    idMenu->addAction(idDirAllAct);
    idMenu->addAction(idDirDirAct);
    idMenu->addAction(idDataAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutDwAct);

    testMenu = new QMenu(tr("&Test"), this);
    testMenu->addAction(getImageTextAct);
    testMenu->addAction(setImageTextAct);
    testMenu->addAction(convertCsvTpsAct);
    testMenu->addAction(reconfigAct);
    testMenu->addAction(renameAct);
    testMenu->addAction(addDirPrefixAct);
    testMenu->addAction(testAct);
    // testMenu->addAction(setImageTextRawAct);
    // // testMenu->addAction(alignDataAct);
    // testMenu->addAction(getIdInfoAct);
    // // testMenu->addAction(getIdDwXmlAct);
    // // testMenu->addAction(imageAverageAct);
    // testMenu->addAction(removeBgdDirAct);
    // testMenu->addAction(removeBorderDirAct);
    // testMenu->addAction(scaleDirAct);
    // testMenu->addAction(clearTxtDirAct);
    // testMenu->addAction(idDirAllAct);
    // testMenu->addAction(flipHorDirAct);
    // testMenu->addAction(flipVerDirAct);
    // testMenu->addAction(rotateDirAct);
    // testMenu->addAction(grayscaleDirAct);
    // testMenu->addAction(lmarkAdjOneAct);
    // testMenu->addAction(lmarkPredAct);
    // testMenu->addAction(lmarkAddOneAct);
    // testMenu->addAction(reconfigAct);
    // testMenu->addAction(openDirRefAct);
    // testMenu->addAction(exportDirRefCsvAct);
    // testMenu->addAction(exportUsingXmlAct);
    // testMenu->addAction(removeFileListAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(modeMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(dataMenu);
    menuBar()->addMenu(lmarkMenu);
    menuBar()->addMenu(semiMenu);
    menuBar()->addMenu(scaleMenu);
    menuBar()->addMenu(idMenu);
    menuBar()->addMenu(helpMenu);

    #ifdef QT_DEBUG
    menuBar()->addMenu(testMenu);
    #endif
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));

    protoLbl = new QLabel;
    protoLbl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    protoUpdate();
    // QFileInfo protoFileInfo(protoFilePath);
    // QString protoFileName = protoFileInfo.fileName();
    // protoLbl->setText(protoFileName);
    protoLbl->setToolTip(tr("Prototype file"));
    statusBar()->addPermanentWidget(protoLbl);

    classLbl = new QLabel;
    classLbl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    QFileInfo idFileInfo(idFilePath);
    QString idFileName = idFileInfo.fileName();
    classLbl->setText(idFileName);
    classLbl->setToolTip(tr("Classification file"));
    statusBar()->addPermanentWidget(classLbl);

    coordLbl = new QLabel;
    coordLbl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    coordLbl->setText("(    ,    )(    ,    )    :    ,   ,   ,   ");
    coordLbl->setToolTip(tr("(bitmap-x,y)(Cartesian-x,y) transparancy: red, green, blue"));
    statusBar()->addPermanentWidget(coordLbl);
    connect(view, SIGNAL( mouseMoved(QPointF) ),this, SLOT( updateStatus(QPointF) ) );

}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(openPrevAct);
    fileToolBar->addAction(openNextAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(removeFileAct);

    moveModeBottom = new QToolButton;
    moveModeBottom->setDefaultAction(moveModeAct);
    fileToolBar->addWidget(moveModeBottom);

    dragModeButton = new QToolButton;
    dragModeButton->setDefaultAction(dragModeAct);
    fileToolBar->addWidget(dragModeButton);

    addModeBottom = new QToolButton; //qaction does not allow checkable
    addModeBottom->setDefaultAction(addModeAct);
    fileToolBar->addWidget(addModeBottom);

    viewToolBar = addToolBar(tr("&View"));

    QToolButton *zoomInIcon = new QToolButton(viewToolBar); // QAction does not allow autorepeat
    zoomInIcon->setAutoRepeat(true);
    zoomInIcon->setAutoRepeatInterval(40);
    zoomInIcon->setAutoRepeatDelay(0);
    zoomInIcon->setDefaultAction(zoomInAct);
    QToolButton *zoomOutIcon = new QToolButton(viewToolBar);
    zoomOutIcon->setAutoRepeat(true);
    zoomOutIcon->setAutoRepeatInterval(40);
    zoomOutIcon->setAutoRepeatDelay(0);
    zoomOutIcon->setDefaultAction(zoomOutAct);
    zoomSlider = new QSlider(viewToolBar->orientation(), viewToolBar);
    zoomSlider->setMinimum(20);
    zoomSlider->setMaximum(500);
    zoomSlider->setValue(100);
    zoomSlider->setTickPosition(QSlider::TicksRight);
    connect(viewToolBar, SIGNAL( orientationChanged(Qt::Orientation) ),
            zoomSlider, SLOT( setOrientation(Qt::Orientation) ) );
    connect(zoomSlider, SIGNAL(valueChanged(int)), view, SLOT(setupMatrix(int)));

    viewToolBar->addWidget(zoomOutIcon);
    viewToolBar->addWidget(zoomSlider);
    viewToolBar->addWidget(zoomInIcon);
}

bool MainWindow::isNoImage(){
    if(screenImg.isNull())
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("No image"));
        return true;
    }
    else
        return false;
}

void MainWindow::updateStatus(QPointF pos)
{
    int x = pos.x();
    int y = pos.y();
    if ( screenImg.valid(x,y) ){
        QRgb px = screenImg.pixel(x,y);
        QString pixelinfo = QString("%1: %2,%3,%4").arg(qAlpha(px)).arg(qRed(px)).arg(qGreen(px)).arg(qBlue(px));
        QString label = QString("(%1, %2)(%3, %4) ").arg(x).arg(y).arg(x).arg(screenImg.height()-y);

        //coordinates in Cartesian coordinates, color
        label += pixelinfo;
        coordLbl->setText(label);
        // coordLbl->show();
    }else{
        coordLbl->setText("(    ,    )(    ,    )    :    ,   ,   ,   ");
    }
}

void MainWindow::openFile()
{
    if (!okToContinue())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    filePath, tr("Images (*.png *.bmp *.tif *.jpg)"));
    if( fileName.isEmpty() )
        return;

    loadFile(fileName);
}

void MainWindow::openDir()
{
    if (!okToContinue()) return;

    QString inDirName = QFileDialog::getExistingDirectory(
        this, tr("Choose a directory"),
        filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    clear();

    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    fileInfoList = inDir.entryInfoList(QStringList("*dw.png"), QDir::Files, QDir::Name );

    if(fileInfoList.size() == 0)
        return;

    if(fileInfoList.size() > 1)
        openNextAct->setEnabled(true);
    fileInfoCount = 0;
    inFileInfo = fileInfoList.at(fileInfoCount);

    protoLoadDir(inDir);

    openDwPng(inFileInfo.absoluteFilePath());
}

// use only from openDir() and openDirAct
void MainWindow::openNext()
{
    if(fileInfoCount > fileInfoList.size()-2) return;
    if (!okToContinue()) return;

    ++fileInfoCount;
    inFileInfo = fileInfoList.at(fileInfoCount);
    openPrevAct->setEnabled(true);
    if(fileInfoCount == fileInfoList.size()-1)
        openNextAct->setEnabled(false);
    openDwPng(inFileInfo.absoluteFilePath());
}

// use only from openDir() and openDirAct
void MainWindow::openPrev()
{
    if(fileInfoCount < 1) return;
    if (!okToContinue()) return;

    --fileInfoCount;
    inFileInfo = fileInfoList.at(fileInfoCount);
    openNextAct->setEnabled(true);
    if(fileInfoCount == 0)
        openPrevAct->setEnabled(false);
    openDwPng(inFileInfo.absoluteFilePath());
}

// used by openFile and open recent files
bool MainWindow::loadFile(const QString & fileName)
{
    clear();

    inFileInfo.setFile(fileName);
    filePath = inFileInfo.absolutePath();

    if( !openDwPng(inFileInfo.absoluteFilePath()) )
        return false;
    fileInfoList.push_back(inFileInfo);
    fileInfoCount = 0;

    //proces recent files
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    updateRecentFileActions();

    openNextAct->setEnabled(false);
    openPrevAct->setEnabled(false);

    protoLoadDir(inFileInfo.dir());
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event) //unreferenced event needst to be here
{
    if (okToContinue()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::okToContinue()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("Warning"),
                                     tr("The image data have been modified.\n""Do you want to save your changes?"),
                                     QMessageBox::Yes | QMessageBox::Default,
                                     QMessageBox::No,
                                     QMessageBox::Cancel | QMessageBox::Escape);
        if (r == QMessageBox::Yes) {
            QString fn = inFileInfo.absoluteFilePath();
            return saveDwPng(fn);
        } else if (r == QMessageBox::No) {
            setWindowModified(false);
            return true;
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::openDwPng(const QString & fileName)
{
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    statusBar()->showMessage(tr("Loading %1").arg(fileName));

    inImg.load(fileName);
    if (inImg.isNull()) {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot open file %1")
                                 .arg(fileName));
        return false;
    }

    if(inImg.depth() == 32 && inImg.isGrayscale())
        inImg = inImg.convertToFormat(QImage::Format_Grayscale8);

    view->clear();
    overImg = QImage();
    openImgActions();
    setWindowTitle(fileName+"[*]");
    QString fn = inFileInfo.fileName();
    info.fromImg(inImg);
    info.setFileName(fn);
    view->setMarkers(info.getLandmarks());
    std::vector< Coord > scaleBar = info.getScaleBar();
    view->setScaleBar(scaleBar);

    QString outlineString = info.getOutline();
    if(!outlineString.isEmpty())
    {
        pxlList outline;
        outline.fromTxt(outlineString);
        overImg = QImage(inImg.width(), inImg.height(), QImage::Format_Indexed8 );
        overImg.fill(255);
        overImg.markList(&outline, 0);

        QRgb semiGreen = 0x8000FF00;
        QRgb semiRed = 0x80FF0000;
        QRgb transparent = 0x00000000;
        int thd = 255;
        overImg.setColor ( 0, semiRed );
        for(int i = 1; i < thd; i++)
            overImg.setColor ( i, semiGreen );
        for(int j = thd ; j < 256; j++)
            overImg.setColor ( j, transparent );

        view->setOverlay(overImg);
    }

    view->setImg(inImg);
    screenImg = inImg;

    if(loadZoomOpt == 0)
    {
        zoomFit();
    }
    else if(loadZoomOpt == 1)
    {
        zoomSlider->setValue(100); // set actual size
    }
    else if(loadZoomOpt == 2)
    {
        zoomLandmarks();
    }
    else if(loadZoomOpt == 3)
    {
        zoomSlider->setValue(zoom3Opt);
    }
    else if(loadZoomOpt == 4)
    {
        zoomSlider->setValue(zoom4Opt);
    }

    // show differences between left and right
    if(differencesLR){
        QFileInfo refFileInfo;
        if(fileInfoCount % 2 == 0)
        { // even
            refFileInfo = fileInfoList.at(fileInfoCount+1); // use next as reference
        }
        else
        { // odd
            refFileInfo = fileInfoList.at(fileInfoCount-1); // use previous as reference
        }
        dwImage refImg;
        refImg.load(refFileInfo.absoluteFilePath());
        WingInfo refInfo;
        refInfo.fromImg(refImg);
        reference = refInfo.toCoordList();
        reference.preshape();
        view->setDifferences(reference);
    }
    else if(reference.size())
    { // show differences from reference as lines
        view->setDifferences(reference);
    }

    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}

void MainWindow::openImgActions()
{
    // saveAct->setEnabled(false);
    // saveAsAct->setEnabled(true);
    // removeFileAct->setEnabled(true);

    moveModeAct->setEnabled(true);
    dragModeAct->setEnabled(true);
    addModeAct->setEnabled(true);

    // dataLmakAct->setEnabled(true);
    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(true);
    zoomSlider->setEnabled(true);

    // lmarkDelAct->setEnabled(true);
    // lmarkResetAct->setEnabled(true);
    // lmarkFitAct->setEnabled(true);

    // setScaleBarAct->setEnabled(true);
    // clearScaleBarAct->setEnabled(true);
    // setResolutionAct->setEnabled(true);
    // scaleInfoAct->setEnabled(true);

    // idAct->setEnabled(true);
}

bool MainWindow::saveDwPng(QString & fileName)
{
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    std::vector < Coord > junc = view->landmarks();
    info.setLandmarks(junc);

    // remove text in old format
    bool ok;
    QStringList txtList = screenImg.textKeys();
    if(txtList.size() > 1)
    {
        dwImage outImg = screenImg.dwCopy();
        info.toImg(outImg); //write wing info to image
        ok = outImg.save( fileName, "PNG");
    }
    else
    {
        info.toImg(screenImg); //write wing info to image
        ok = screenImg.save( fileName, "PNG");
    }
    QApplication::restoreOverrideCursor();
    if(ok){
        setWindowModified(false);
        saveAct->setEnabled(false);
        statusBar()->showMessage(tr("File saved"),2000);
    }else{
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot save file %1")
                                 .arg(fileName));
        return false;
    }
    return true;
}

void MainWindow::removeFile()
{
    if(isNoImage())
        return;
    if (!okToContinue())
        return;

    QString trashPath = inFileInfo.absolutePath() + "/trash";
    QDir trashDir(trashPath);
    if(!trashDir.exists())
    {
        QDir theDir(inFileInfo.absolutePath());
        theDir.mkdir("trash");
    }

    QString newName = inFileInfo.absolutePath() + "/trash/" + inFileInfo.fileName();

    bool ok = QFile::rename(fileInfoList.at(fileInfoCount).absoluteFilePath(), newName);
    if(!ok) return;

    if(fileInfoList.size() == 1)
    {
        clear();
        return;
    }

    int toRemove = fileInfoCount; // fileInfoCount will change!!!
    if(inFileInfo == fileInfoList.back()){
        openPrev();
    }else{
        openNext();
        fileInfoCount--;
    }

    fileInfoList.removeAt(toRemove);
    if(inFileInfo == fileInfoList.back())
        openNextAct->setEnabled(false);
}

bool MainWindow::save()
{
    if (inFileInfo.completeSuffix()=="dw.png") {
        QString fn = inFileInfo.absoluteFilePath();
        return saveDwPng(fn);
    } else {
        return saveAs();
    }
}

bool MainWindow::saveAs(void)
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    //    dialog.setConfirmOverwrite(true);
    //    dialog.setOption(QFileDialog::DontConfirmOverwrite, false); // by default
    dialog.setDefaultSuffix("dw.png"); // this is not possible with getSaveFileName
    dialog.setNameFilter( tr("Image & data (*.dw.png)") );

    //There is a problem when the selected file exists therefore files are numbered until they do not exist.
    QString newName = inFileInfo.absolutePath() + "/" + inFileInfo.baseName() + ".dw.png"; //This is required for proper handling of ".dw.png"
    QFileInfo newInfo(newName);
    int count = 0;
    while(newInfo.exists()){
        ++count;
        QString countStr;
        countStr.setNum(count);
        newName = inFileInfo.absolutePath() + "/" + inFileInfo.baseName() + "_" + countStr + ".dw.png";
        newInfo.setFile(newName);
    }
    dialog.setDirectory( inFileInfo.dir() );
    dialog.selectFile(newInfo.baseName() );
    if ( !dialog.exec() )
        return false; //canceled
    QStringList fileNames = dialog.selectedFiles();
    QString fileName = fileNames.at(0);

    // force the format .dw.png
    QFileInfo localFileInfo(fileName);
    if(localFileInfo.completeSuffix() != "dw.png")
    {
        fileName = localFileInfo.filePath() + ".dw.png";
        localFileInfo.setFile(fileName);
        if(localFileInfo.exists())
        {
            QMessageBox::StandardButton reply =
                QMessageBox::warning ( this, tr("Warning"),
                                     tr("The file %1 exists.\n"
                                        "Do you want to replace it?").arg(fileName),
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
            if(reply == QMessageBox::No)
                return false;
        }
    }

    if( saveDwPng(fileName) ) {
        setWindowTitle(fileName+"[*]");
        QFileInfo newInfo(fileName);
        inFileInfo = newInfo; // set file
        return true;
    } else {
        return false;
    }
}

void MainWindow::saveOn()
{
    setWindowModified(true);
    saveAct->setEnabled(true);
}

// remame files or directories in directory accordint to txt files
void MainWindow::rename()
{
    QString inDirName = QFileDialog::getExistingDirectory(
        this, tr("Choose a directory"),
        filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );
    dwFileRename renamer;
    renamer.rename(inDir);
}

// add prefix to file name reflecting direectories hierarchy
void MainWindow::addDirPrefix()
{
    QString inDirName = QFileDialog::getExistingDirectory(
        this, tr("Choose a directory"),
        filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );
    dwFileRename renamer;
    //    renamer.addSufix(inDir);
    renamer.addDirPrefix(inDir);
}

void MainWindow::updateRecentFileActions()
{
    QMutableStringListIterator i(recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }
    for (int j = 0; j < MaxRecentFiles; ++j) {
        if (j < recentFiles.count()) {
            QString text = QString("&%1 %2")
                               .arg(j + 1)
                               .arg(QFileInfo(recentFiles[j]).fileName()); //strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        } else {
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentFiles.isEmpty());
}

void MainWindow::openRecentFile()
{
    if (okToContinue()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
            loadFile(action->data().toString());
    }
}

void MainWindow::resetActions()
{
    saveAct->setEnabled(false);

    // moveModeAct->setEnabled(false);
    // dragModeAct->setEnabled(false);
    // addModeAct->setEnabled(false);

    // zoomInAct->setEnabled(false);
    // zoomOutAct->setEnabled(false);
    // zoomSlider->setEnabled(false);

    //////////////////
    // saveAsAct->setEnabled(false);
    // removeFileAct->setEnabled(false);
    // dataLmakAct->setEnabled(false);
    // lmarkDelAct->setEnabled(false);
    // lmarkResetAct->setEnabled(false);
    // lmarkFitAct->setEnabled(false);

    // setScaleBarAct->setEnabled(false);
    // clearScaleBarAct->setEnabled(false);
    // setResolutionAct->setEnabled(false);
    // scaleInfoAct->setEnabled(false);

    // idAct->setEnabled(false);
}

void MainWindow::readSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString appName = QGuiApplication::applicationDisplayName();
    QSettings settings(companyName, appName);
    loadZoomOpt = settings.value("loadZoom", 0).toInt();
    zoom3Opt = settings.value("zoom3", 150).toInt();
    zoom4Opt = settings.value("zoom4", 200).toInt();

    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();
    filePath = settings.value("filePath").toString();
    QString idDefault = QCoreApplication::applicationDirPath()+"/dwxml/apis-mellifera-lineages-classification.dw.xml";
    idFilePath = settings.value("idFilePath", idDefault).toString();

    QString prDefault = QCoreApplication::applicationDirPath()+"/dwxml/apis-worker-prototype.dw.png";
    protoFilePath = settings.value("protoFilePath", prDefault).toString();

    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString appName = QGuiApplication::applicationDisplayName();
    QSettings settings(companyName, appName);
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("filePath", filePath);
    settings.setValue("idFilePath", idFilePath);
    settings.setValue("protoFilePath", protoFilePath);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::updateOptions()
{
    QString companyName = QGuiApplication::organizationName();
    QString appName = QGuiApplication::applicationDisplayName();
    QSettings settings(companyName, appName);
    loadZoomOpt = settings.value("loadZoom", 0).toInt();
    zoom3Opt = settings.value("zoom3", 150).toInt();
    zoom4Opt = settings.value("zoom4", 200).toInt();
}

void MainWindow::addMode()
{
    view->setMode(View::AddMode);
    addModeAct->setChecked(true);
    dragModeAct->setChecked(false);
    moveModeAct->setChecked(false);
}

void MainWindow::dragMode()
{
    view->setMode(View::DragMode);
    addModeAct->setChecked(false);
    dragModeAct->setChecked(true);
    moveModeAct->setChecked(false);
}

void MainWindow::moveMode()
{
    view->setMode(View::MoveMode);
    addModeAct->setChecked(false);
    dragModeAct->setChecked(false);
    moveModeAct->setChecked(true);
}

//use only local variables
void MainWindow::importTps()
{
        QString fileName = QFileDialog::getOpenFileName(this,
                                  tr("Open File"), filePath, tr("TPS files (*.tps)"));
        if (fileName.isEmpty())
            return;

    //
        QFile inFile(fileName);
        if (!inFile.open(QIODevice::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot open file %1:\n%2.")
                                     .arg(fileName, inFile.errorString()));
            return;
        }

        QTextStream inStream(&inFile);
        inStream.setCodec("UTF-8");
        // qt6    inStream.setEncoding(QStringConverter::Utf8);
        QStringList tpsTxtList;
        while (!inStream.atEnd())
        {
            QString line = inStream.readLine();
            tpsTxtList.push_back(line);
        }
        inFile.close();

        if(tpsTxtList.size() == 0)
            return;

        TPS tps(tpsTxtList);
        std::vector< Coord > landmarks = tps.getLandmarks();
        view->setMarkers(landmarks);
    //

        QApplication::setOverrideCursor(Qt::WaitCursor);
        dwRclList tpsList;
        QString error = tpsList.readTps(fileName);
        if(!error.isEmpty())
            QMessageBox::warning(this, tr("Warning"), error);

        QFileInfo inInfo(fileName);
        std::vector< dwRCoordList > tpsVec = tpsList.list();
        for(unsigned i = 0; i < tpsVec.size(); ++i){
            dwRCoordList rcl = tpsVec[i];
            QString tpsImgName = inInfo.absolutePath() + "/" + rcl.getId();
            dwImage tpsImg(tpsImgName);

            if (tpsImg.isNull()) {
                statusBar()->showMessage(tr("Cannot open file %1").arg(tpsImgName), 2000);
                continue;
            }
            statusBar()->showMessage(tr("Importing %1").arg(tpsImgName));

            WingInfo pngInfo;
            rcl.flip(tpsImg.height()); //convert from cartesian to bitmap coordinates
            std::vector< realCoord > junc = rcl.list();
            pngInfo.setLandmarks(junc);
            pngInfo.toImg(tpsImg); //write wing info to image

            QFileInfo tpsInfo(tpsImgName);
            QString outFileName;
            if(tpsInfo.completeSuffix() != "dw.png")
            {
                outFileName = inInfo.absolutePath() + "/" + tpsInfo.baseName() + ".dw.png"; //new file name
                QFileInfo outInfo(outFileName);
                if(outInfo.exists()){
                    QApplication::restoreOverrideCursor();
                    QMessageBox::StandardButton reply =
                            QMessageBox::warning ( this, tr("Warning"),
                                                   tr("The file %1 exists.\n"
                                                      "Do you want to replace it?").arg(outFileName),
                                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
                    QApplication::setOverrideCursor(Qt::WaitCursor);
                    if(reply == QMessageBox::No)
                        continue;
                }
            }
            else
            {
                outFileName = inInfo.absolutePath() + "/" + tpsInfo.fileName(); // old file name with path
            }
            tpsImg.save( outFileName, "PNG");
        }
        QApplication::restoreOverrideCursor();

        statusBar()->showMessage(tr("Finished"), 2000);
}

bool MainWindow::saveAsTps(QString & fileName)
{
    QFileInfo tpsFileInfo(fileName);
    if(tpsFileInfo.suffix().toUpper() != "TPS"){
        fileName = tpsFileInfo.absolutePath()+"/"+tpsFileInfo.baseName() + ".tps";
    }
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot write file %1:\n%2")
                                 .arg(fileName, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    std::vector< Coord > junc = view->landmarks();
    info.setLandmarks(junc);
    QString data = info.toTps();
    out << data;
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::exportData()
{
    if(isNoImage())
        return;
    std::vector< Coord > junc = view->landmarks();
    if(junc.size() == 0)
        return;

    QString outName = inFileInfo.path()+"/"+inFileInfo.baseName()+".csv";
    QString selectedFilter = tr("CSV files (*.csv)");
    QString fileName = QFileDialog::getSaveFileName
        (this, tr("Save as"), outName,
         tr("CSV files (*.csv);;TPS files (*.tps);;Tab delimited text (*.txt)"),
         &selectedFilter
         );
    if (fileName.isEmpty())
        return;

    QFile outFile(fileName);
    if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning
            (this, tr("Warning"),
             tr("Cannot write file %1:\n%2")
                 .arg(fileName, outFile.errorString())
             );
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    info.setLandmarks(junc);
    QTextStream outStream(&outFile);

    QString data;
    if(selectedFilter == tr("CSV files (*.csv)"))
        data = info.toCsv();
    else if(selectedFilter == tr("TPS files (*.tps)"))
        data = info.toTps();
    else if(selectedFilter == tr("Tab delimited text (*.txt)"))
    {
        data = info.toTxtHead();
        data += info.toTxtRow();
    }

    outStream << data;
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::exportDataDir()
{
    QString inDirName = QFileDialog::getExistingDirectory
        (this, tr("Choose a directory"),
         filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );

    QString outName = inDir.path()+"/"+ inDir.dirName()+".csv";
    QString selectedFilter = tr("CSV files (*.csv)");
    QString fileName = QFileDialog::getSaveFileName
        (this, tr("Export data as"), outName,
         tr("CSV files (*.csv);;TPS files (*.tps);;Tab delimited text (*.txt)"),
         &selectedFilter
         );
    if (fileName.isEmpty())
        return;

    QFile outFile(fileName);
    if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning
            (this, tr("Warning"),
             tr("Cannot write file %1:\n%2")
                 .arg(fileName, outFile.errorString())
             );
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.dw.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;

    QTextStream outStream(&outFile);
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        dwImage img(fileName);
        WingInfo localInfo;
        QString fn = localFileInfo.fileName();
        localInfo.fromImg(img);
        localInfo.setFileName(fn);

        QString data;
        if(selectedFilter == tr("CSV files (*.csv)"))
        {
            if(i==0)
                data = localInfo.toCsvHead();
            data += localInfo.toCsvRow();
        }else if(selectedFilter == tr("TPS files (*.tps)"))
            data = localInfo.toTps();
        else if(selectedFilter == tr("Tab delimited text (*.txt)"))
        {
            if(i==0)
                data = localInfo.toTxtHead();
            data += localInfo.toTxtRow();

            //            //export whole imgage text
            //            QString inTxt = img.text();
            //            inTxt.replace("\n", "#");
            //            inTxt.append("\n");
            //            data += inTxt;
        }

        outStream << data;
    }

    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::preferences()
{
    dwOptions * prefDialog = new dwOptions(this);
    prefDialog->show();
}

int MainWindow::getZoom()
{
    return zoomSlider->value();
}

void MainWindow::flipHor(){
    if(isNoImage())
        return;

    screenImg = screenImg.mirrored(true, false); //horizontaly
    dwRCoordList points = view->landmarks();
    points.flipHor(screenImg.width());
    view->setMarkers(points.coordList());
    view->setImg(screenImg);
    saveOn();
}

void MainWindow::flipVer(){
    if(isNoImage())
        return;

    screenImg = screenImg.mirrored(); //vertically
    dwRCoordList points = view->landmarks();
    points.flip(screenImg.height());
    view->setMarkers(points.coordList());
    view->setImg(screenImg);
    saveOn();
}

void MainWindow::rotate(){
    if(isNoImage())
        return;

    dwRCoordList points = view->landmarks();
    points.rotate90(screenImg.height()); //points moved before image because hieght changes
    view->setMarkers(points.coordList());

    screenImg = screenImg.getRotated();  // 90 degree clockwise
    view->setImg(screenImg);
    saveOn();
}

void MainWindow::grayscale(){
    if(isNoImage())
        return;

    screenImg = screenImg.convertToGray8();
    view->setImg(screenImg);
    saveOn();
}

void MainWindow::crop()
{
    if(isNoImage())
        return;

    if(!screenImg.isGrayTable())
        screenImg = screenImg.convertToGray8();
    unsigned otlThd = info.getOutlineThd();
    if(otlThd == 0)
        otlThd = screenImg.outlineThd(); //230 if not automatic

    screenImg.thresholdFast(otlThd);
    view->setImg(screenImg);
    thresholdDialog * dialog = new thresholdDialog(otlThd, this);
    dialog->exec();
    if(dialog->wasCanceled())
    {
        screenImg = inImg;
        view->setImg(screenImg);
        return;
    }

    Outline maybeWingsOutlines;
    maybeWingsOutlines.fromImg(&screenImg);

    // save only the largest outline
    pxlList * otl = maybeWingsOutlines.maxOutline();
    dwImage wingImg = inImg.simpleCrop(otl);
    if(wingImg.isNull())
        return;

    screenImg = wingImg;
    view->setImg(screenImg);
    saveOn();
}

void MainWindow::grayscaleDir(){
    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                          filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        QApplication::processEvents(); // for statusBar message refresh

        dwImage img(fileName);
        if(img.isNull()) continue;
        if(!img.isGrayTable())
        {
            img = img.convertToGray8();
            img.save( fileName, "PNG");
        }
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Finished"), 2000);
}

void MainWindow::cropDir()
{
    bool ok;
    unsigned char colorThd = QInputDialog::getInt(this, "", tr("Color threshold for cropping the image (0 - automatic detection)"),
                                                  0, 0, 255, 1, &ok);
    if(!ok)
        return;

    unsigned sizeThd;
    unsigned sizeMax;
    if(!screenImg.isNull())
    {
        sizeThd = screenImg.width()/4.0;
        sizeMax = 2*screenImg.width()+2*screenImg.height();
    }
    else
    {
        sizeThd = 500;
        sizeMax = 10000;
    }

    sizeThd = QInputDialog::getInt(this, "", tr("Size threshold for cropping the image (0 - automatic detection)"),
                                   sizeThd, 8, sizeMax, 1, &ok);
    if(!ok)
        return;

    int margin = QInputDialog::getInt(this, "", tr("Margin"),
                                      10, 0, 100, 1, &ok);
    if(!ok)
        return;

    QString inDirName = QFileDialog
        ::getExistingDirectory(this, tr("Choose a directory"),
                               filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    clear();

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;

    for (int i = 0; i < localFileInfoList.size(); ++i) {
        inFileInfo = localFileInfoList.at(i);

        if(inFileInfo.completeSuffix() == "dw.png")
            continue;
        if( !openDwPng(inFileInfo.absoluteFilePath()) )
            continue ;
        WingInfo inInfo;
        inInfo.fromImg(screenImg);
        if(colorThd == 0)
            colorThd = inInfo.getOutlineThd();
        if(!screenImg.isGrayTable())
            screenImg = screenImg.convertToGray8();
        screenImg.threshold(colorThd);
        Outline maybeWingsOutlines;
        maybeWingsOutlines.fromImg(&screenImg);
        std::list< pxlList > * maybeWings = maybeWingsOutlines.pixels();

        screenImg.setGrayTable();

        int wingCount = 0;
        std::list< pxlList >::iterator otl;
        for( otl = maybeWings->begin(); otl != maybeWings->end(); otl++ ){
            if( otl->clockwise || (otl->xSize() < (int)sizeThd) )  // clockwise otl is white spot on black background
                continue;

            dwImage wingImg = inImg.simpleCrop( &(*otl), margin);

            QString cntStr;
            cntStr.setNum(wingCount);
            wingCount++;
            QString dwpngFileName;
            dwpngFileName = inFileInfo.path()+"/"+inFileInfo.baseName()+"_"+cntStr+".dw.png";
            wingImg.save( dwpngFileName, "PNG");
        }
    }
    QApplication::restoreOverrideCursor();
    //statusBar()->showMessage(tr("File saved"), 2000);
}

// scale all images from a directory to fixed dpi
void MainWindow::scaleDir()
{
    bool ok;
    double inNumber = QInputDialog::getDouble(this, "", tr("Scaling factor"),
                                              0, 0, 1, 8, &ok);
    if(!ok) return;

    if(inNumber == 0){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Input value has to be greater than 0"));
        return;
    }

    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), filePath, QFileDialog::ShowDirsOnly);
    if(inDirName.isEmpty())
        return;

    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        QApplication::processEvents(); // for statusBar message refresh

        dwImage img(fileName);

        const double scale = inNumber;

        img = img.scale(scale); // landmarks are scaled inside
        img.save( fileName, "PNG");
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Finished"), 2000);
}

// used by thresholdDialog
void MainWindow::threshold( int value )
{
    // otlThd = value; //update the value
    screenImg.thresholdFast(value);
    view->setImg(screenImg);
}

void MainWindow::zoomIn()
{
    zoomSlider->setValue(zoomSlider->value() + 10);
}

void MainWindow::zoomOut()
{
    zoomSlider->setValue(zoomSlider->value() - 10);
}

// fit image to window
void MainWindow::zoomFit()
{
    if(isNoImage())
        return;

    QRect imgRect = inImg.rect();
    view->fitInView(imgRect, Qt::KeepAspectRatio);

    QTransform transform = view->transform();
    double newScale = transform.m11();
    zoomSlider->setValue(newScale*100);
}

// set magnification 1:1
void MainWindow::zoomActual()
{
    zoomSlider->setValue(100);
}

void MainWindow::zoomLandmarks()
{
    if(isNoImage())
        return;

    dwRCoordList points = view->landmarks();
    if( points.size() < 2)
        return;

    double xMin = points.xMin();
    double xMax = points.xMax();
    double xSize = xMax - xMin;
    double yMin = points.yMin();
    double yMax = points.yMax();
    double ySize = yMax - yMin;
    view->fitInView(xMin, yMin, xSize, ySize, Qt::KeepAspectRatio);

    QTransform transform = view->transform();
    double newScale = transform.m11();
    zoomSlider->setValue(newScale*100);
    zoomOut();
}

// set magnification defined by user (150% by default)
void MainWindow::zoom3()
{
    zoomSlider->setValue(zoom3Opt);
}

// set magnification defined by user (200% by default)
void MainWindow::zoom4()
{
    zoomSlider->setValue(zoom4Opt);
}

// show landmarks coordinates in text window
void MainWindow::dataLmak()
{
    std::vector< Coord > junc = view->landmarks();
    info.setLandmarks(junc);
    QString data = info.toTxt();
    TxtWindow * dataWindow = new TxtWindow;
    dataWindow->setPlainTxt(data);
    dataWindow->show();
}

// calculate centroid size of current landmarks
void MainWindow::dataCs()
{
    if(isNoImage())
        return;

    std::vector< Coord > landmarks = view->landmarks();
    if( landmarks.size() == 0 ){
        QMessageBox::warning(this, tr("Warning"), tr("No data available"));
        return;
    }

    dwRCoordList realLandmarks(landmarks);
    double CS = realLandmarks.centroidSize();

    QString data;
    data += "file:\t";
    data += inFileInfo.canonicalFilePath() + "\n";

    double dpm = info.getResolution();
    if(dpm > 0.0){
        QString s = tr("resolution:\t%1\tdots per meter\n").arg(dpm);
        data += s;
        data += tr("units:\tmeters \n");
    }else{
        dpm = 1.0;
        data += tr("units:\tpixels \n");
    }

    data += tr("centroid size:\t");
    data += QString::number(CS/dpm, 'g', 10);
    data += "\n";

    TxtWindow * dataWindow = new TxtWindow;
    dataWindow->setPlainTxt(data);
    dataWindow->show();
}

void MainWindow::dataDist()
{
    std::vector< Coord > points = view->landmarks();
    if(points.size() < 2){
        return;
    }

    QString data;
    data += "file:\t";
    data += inFileInfo.canonicalFilePath() + "\n";

    double dpm = info.getResolution();
    if(dpm > 0.0){
        QString s = tr("resolution:\t%1\tdots per meter\n").arg(dpm);
        data += s;
        data += tr("units:\tmeters \n");
    }else{
        data += tr("units:\tpixels \n");
    }
    data += tr("\n");

    data += tr("landmarks\tdistance\n");
    QString cData = tr("landmarks\tcumulative distance\n");
    double sum = 0.0;
    for(unsigned i = 1; i < points.size(); ++i)
    {
        Coord prevPnt = points[i-1];
        Coord currPnt = points[i];
        double dist = coordDistance(prevPnt, currPnt);
        if(dpm > 0.0)
            dist/=dpm;
        QString s = tr("%1-%2\t%3\n").arg(i).arg(i+1).arg(dist);
        data += s;
        sum += dist;
        cData += tr("1-%1\t%2\n").arg(i+1).arg(sum);
    }
    if(points.size() > 2)
    {
        data += tr("\n");
        data += cData;
    }

    TxtWindow * dataWindow = new TxtWindow;
    dataWindow->setPlainTxt(data);
    dataWindow->show();
}

void MainWindow::lmarkDel()
{
    view->deleteSelected();
}

void MainWindow::lmarkReset()
{
    if(isNoImage())
        return;
    if(prototypeImg.isNull())
        protoLoad();
    if(prototypeImg.isNull())
        return;

    WingInfo protoInfo;
    protoInfo.fromImg(prototypeImg);
    std::vector< Coord > protoLmark = protoInfo.getLandmarks();
    info.setLandmarks(protoLmark);
    view->setMarkers(protoLmark);
    setWindowModified(true);
}

void MainWindow::lmarkResetDir()
{
    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                          filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );

    protoLoadDir(inDir);
    if(prototypeImg.isNull())
        return;
    WingInfo prototypeInfo;
    prototypeInfo.fromImg(prototypeImg);
    std::vector< Coord > prototypeLandmarks = prototypeInfo.getLandmarks();

    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        QApplication::processEvents(); // for statusBar message refresh

        dwImage img(fileName);
        if(img.isNull()) continue;
        WingInfo theInfo;
        theInfo.fromImg(img);
        theInfo.setLandmarks(prototypeLandmarks);
        theInfo.toImg(img);
        img.save( fileName, "PNG");
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Finished"), 2000);
}

void MainWindow::lmarkFit()
{
    if(isNoImage())
        return;

    std::vector< Coord > landmarks = view->landmarks();
    if( landmarks.size() == 0 )
    {
        QMessageBox::warning(this, tr("Warning"), tr("No data available"));
        return;
    }

    if(prototypeImg.isNull())
        protoLoad();
    if(prototypeImg.isNull())
        return;

    WingInfo masterInfo;
    masterInfo.fromImg(prototypeImg);
    std::vector< Coord > masterPoints = masterInfo.getLandmarks();

    dwRCoordList validList;
    dwRCoordList shortMasterList;
    std::vector< bool > isValid(landmarks.size(), false);
    unsigned fittedCount = 0;
    for(unsigned i = 0; i < landmarks.size(); ++i)
    {
        Coord point = landmarks[i];
        if( screenImg.valid(point.dx(), point.dy()))
        {
            validList.push_back(landmarks[i]);
            shortMasterList.push_back(masterPoints[i]);
            isValid[i] = true;
            ++fittedCount;
        }
    }

    if(fittedCount < 2)
    {
        QMessageBox::warning(this, tr("Warning"), tr("At least 2 landmarks have to be valid."));
        return;
    }
    //    else if(fittedCount == landmarks.size())
    //    {
    //        QMessageBox::warning(this, tr("Warning"), tr("At least 1 landmark has to be invalid."));
    //        return;
    //    }

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    realCoord validCentroid = validList.centroid();
    realCoord masterCentroid = shortMasterList.centroid();

    double validCentroidSize = validList.centroidSize();
    double masterCentroidSize = shortMasterList.centroidSize();
    double csRatio = validCentroidSize/masterCentroidSize;

    validList.center();
    validList.scale(1.0/validCentroidSize);
    shortMasterList.center();
    shortMasterList.scale(1.0/masterCentroidSize);
    double angle = shortMasterList.rotationAngle(validList.list());

    masterCentroid *= -1.0;
    dwRCoordList outList;
    outList = masterPoints;
    outList.add(masterCentroid); //new centroid in (0,0)
    outList.rotate(angle);
    outList.scale(csRatio);
    outList.add(validCentroid);

    // replace the original valid points
    std::vector< Coord > fittedLandmarks = outList.coordList();
    for(unsigned i = 0; i < landmarks.size(); ++i)
        if( isValid[i])
            fittedLandmarks[i] = landmarks[i];

    // add the fited landmarks to the screenImg be
    info.setLandmarks(fittedLandmarks);
    info.toImg(screenImg);


    dwAdjust adj(screenImg, prototypeImg);
    fittedLandmarks = adj.adjusted();
    // Adjust adj(screenImg, prototypeImg);
    // fittedLandmarks = adj.fitPoints();


    view->setMarkers(fittedLandmarks);

    saveOn();
    QApplication::restoreOverrideCursor();
}

void MainWindow::lmarkFitDir()
{
    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                          filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );
    filePath = inDir.absolutePath();

    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;

    protoLoad();
    if(prototypeImg.isNull())
        return;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        QApplication::processEvents(); // for statusBar message refresh

        dwImage img(fileName);
        if(img.isNull())
            continue;
        if(!img.isGrayTable())
            img = img.convertToGray8();

        WingInfo info;
        info.fromImg(img);
        std::vector< Coord > landmarks = info.getLandmarks();
        if(landmarks.size() == 0)
            continue;

        //        Adjust adj(img, prototypeImg);
        dwAdjust adj(img, prototypeImg);
        std::vector< Coord > fittedLandmarks = adj.adjusted();

        info.setLandmarks(fittedLandmarks);
        info.toImg(img);
        img.save( fileName, "PNG");
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Finished"), 2000);
}

// find outliers and open them starting from the worst
void MainWindow::lmarkOut()
{
    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                          filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    QDir inDir( inDirName );
    filePath = inDir.absolutePath();

    dwRclList rcl;
    rcl.setId(inDir.dirName());
    QStringList extList = (QStringList() << "*.dw.png");
    QFileInfoList fileList = inDir.entryInfoList(extList, QDir::Files, QDir::Name );
    for (int j = 0; j < fileList.size(); ++j) {
        QFileInfo localFileInfo = fileList.at(j);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        dwImage img(fileName);
        WingInfo localInfo;
        localInfo.fromImg(img);
        dwRCoordList coordinates(localInfo.getLandmarks());
        coordinates.setId(localFileInfo.absoluteFilePath());
        rcl.push_back(coordinates);
    }

    QString zeroCS = rcl.findZeroCS();
    if(!zeroCS.isEmpty()){
        QApplication::restoreOverrideCursor();
        zeroCS.prepend("Centroid size = 0\n");
        QMessageBox::warning(this, tr("Warning"), zeroCS);
        return;
    }

    if(!rcl.isSizeEqual()){
        QApplication::restoreOverrideCursor();
        QString sizeOutliers = rcl.sizeOutliers();
        QMessageBox::warning(this, tr("Warning"), sizeOutliers);
        return;
    }

    if(rcl.size() == 0){
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"), tr("No dw.png files found"));
        return;
    }

    reference = rcl.superimposeGPA();
    reference.center();
    reference.scale(1.0/reference.centroidSize());
    statusBar()->showMessage(tr("Finished"));

    ///
    dwDataSet theDataSet;
    theDataSet.fromRclList(rcl);
    std::vector< QString > outList = theDataSet.outliersMD();
    ///
    // std::vector< QString > outList = rcl.outliers();
    ///

    QList< QFileInfo > theFileList;
    for(unsigned i = 0; i < outList.size(); ++i)
    {
        QString theStr = outList[i];
        QFileInfo theInfo(theStr);
        theFileList.push_back(theInfo);
    }
    fileInfoList = theFileList;

    if(fileInfoList.size())
    {
        if(fileInfoList.size() > 1)
            openNextAct->setEnabled(true);
        fileInfoCount = 0;
        inFileInfo = fileInfoList.at(fileInfoCount);
        openDwPng(inFileInfo.absoluteFilePath());
    }

    QApplication::restoreOverrideCursor();
}

// find pairs outliers
void MainWindow::lmarkOneOut()
{
    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                          filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    QDir inDir( inDirName );
    filePath = inDir.absolutePath();

    dwRclList rcl;
    //rcl.fromImgDir(inDir);
    ///
    rcl.setId(inDir.dirName());
    QStringList extList = (QStringList() << "*.dw.png");
    QFileInfoList fileList = inDir.entryInfoList(extList, QDir::Files, QDir::Name );
    for (int j = 0; j < fileList.size(); ++j) {
        QFileInfo localFileInfo = fileList.at(j);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        dwImage img(fileName);
        WingInfo localInfo;
        localInfo.fromImg(img);
        dwRCoordList coordinates(localInfo.getLandmarks());
        coordinates.setId(localFileInfo.absoluteFilePath());
        rcl.push_back(coordinates);
    }

    QString zeroCS = rcl.findZeroCS();
    if(!zeroCS.isEmpty()){
        QApplication::restoreOverrideCursor();
        zeroCS.prepend("Centroid size = 0\n");
        QMessageBox::warning(this, tr("Warning"), zeroCS);
        return;
    }

    if(!rcl.isSizeEqual()){
        QApplication::restoreOverrideCursor();
        QString sizeOutliers = rcl.sizeOutliers();
        QMessageBox::warning(this, tr("Warning"), sizeOutliers);
        return;
    }

    if(rcl.size() == 0){
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"), tr("No dw.png files found"));
        return;
    }

    bool ok;
    int landmarkMax = rcl.getMinSize();
    QApplication::restoreOverrideCursor();
    int theOne = QInputDialog::getInt(this, "", tr("Landmark number"), 1, 1, landmarkMax, 1, &ok);
    if(!ok)
        return;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    reference = rcl.superimposeGPA();
    reference.center();
    reference.scale(1.0/reference.centroidSize());
    statusBar()->showMessage(tr("Finished"));

    std::vector< QString > outList = rcl.oneOutlier(theOne);
    QList< QFileInfo > theFileList;
    for(unsigned i = 0; i < outList.size(); ++i)
    {
        QString theStr = outList[i];
        QFileInfo theInfo(theStr);
        theFileList.push_back(theInfo);
    }
    fileInfoList = theFileList;

    if(fileInfoList.size())
    {
        if(fileInfoList.size() > 1)
            openNextAct->setEnabled(true);
        fileInfoCount = 0;
        inFileInfo = fileInfoList.at(fileInfoCount);
        openDwPng(inFileInfo.absoluteFilePath());
    }

    QApplication::restoreOverrideCursor();
}

// find pairs outliers
void MainWindow::lmarkPairOut()
{
    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                          filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    QDir inDir( inDirName );

    dwPairLR pairLR;
    std::vector< QString > sortStrList = pairLR.outliers(inDir);

    QList< QFileInfo > theFileList;
    for(unsigned i = 0; i < sortStrList.size(); ++i){
        QString theStr = inDir.absolutePath() + "/" + sortStrList[i];
        QFileInfo theInfo(theStr);
        theFileList.push_back(theInfo);
    }
    fileInfoList = theFileList;
    differencesLR = true;

    if(fileInfoList.size()){
        if(fileInfoList.size() > 1)
            openNextAct->setEnabled(true);
        fileInfoCount = 0;
        inFileInfo = fileInfoList.at(fileInfoCount);
        openDwPng(inFileInfo.absoluteFilePath());
    }

    QApplication::restoreOverrideCursor();
}

void MainWindow::protoSet()
{
    QString inFileName = QFileDialog
        ::getOpenFileName(this,
                          tr("Set prototype"), protoFilePath,
                          tr("Images with landmarks (*.dw.png)"));

    if (inFileName.isEmpty())
        return;

    protoFilePath = inFileName;
    protoUpdate();
    // QFileInfo protoFileInfo(protoFilePath);
    // QString protoFileName = protoFileInfo.fileName();
    // protoLbl->setText(protoFileName); // update status bar

    // prototypeImg.load(protoFilePath);
    // if(prototypeImg.isNull())
    //     QMessageBox::warning(this, tr("Warning"),
    //                          tr("Cannot open file %1")
    //                              .arg(protoFilePath));
}
void MainWindow::protoUpdate()
{
    prototypeImg.load(protoFilePath);
    if(!prototypeImg.isNull())
    {
        QFileInfo protoFileInfo(protoFilePath);
        QString protoFileName = protoFileInfo.fileName();
        protoLbl->setText(protoFileName); // update status bar
        return;
    }
    QMessageBox::warning(this, tr("Warning"),
                         tr("Cannot open file %1")
                             .arg(protoFilePath));

    // load from idFile
    QDomDocument XmlDoc = loadXmlDoc(idFilePath);
    if(XmlDoc.isNull())
    {
        protoDefault();
        return;
    }

    dwLdaGm lda(XmlDoc);
    QString ldaError = lda.getError();
    if(ldaError != "")
    {
        QMessageBox::warning(this, tr("Warning"), ldaError);
        protoDefault();
        return;
    }

    QString prototypeName = lda.getPrototype();

    QFileInfo idFileInfo(idFilePath);
    protoFilePath = idFileInfo.path() + "/" + prototypeName;

    prototypeImg.load(protoFilePath);
    if(!prototypeImg.isNull())
    {
        QFileInfo protoFileInfo(protoFilePath);
        QString protoFileName = protoFileInfo.fileName();
        protoLbl->setText(protoFileName); // update status bar
        return;
    }
    QMessageBox::warning(this, tr("Warning"),
                         tr("Cannot open file %1")
                             .arg(protoFilePath));
    protoDefault();

}

void MainWindow::protoDefault()
{
    protoFilePath = QCoreApplication::applicationDirPath()+"/dwxml/apis-worker-prototype.dw.png";
    prototypeImg.load(protoFilePath);
    if(!prototypeImg.isNull())
    {
        QFileInfo protoFileInfo(protoFilePath);
        QString protoFileName = protoFileInfo.fileName();
        protoLbl->setText(protoFileName); // update status bar
        return;
    }
    QMessageBox::critical(this,
                         "Reinstallation Required",
                         "A critical issue has been detected. Please reinstall the application to ensure proper functionality.");
}

// it is possible to open many viewers
// add setImage to dwImageViewer
void MainWindow::protoView()
{
    if(prototypeImg.isNull())
        protoLoad();
    if(prototypeImg.isNull())
        return;
    dwImageViewer * imageViewer = new dwImageViewer(prototypeImg, this);
    QFileInfo protoFileInfo(protoFilePath);
    QString protoFileName = protoFileInfo.fileName();
    imageViewer->setWindowTitle(protoFileName);
    imageViewer->show();
}

void MainWindow::protoLoad()
{
    // first try to load it from the prototype file inside the directory
    QString inDirName = inFileInfo.absolutePath();
    QDir inDir(inDirName);

    bool ok = protoLoadDir(inDir);
    if(ok) return; //prototype was fond in the directory

    // if no prototype in directory load from idFile
    QDomDocument XmlDoc = loadXmlDoc(idFilePath);
    if(XmlDoc.isNull())
        return;

    dwLdaGm lda(XmlDoc);
    QString ldaError = lda.getError();
    if(ldaError != "")
    {
        QMessageBox::warning(this, tr("Warning"), ldaError);
        return;
    }

    QString prototypeName = lda.getPrototype();

    QFileInfo idFileInfo(idFilePath);
    protoFilePath = idFileInfo.path() + "/" + prototypeName;
    protoUpdate();

    // prototypeImg.load(protoFilePath);
    // if(prototypeImg.isNull())
    //     QMessageBox::warning(this, tr("Warning"),
    //                          tr("Cannot open file %1")
    //                              .arg(prototypeName));
}

bool MainWindow::protoLoadDir(QDir dir)
{
    QString inDirName = dir.absolutePath() + "/prototype";
    QDir inDir(inDirName);

    QString thePath = inDir.absolutePath();

    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return false;

    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString localFileName = localFileInfo.fileName();
        if(localFileName.startsWith("prototype-")){
            protoFilePath = thePath + "/" + localFileName;
            protoUpdate();
            return true;
        }
    }
    return false;
}

QDomDocument MainWindow::loadXmlDoc(QString &inFileName)
{
    QDomDocument xmlDoc;
    QFile file(inFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot open file %1:\n%2")
                                 .arg(inFileName, file.errorString()));
        return QDomDocument();
    }

    QString errorStr;
    int errorLine;
    int errorColumn;
    if (!xmlDoc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        file.close();
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, tr("Warning"),
                             tr("Parse error in file %1, at line %2, column %3: %4")
                                 .arg(inFileName)
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return QDomDocument();
    }

    file.close();
    return xmlDoc;
}

// find outline
void MainWindow::semiOutline()
{
    if(isNoImage())
        return;

    // otlThd = info.getOutlineThd();
    int otlThd = 128;

    if(!screenImg.isGrayTable())
        screenImg = screenImg.convertToGray8();
    screenImg.thresholdFast(otlThd);
    view->setImg(screenImg);
    thresholdDialog * dialog = new thresholdDialog(otlThd, this);
    dialog->exec();
    if(dialog->wasCanceled())
    {
        screenImg.setGrayTable();
        view->setImg(screenImg);
        return;
    }

    Outline maybeWingsOutlines;
    maybeWingsOutlines.fromImg(&screenImg);
    screenImg.setGrayTable();
    pxlList* outline;
    outline = maybeWingsOutlines.maxCCOutline();
    QString outlineString = outline->toTxtChain();
    info.setOutline(outlineString);
    saveOn();

    overImg = QImage(screenImg.width(), screenImg.height(), QImage::Format_Indexed8 );
    overImg.fill(255);
    overImg.markList(outline, 0);

    QRgb semiGreen = 0x8000FF00;
    QRgb semiRed = 0x80FF0000;
    QRgb transparent = 0x00000000;
    int thd = 255;
    overImg.setColor ( 0, semiRed );
    for(int i = 1; i < thd; i++)
        overImg.setColor ( i, semiGreen );
    for(int j = thd ; j < 256; j++)
        overImg.setColor ( j, transparent );

    view->setOverlay(overImg);
    view->setImg(screenImg);

    //    QString outString = outline->toTpsChain();
    //    QFile data("E:/wings/tps/chain.txt");
    //    if (data.open(QFile::WriteOnly)) {
    //        QTextStream out(&data);
    //        out << outString;
    //    }
}

void MainWindow::semiOutlineDir()
{
    bool ok;
    unsigned char colorThd = QInputDialog::getInt(this, "", tr("Color threshold for cropping the image)"),
                                                  128, 0, 255, 1, &ok);
    if(!ok)
        return;

    QString inDirName = QFileDialog
        ::getExistingDirectory(this, tr("Choose a directory"),
                               filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    clear();

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.dw.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;
    for (int i = 0; i < localFileInfoList.size(); ++i) {

        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        QApplication::processEvents(); // for statusBar message refresh

        dwImage theImg(fileName);
        Outline outlines;
        if(theImg.isGrayTable())
        {
            theImg.threshold(colorThd);
            outlines.fromImg(&theImg);
            theImg.setGrayTable();
        }else
        {
            dwImage tempImg = theImg;
            tempImg.detach();
            tempImg = tempImg.convertToGray8();
            tempImg.threshold(colorThd);
            outlines.fromImg(&tempImg);
        }
        pxlList* outlineMax;
        outlineMax = outlines.maxCCOutline();
        QString outlineString = outlineMax->toTxtChain();
        info.setOutline(outlineString);

        WingInfo theInfo;
        theInfo.fromImg(theImg);
        theInfo.setOutline(outlineString);
        theInfo.toImg(theImg);
        theImg.save(fileName, "PNG");
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Finished"), 2000);
}

// find semilandmarks
void MainWindow::semiLand1()
{
    QString outlineString = info.getOutline();
    if(outlineString.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Create an outline first"));
        return;
    }
    pxlList outline;
    outline.fromTxt(outlineString);

    std::vector < Coord > landmarks = view->landmarks();
    if(landmarks.size() < 1)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Number of landmarks different than %1").arg(1));
        return;
    }
    Coord lastPoint = landmarks.front();

    bool ok;
    unsigned char semiLndSize = QInputDialog::getInt(this, "", tr("Number of semilandmarks"),
                                                     50, 0, 99, 1, &ok);
    if(!ok) return;

    std::vector<Coord> semilandmarks = outline.semilandmarks(semiLndSize, lastPoint);
    view->setMarkers(semilandmarks);
}

void MainWindow::semiLand2()
{
    QString outlineString = info.getOutline();
    if(outlineString.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Create an outline first"));
        return;
    }
    pxlList outline;
    outline.fromTxt(outlineString);

    std::vector < Coord > landmarks = view->landmarks();
    if(landmarks.size() < 2)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Number of landmarks different than %1").arg(2));
        return;
    }
    Coord fromPoint = landmarks[0];
    Coord toPoint = landmarks[1];

    bool ok;
    unsigned char semiLndSize = QInputDialog::getInt(this, "", tr("Number of semilandmarks"),
                                                     50, 0, 99, 1, &ok);
    if(!ok) return;

    std::vector<Coord> semilandmarks = outline.semilandmarks2points(semiLndSize, fromPoint, toPoint);
    view->setMarkers(semilandmarks);
}

void MainWindow::semiLand2Dir()
{
    bool ok;
    unsigned char semiLndSize = QInputDialog::getInt(this, "", tr("Number of semilandmarks"),
                                                     50, 0, 99, 1, &ok);
    if(!ok)
        return;

    QString inDirName = QFileDialog
        ::getExistingDirectory(this, tr("Choose a directory"),
                               filePath, QFileDialog::ShowDirsOnly);
    if (inDirName.isEmpty())
        return;

    clear();

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.dw.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        QApplication::processEvents(); // for statusBar message refresh

        dwImage theImg(fileName);
        WingInfo theInfo;
        theInfo.fromImg(theImg);

        QString outlineString = theInfo.getOutline();
        if(outlineString.isEmpty())
            continue; // report error
        pxlList outline;
        outline.fromTxt(outlineString);

        std::vector < Coord > landmarks = theInfo.getLandmarks();
        if(landmarks.size() < 2)
            continue; // report error
        Coord fromPoint = landmarks[0];
        Coord toPoint = landmarks[1];
        std::vector<Coord> semilandmarks = outline.semilandmarks2points(semiLndSize, fromPoint, toPoint);
        theInfo.setLandmarks(semilandmarks);
        theInfo.toImg(theImg);
        theImg.save(fileName, "PNG");
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Finished"), 2000);
}

void MainWindow::semiAlign()
{
    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                          filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    QDir inDir( inDirName );
    filePath = inDir.absolutePath();

    dwRclList lmkRcl;
    dwRclList otlRcl;
    lmkRcl.setId(inDir.dirName());
    QStringList extList = (QStringList() << "*.dw.png");
    QFileInfoList fileList = inDir.entryInfoList(extList, QDir::Files, QDir::Name );
    for (int i = 0; i < fileList.size(); ++i)
    {
        QFileInfo localFileInfo = fileList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        dwImage img(fileName);
        WingInfo localInfo;
        localInfo.fromImg(img);
        dwRCoordList lmkList(localInfo.getLandmarks());
        if(lmkList.size() == 0)
            continue;
        lmkList.setId(localFileInfo.absoluteFilePath());

        QString outlineString = localInfo.getOutline();
        pxlList outline;
        if(!outlineString.isEmpty())
            outline.fromTxt(outlineString);
        else
            continue;
        std::vector< Coord > otlVec = outline.toVec();
        dwRCoordList otlList(otlVec);

        lmkRcl.push_back(lmkList);
        otlRcl.push_back(otlList);

    }

    dwSLAO slao(lmkRcl, otlRcl);
    lmkRcl= slao.align();

    // write aligned landmarks back to files
    std::vector< dwRCoordList > tpsVec = lmkRcl.list();
    for(unsigned i = 0; i < tpsVec.size(); ++i){
        dwRCoordList rcl = tpsVec[i];
        QString theImgName = rcl.getId();
        dwImage theImg(theImgName);

        if (theImg.isNull()) {
            statusBar()->showMessage(tr("Cannot open file %1").arg(theImgName), 2000);
            continue;
        }
        statusBar()->showMessage(tr("Writing %1").arg(theImgName));

        WingInfo pngInfo;
        pngInfo.fromImg(theImg);
        std::vector< realCoord > junc = rcl.list();
        pngInfo.setLandmarks(junc);
        pngInfo.toImg(theImg);
        theImg.save( theImgName, "PNG");
    }

    QApplication::restoreOverrideCursor();
}

void MainWindow::setScaleBar()
{
    //    double scaleRefLength = info.getScaleRefLength();
    if(info.hasScaleBar()){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Scale bar exists, please remove it first"));
        return;
    }
    std::vector < Coord > landmarks = view->landmarks();
    if(landmarks.size() < 2){
        QMessageBox::warning(this, tr("Warning"),
                             tr("At least two landmarks are required in order to set the scale bar"));
        return;
    }
    bool ok;
    double inNumber = QInputDialog::getDouble(this, "", tr("Reference length in meters (for example 1mm=0.001m)"), 0.0, 0, 100000, 6, &ok);
    if(!ok) return;

    if(inNumber == 0){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Input value has to be greater than 0"));
        return;
    }

    view->setScaleBar();
    std::vector< Coord > scaleBar = view->getScaleBar();
    info.setScaleBar(scaleBar, inNumber);
    saveOn();
}

void MainWindow::clearScaleBar()
{
    info.clearScaleBar();
    view->clearScaleBar();
    saveOn();
}

void MainWindow::setResolution()
{
    if(info.hasScaleBar()){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Scale bar exists, please remove it first"));
        return;
    }

    double resolution = info.getResolution();
    bool ok;
    double inNumber = QInputDialog::getDouble(this, "", tr("Image resolution in pixels per meters"),
                                              resolution, 0, 10000000, 10, &ok);
    if(!ok) return;

    if(inNumber == 0){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Input value has to be greater than 0"));
        return;
    }
    info.setResolution(inNumber);
    saveOn();
}

void MainWindow::setResolutionDir()
{
    bool ok;
    double inNumber = QInputDialog::getDouble(this, "", tr("Image resolution in pixels per meters"),
                                              0, 0, 10000000, 10, &ok);
    if(!ok) return;

    if(inNumber == 0){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Input value has to be greater than 0"));
        return;
    }

    QString inDirName = QFileDialog::getExistingDirectory(
        this, tr("Choose a directory"),
        filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        QApplication::processEvents(); // fro statusBar message refresh

        dwImage img(fileName);
        WingInfo localInfo;
        localInfo.fromImg(img);


        localInfo.clearScaleBar(); //just in case
        localInfo.setResolution(inNumber);

        localInfo.toImg(img); //write wing info to image
        img.save( fileName, "PNG");

    }
    statusBar()->showMessage(tr("Finished"), 2000);
    QApplication::restoreOverrideCursor();
}

void MainWindow::scaleInfo()
{
    if(isNoImage())
        return;

    QString outTxt;
    double res = info.getResolution();
    if(res > 0)
    {
        QString resStr = QString::number(res,'f', 6);
        outTxt = tr("Image resolution = %1 pixels per meter").arg(resStr);
    }else
        outTxt = tr("Image resolution was not set");
    outTxt += "\n";

    if(info.hasScaleBar())
    {
        double scaleRef = info.getScaleReference();
        QString argStr = QString::number(scaleRef,'f');
        outTxt += tr("Scale bar reference length = %1 meters").arg(argStr);
        outTxt += "\n";

        std::vector< Coord > scaleBar = info.getScaleBar();
        double distance = coordDistance(scaleBar[0], scaleBar[1]);
        argStr = QString::number(distance,'f', 6);
        outTxt += tr("Scale bar length = %1 pixels").arg(argStr);
        outTxt += "\n";
        outTxt += tr("Scale bar landmarks: ");
        Coord flipped = scaleBar[0]; //for conversion to cartesian coordinates
        flipped.flip(inImg.height());
        outTxt += flipped.toTxt();
        outTxt += " - ";
        flipped = scaleBar[1];
        flipped.flip(inImg.height());
        outTxt += flipped.toTxt();
    }
    else
        outTxt += tr("No scale bar");
    QMessageBox::about(this, "", outTxt);
}

// set classification file
void MainWindow::idFile()
{
    QFileInfo idFileInfo(idFilePath);
    QString xmlDir = idFileInfo.path();

    QString inFileName = QFileDialog
        ::getOpenFileName(this,
                          tr("Set classification") + tr("Open file"), xmlDir,
                          tr(".DW.XML files (*.dw.xml)"));
    if (inFileName.isEmpty())
        return;
    QDomDocument XmlDoc = loadXmlDoc(inFileName);
    if(XmlDoc.isNull())
        return;
    dwLdaGm lda(XmlDoc);
    QString ldaError = lda.getError();
    if(ldaError != "")
    {
        QMessageBox::warning(this, tr("Warning"), ldaError);
        return;
    }

    idFilePath = inFileName;
    QFileInfo newFileInfo(idFilePath);
    QString idFileName = newFileInfo.fileName();
    classLbl->setText(idFileName); // update status bar

    QString protoFileName = lda.getPrototype();
    protoFilePath = newFileInfo.path() + "/" + protoFileName;

    prototypeImg.load(protoFilePath);
    if(prototypeImg.isNull())
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot open file %1")
                                 .arg(protoFileName));
    protoLbl->setText(protoFileName); // update status bar
}

// classify currently open data using classifiction from xml file
void MainWindow::id()
{
    QDomDocument XmlDoc = loadXmlDoc(idFilePath);
    if(XmlDoc.isNull())
        return;

    dwLdaGm lda(XmlDoc);
    QString ldaError = lda.getError();
    if(ldaError != "")
    {
        QMessageBox::warning(this, tr("Warning"), ldaError);
        return;
    }

    //get data to be classified
    dwRCoordList currData(view->landmarks());
    currData.setId(inFileInfo.fileName());
    currData.flip(screenImg.height()); //convert to cartesian

    unsigned minSize = lda.getReferenceSize();
    if(currData.size() != minSize){
        QMessageBox::warning(this, tr("Warning"),
                             tr("Difference in number of landmarks between reference (%1) and data (%2)")
                                 .arg(minSize).arg(currData.size()) );
        return;
    }
    dwRclList rcl;
    rcl.push_back(currData);
    QString outData = idOutput(rcl, lda);

    TxtWindow * dataWindow = new TxtWindow(outData, this);
    dataWindow->setWindowTitle(tr("Results")+"[*]");
    dataWindow->show();
}

QString MainWindow::idOutput(dwRclList & rcl, dwLdaGm & lda)
{
    QString outData;

    outData += "<br>";

    if(lda.hasLda())
    {
        outData += tr("Most similar class") + "<b>";
        outData += lda.classifyTab(rcl);
        outData += "</b><br>";

        outData += tr("LDA scores");
        dwDataSet ldaScoresData = lda.ldaScores(rcl);
        outData += ldaScoresData.toHtmlTable();
        outData += "<br>";
    }

    dwDataSet probData = lda.cvaProb(rcl);

    outData += "<br>";
    outData += tr("Most similar class") + "<b>";

    outData += probData.toHtmlMaxValue();
    outData += "</b><br>";

    outData += tr("CVA probabilities");
    outData += probData.toHtmlTable();

    QImage diagram = lda.cvaDiagram(rcl);
    QString diagramFileName = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "identiflyCVA.png";
    diagram.save(diagramFileName);
    outData += "<br>";

    outData += tr("CVA graph");
    outData += QString("<p><img border=\"0\" src=\"%1\" width=\"%2\" height=\"%3\"></p>")
                   .arg(diagramFileName).arg(diagram.width()).arg(diagram.height());
    outData += "<br>";

    outData += tr("CVA scores");
    dwDataSet scoresData = lda.cvaScores(rcl);
    outData += scoresData.toHtmlTable();

    return outData;
}

// classify average from directory using classifiction from xml file
void MainWindow::idDir()
{
    QDomDocument XmlDoc = loadXmlDoc(idFilePath);
    if(XmlDoc.isNull())
        return;

    dwLdaGm lda(XmlDoc);
    QString ldaError = lda.getError();
    if(ldaError != "")
    {
        QMessageBox::warning(this, tr("Warning"), ldaError);
        return;
    }

    //choose data to be classified
    //    QFileInfo fileInfo(filePath);
    //    QString dirName = ;
    QString inDirName = QFileDialog::getExistingDirectory(
        this, tr("Choose a directory"),
        filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    dwRclList rcl;

    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.dw.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;

    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        dwImage img(fileName);
        WingInfo localInfo;
        QString fn = localFileInfo.fileName();
        localInfo.fromImg(img);
        localInfo.setFileName(fn);
        dwRCoordList theList = localInfo.toCoordList();
        rcl.push_back(theList);
    }

    //error checking
    // todo: add more information about missing landmarks
    int minSize = rcl.getMinSize();
    int refSize = lda.getReferenceSize();
    if(minSize != refSize)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Number of landmarks different than %1").arg(refSize));
        return;
    }

    dwRCoordList ref = rcl.superimposeGPA();
    ref.setId(inDir.dirName());
    ref.flip();
    dwRclList refRcl;
    refRcl.push_back(ref);
    QString outData = idOutput(refRcl, lda);

    TxtWindow * dataWindow = new TxtWindow(outData, this);
    dataWindow->setWindowTitle(tr("Results")+"[*]");
    dataWindow->show();
}

// classify all files from directory using classifiction from xml file
void MainWindow::idDirAll()
{
    QDomDocument XmlDoc = loadXmlDoc(idFilePath);
    if(XmlDoc.isNull())
        return;

    dwLdaGm lda(XmlDoc);
    QString ldaError = lda.getError();
    if(ldaError != "")
    {
        QMessageBox::warning(this, tr("Warning"), ldaError);
        return;
    }

    //choose data to be classified
    QString inDirName = QFileDialog::getExistingDirectory(
        this, tr("Choose a directory"),
        filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    dwRclList rcl;

    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.dw.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;

    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        dwImage img(fileName);
        WingInfo localInfo;
        QString fn = localFileInfo.fileName();
        localInfo.fromImg(img);
        localInfo.setFileName(fn);
        dwRCoordList theList = localInfo.toCoordList();
        rcl.push_back(theList);
    }

    rcl.flip();
    QString outData = idOutput(rcl, lda);

    TxtWindow * dataWindow = new TxtWindow(outData, this);
    dataWindow->setWindowTitle(tr("Results")+"[*]");
    dataWindow->show();
}

// classify directories from directory using classifiction from xml file
void MainWindow::idDirDir()
{
    QDomDocument XmlDoc = loadXmlDoc(idFilePath);
    if(XmlDoc.isNull())
        return;

    dwLdaGm lda(XmlDoc);
    QString ldaError = lda.getError();
    if(ldaError != "")
    {
        QMessageBox::warning(this, tr("Warning"), ldaError);
        return;
    }

    //choose data to be classified
    QString inDirName = QFileDialog::getExistingDirectory(
        this, tr("Choose a directory"),
        filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    dwRclList rcl;

    QDir inDir(inDirName);
    QFileInfoList localFileInfoList = inDir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;

    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString inSubDirName = localFileInfo.absoluteFilePath();

        QDir inSubDir( inSubDirName );
        filePath = inSubDir.absolutePath();
        QFileInfoList subFileInfoList = inSubDir.entryInfoList( QStringList("*.dw.png"), QDir::Files, QDir::Name );
        if(subFileInfoList.size() == 0) continue;

        dwRclList subRcl;

        for (int j = 0; j < subFileInfoList.size(); ++j) {
            QFileInfo subFileInfo = subFileInfoList.at(j);
            QString fileName = subFileInfo.absoluteFilePath();
            dwImage img(fileName);
            WingInfo localInfo;
            QString fn = subFileInfo.fileName();
            localInfo.fromImg(img);
            localInfo.setFileName(fn);
            dwRCoordList theList = localInfo.toCoordList();
            subRcl.push_back(theList);
        }
        dwRCoordList ref = subRcl.superimposeGPA();
        ref.setId(inSubDir.dirName());
        ref.flip();
        rcl.push_back(ref);
    }
    QString outData = idOutput(rcl, lda);

    TxtWindow * dataWindow = new TxtWindow(outData, this);
    dataWindow->setWindowTitle(tr("Results")+"[*]");
    dataWindow->show();
}

// classify data from dw.png, tps or csv file
void MainWindow::idData()
{
    QDomDocument XmlDoc = loadXmlDoc(idFilePath);
    if(XmlDoc.isNull())
        return;

    dwLdaGm lda(XmlDoc);
    QString ldaError = lda.getError();
    if(ldaError != "")
    {
        QMessageBox::warning(this, tr("Warning"), ldaError);
        return;
    }

    dwRclList rcl = getRclData();
    if(rcl.size() == 0)
        return;

    int refSize = lda.getReferenceSize();
    if(rcl.getMinSize() < refSize)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Number of landmarks different than %1").arg(refSize));
        return;
    }

    QString outData = idOutput(rcl, lda);
    TxtWindow * dataWindow = new TxtWindow(outData);
    dataWindow->setWindowTitle(tr("Results")+"[*]");
    dataWindow->show();
    clear();
}

dwRclList MainWindow::getRclData()
{
    dwRclList rcl;

    //choose data
    QString dataName = QFileDialog
        ::getOpenFileName(this, tr("Choose data"), filePath,
                          tr("Data files .DW.PNG, .CSV, .TPS (*.dw.png *.csv *.tps)"));
    if(dataName.isEmpty())
        return rcl;

    QFile dataFile(dataName);
    if (!dataFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Cannot open file %1:\n%2")
                                 .arg(dataName, dataFile.errorString()));
        return rcl;
    }

    QFileInfo dataFileInfo(dataName);
    inFileInfo = dataFileInfo; // for diagram file

    if(dataFileInfo.completeSuffix() == "dw.png"){
        dwImage img(dataName);
        WingInfo localInfo;
        QString fn = dataFileInfo.fileName();
        localInfo.fromImg(img);
        localInfo.setFileName(fn);
        dwRCoordList theList = localInfo.toCoordList();
        theList.flip(); //convert to cartesian coordinates
        rcl.push_back(theList);
    }else if(dataFileInfo.suffix() == "csv"){
        dwDataSet dataSet;
        dataSet.fromCsv(dataName);
        rcl = dataSet.toRclList();
    }else if(dataFileInfo.suffix() == "tps"){
        rcl.fromTps(dataName);
    }
    return rcl;
}

void MainWindow::about()
{
    // "<p>If you find this software useful please cite it:<br />Tofilski A (20xx) . <a href=\"https://doi.org/xxx\">https://doi.org/xxx</a></p>"

    QString aboutTxt(tr(
        "<p><b>%1 version %2</b></p>"
        "<p>Author: Adam Tofilski</p>"
        "<p>Home page: <a href=\"http://drawwing.org/identifly\">drawwing.org/identifly</a></p>"
        "<p>This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.</p>"
        "<p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. </p>"
        "<p>You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.</p>"
        ));

    QMessageBox::about(this, tr("About"), aboutTxt.arg(QGuiApplication::applicationDisplayName(), QGuiApplication::applicationVersion()));
}

void MainWindow::getImageText()
{
    if(isNoImage())
        return;

    QString outTxt = info.getImageText(inImg);
    if(outTxt.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("There is no image tekst."));
        return;
    }

    TxtWindow * dataWindow = new TxtWindow(outTxt);
    dataWindow->show();
}

void MainWindow::setImageText()
{
    if(isNoImage())
        return;

    bool ok;
    //: translation optional
    QString inText = QInputDialog::getText(this, "",
                                           tr("Image text separated with one colon"), QLineEdit::Normal, "", &ok);
    if(!ok)
        return;
    if(inText.isEmpty())
        return;

    info.fromTxt(inText);
    saveOn();
}

// convert csv to tps
void MainWindow::convertCsvTps()
{
    QString inFileName = QFileDialog::getOpenFileName(this,
                                                      tr("Open File"), filePath, "CSV file (*.csv)");
    if (inFileName.isEmpty()) return;
    QFileInfo inFileInfo(inFileName);

    dwRclList rcl;
    rcl.fromCsv(inFileName);
    QString text = rcl.toTps();

    QString name = inFileInfo.path() + "/" + inFileInfo.baseName()+".tps";
    QString outFileName = QFileDialog::getSaveFileName(
        this, tr("Save as"), name,
        "TPS files (*.tps)");
    if(outFileName.isEmpty()) return;

    QFile file(outFileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) return;
    QTextStream out(&file);
    out << text;
    file.close();
}

// reconfigure landmark configuration
// using xml file
void MainWindow::reconfig()
{
    QString xmlDir = QCoreApplication::applicationDirPath();
    xmlDir += "/dwxml";

    QString xmlName = QFileDialog::getOpenFileName(this, tr("Open file"), xmlDir, tr("XML files (*.dw.xml)"));

    QDomDocument XmlDoc = loadXmlDoc(xmlName);
    if(XmlDoc.isNull())
        return;

    QString inDirName = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
                                                          filePath, QFileDialog::ShowDirsOnly);

    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );
    filePath = inDir.absolutePath();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;

    WingInfo tmpInfo;

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();
        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        QApplication::processEvents(); // for statusBar message refresh

        dwImage theImg(fileName);
        if(theImg.isNull())
            continue;

        WingInfo localInfo;
        QString localName = localFileInfo.fileName();
        localInfo.fromImg(theImg);
        localInfo.setFileName(localName);
        localInfo.XmlReconfiguration(XmlDoc);
        localInfo.toImg(theImg); //write wing info to image

        theImg.save( fileName, "PNG");
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Finished"), 2000);
}


// adjust one landmark
void MainWindow::test()
{
    if(isNoImage())
        return;

    std::vector< Coord > landmarks = view->landmarks();
    if( landmarks.size() == 0 ){
        QMessageBox::warning(this, tr("Warning"), tr("No data available"));
        return;
    }

    if(prototypeImg.isNull())
        protoLoad();
    if(prototypeImg.isNull())
        return;


    WingInfo prototypeInfo;
    prototypeInfo.fromImg(prototypeImg);
    std::vector< Coord > prototypeList = prototypeInfo.getLandmarks();

    bool ok;
    int landmarkMax = prototypeList.size();
    int theOne = QInputDialog::getInt(this, "", tr("Landmark number"), 1, 1, landmarkMax, 1, &ok);
    if(!ok)
        return;
    --theOne; // indexing starts with 0

    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    info.setLandmarks(landmarks);
    info.toImg(screenImg);

    dwAdjust adj(screenImg, prototypeImg);
    Coord adjusted = adj.adjustedOne(theOne);
    landmarks[theOne] = adjusted;


    view->setMarkers(landmarks);
    saveOn();

    QApplication::restoreOverrideCursor();
}

