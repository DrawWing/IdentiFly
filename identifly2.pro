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
    src/TPS.cpp \
    src/coord.cpp \
    src/dwAdjust.cpp \
    src/dwChisqr.cpp \
    src/dwData.cpp \
    src/dwDataSet.cpp \
    src/dwImageViewer.cpp \
    src/dwImg.cpp \
    src/dwMarker.cpp \
    src/dwOptions.cpp \
    src/dwPairLR.cpp \
    src/dwRCoordList.cpp \
    src/dwRclList.cpp \
    src/dwSLAO.cpp \
    src/dwView.cpp \
    src/dwldagm.cpp \
    src/dwvector.cpp \
    src/histogram.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/nodepxl.cpp \
    src/outline.cpp \
    src/pxllist.cpp \
    src/realcoord.cpp \
    src/thresholdDialog.cpp \
    src/txtwindow.cpp \
    src/winginfo.cpp

HEADERS += \
    src/TPS.h \
    src/coord.h \
    src/dwAdjust.h \
    src/dwChisqr.h \
    src/dwData.h \
    src/dwDataSet.h \
    src/dwImageViewer.h \
    src/dwImg.h \
    src/dwMarker.h \
    src/dwOptions.h \
    src/dwPairLR.h \
    src/dwRCoordList.h \
    src/dwRclList.h \
    src/dwSLAO.h \
    src/dwView.h \
    src/dwldagm.h \
    src/dwvector.h \
    src/histogram.h \
    src/mainwindow.h \
    src/nodepxl.h \
    src/outline.h \
    src/pxllist.h \
    src/realcoord.h \
    src/thresholdDialog.h \
    src/txtwindow.h \
    src/winginfo.h

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets/identifly.qrc

win32:RC_ICONS = assets/identifly.ico

TARGET = IdentiFly
VERSION = 2.0.0
DEFINES += VERSION_STRING=\\\"2.0.0\\\"

win32 {
DESTDIR = $$PWD/../IdentiFly-windows-binaries
QMAKE_POST_LINK =  windeployqt $$shell_path($$DESTDIR/$${TARGET}.exe)
}

DISTFILES += \
    README.Rmd \
    README.md

