#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QCloseEvent>
#include <QFileInfo>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QSlider>
#include <QToolBar>
#include <QToolButton>
#include <QDir>
#include <QDomDocument>

#include "dwImg.h"
#include "dwView.h"
#include "winginfo.h"
#include "dwRclList.h"
#include "dwldagm.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateOptions();
    int getZoom();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void openFile();
    void openDir();
    void openNext();
    void openPrev();
    // void updateStatus( QPointF );
    // void openNext();
    // void openPrev();
    void openRecentFile();
    void removeFile();
    bool save();
    bool saveAs();
    void saveOn();
    void rename();
    void addDirPrefix();
    void updateStatus( QPointF );
    void addMode();
    void dragMode();
    void moveMode();
    void importTps();
    bool saveAsTps(QString & fileName);
    void exportData();
    void exportDataDir();
    void preferences();

    void flipHor();
    void flipVer();
    void rotate();
    void grayscale();
    void crop();
    void cropDir();
    void scaleDir();
    void threshold(int); // used by thresholdDialog

    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomActual();
    void zoomLandmarks();
    void zoom3();
    void zoom4();

    void dataLmak();
    void dataDist();
    void dataCs();

    void lmarkDel();
    void lmarkReset();
    void lmarkResetDir();
    void lmarkFit();
    void lmarkFitDir();
    void lmarkOut();
    void lmarkOneOut();
    void lmarkPairOut();

    void protoSet();
    void protoView();

    // move from slots
    void protoUpdate();
    void protoDefault();
    void protoLoad();
    bool protoLoadDir(QDir dir);
    QDomDocument loadXmlDoc(QString &inFileName);

    void semiOutline();
    void semiOutlineDir();
    void semiLand1();
    void semiLand2();
    void semiLand2Dir();
    void semiAlign();

    void setScaleBar();
    void clearScaleBar();
    void setResolution();
    void setResolutionDir();
    void scaleInfo();

    void idFile();
    void id();
    void idDir();
    void idDirAll();
    void idDirDir();
    void idData();

    void about();

    void getImageText();
    void setImageText();
    void convertCsvTps();
    void reconfig();
    void test();

private:
    void clear();
    void createActions();
    void createMenus();
    void createStatusBar();
    void createToolBars();
    bool isNoImage();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool openDwPng(const QString &);
    void openImgActions();
    bool saveDwPng(QString & fileName);
    void updateRecentFileActions();
    void resetActions();
    void readSettings();
    void writeSettings();
    QString idOutput(dwRclList & rcl, dwLdaGm & lda);
    dwRclList getRclData();

    View *view;
    dwImage inImg;
    dwImage screenImg; //shalow copy of displayed image //screenImg is required for thdDialog
    dwImage overImg; //mostly transparent image with important features
    dwImage prototypeImg; //master of landmark configuration
    WingInfo info;
    bool differencesLR; // show the differences only between left and light
    dwRCoordList reference; //for drawing difference lines

    QAction *openAct;
    QAction *openDirAct;
    QAction *openNextAct;
    QAction *openPrevAct;
    // QAction *openDirAct;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActions[MaxRecentFiles];
    QAction *removeFileAct;
    QAction *separatorAction;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *moveModeAct;
    QAction *dragModeAct;
    QAction *addModeAct;
    QAction *importTpsAct;
    QAction *exportDataAct;
    QAction *exportDataDirAct;
    QAction *preferencesAct;
    QAction *exitAct;

    QAction *flipHorAct;
    QAction *flipVerAct;
    QAction *rotateAct;
    QAction *grayscaleAct;
    QAction *cropAct;
    QAction *cropDirAct;
    QAction *scaleDirAct;

    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *zoomFitAct;
    QAction *zoomActualAct;
    QAction *zoomLandmarksAct;
    QAction *zoom3Act;
    QAction *zoom4Act;

    QAction *dataLmakAct;
    QAction *dataCsAct;
    QAction *dataDistAct;

    QAction *lmarkDelAct;
    QAction *lmarkResetAct;
    QAction *lmarkResetDirAct;
    QAction *lmarkFitAct;
    QAction *lmarkFitDirAct;
    QAction *lmarkOutAct;
    QAction *lmarkOneOutAct;
    QAction *lmarkPairOutAct;
    QAction *protoSetAct;
    QAction *protoViewAct;

    QAction *semiOutlineAct;
    QAction *semiOutlineDirAct;
    QAction *semiLandAct;
    QAction *semiLand2Act;
    QAction *semiLand2DirAct;
    QAction *semiAlignAct;

    QAction *setScaleBarAct;
    QAction *clearScaleBarAct;
    QAction *setResolutionAct;
    QAction *setResolutionDirAct;
    QAction *scaleInfoAct;

    QAction *idFileAct;
    QAction *idAct;
    QAction *idDirAct;
    QAction *idDirAllAct;
    QAction *idDirDirAct;
    QAction *idDataAct;

    QAction *aboutDwAct;
    QAction *getImageTextAct;
    QAction *setImageTextAct;
    QAction *convertCsvTpsAct;
    QAction *reconfigAct;
    QAction *renameAct;
    QAction *addDirPrefixAct;
    QAction *testAct;

    QLabel * protoLbl; // for statusbar
    QLabel * classLbl; // for statusbar
    QLabel * coordLbl; // for statusbar
    QString filePath; // path without file name // to replace with inFileInfo.absoluteFilePath()
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolButton *moveModeBottom;
    QToolButton *dragModeButton;
    QToolButton *addModeBottom;
    QSlider *zoomSlider;

    QMenu *fileMenu;
    QMenu *modeMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *dataMenu;
    QMenu *lmarkMenu;
    QMenu *semiMenu;
    QMenu *scaleMenu;
    QMenu *idMenu;
    QMenu *helpMenu;
    QMenu *testMenu;

    QFileInfo inFileInfo;
    QFileInfoList fileInfoList; //list of files
    int fileInfoCount; //the index of current file
    QStringList recentFiles;

    // options
    int loadZoomOpt;
    int zoom3Opt;
    int zoom4Opt;
    QString idFilePath; // path to file with identification information
    QString protoFilePath; // path to file with prototype image
};
#endif // MAINWINDOW_H
