QT       += core widgets gui xml printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\programs\opencv\build\include
LIBS += C:\programs32\opencv-build\bin\libopencv_core481.dll
LIBS += C:\programs32\opencv-build\bin\libopencv_highgui481.dll
LIBS += C:\programs32\opencv-build\bin\libopencv_imgproc481.dll
LIBS += C:\programs32\opencv-build\bin\libopencv_features2d481.dll
LIBS += C:\programs32\opencv-build\bin\libopencv_calib3d481.dll
LIBS += C:\programs32\opencv-build\bin\libopencv_ml481.dll
LIBS += C:\programs32\opencv-build\bin\libopencv_imgcodecs481.dll

SOURCES += \
    TPS.cpp \
    coord.cpp \
    dwAdjust.cpp \
    dwChisqr.cpp \
    dwData.cpp \
    dwDataSet.cpp \
    dwImageViewer.cpp \
    dwImg.cpp \
    dwMarker.cpp \
    dwOptions.cpp \
    dwPairLR.cpp \
    dwRCoordList.cpp \
    dwRclList.cpp \
    dwView.cpp \
    dwldagm.cpp \
    dwvector.cpp \
    histogram.cpp \
    main.cpp \
    mainwindow.cpp \
    nodepxl.cpp \
    outline.cpp \
    pxllist.cpp \
    realcoord.cpp \
    thresholdDialog.cpp \
    txtwindow.cpp \
    winginfo.cpp

HEADERS += \
    TPS.h \
    coord.h \
    dwAdjust.h \
    dwChisqr.h \
    dwData.h \
    dwDataSet.h \
    dwImageViewer.h \
    dwImg.h \
    dwMarker.h \
    dwOptions.h \
    dwPairLR.h \
    dwRCoordList.h \
    dwRclList.h \
    dwView.h \
    dwldagm.h \
    dwvector.h \
    histogram.h \
    mainwindow.h \
    nodepxl.h \
    outline.h \
    pxllist.h \
    realcoord.h \
    thresholdDialog.h \
    txtwindow.h \
    winginfo.h

TRANSLATIONS += \
    identifly2_pl_PL.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    identifly.qrc

RC_ICONS = identifly.ico

TARGET = IdentiFly
VERSION = 2.0.0
DEFINES += VERSION_STRING=\\\"2.0.0\\\"

win32 {
DESTDIR = $$PWD/../IdentiFly-windows-binaries
QMAKE_POST_LINK =  windeployqt $$shell_path($$DESTDIR/$${TARGET}.exe)
}

