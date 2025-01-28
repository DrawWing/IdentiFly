#ifndef WINGINFO_H
#define WINGINFO_H

#include <QDomDocument> // for xml

#include "dwImg.h"
#include "dwRCoordList.h"

class WingInfo
{
public:
    WingInfo(void);
    ~WingInfo(void);
    void clear();
    void toImg(QImage&) const;
    dwRCoordList toCoordList() const;
    QString toTxt() const;
    QString toTps() const;
    QString toCsvHead() const;
    QString toTxtHead() const;
    QString toCsv() const;
    QString toCsvRow() const;
    QString toTxtRow() const;
//    QString index() const;
    QString XmlExportHeader(QDomDocument XmlDoc) const;
    QString XmlExport(QDomDocument XmlDoc) const;
    void XmlReconfiguration(QDomDocument XmlDoc);
    void XmlReconfiguration1(QDomDocument XmlDoc);
    QString apisDistances() const;
    void fromImg(const QImage &);
    void fromTxt(const QString & inTxt);

    // !!! to be removed
    bool left; //is this a left wing;

    unsigned outlineThd; // synonim of wingThd wing_threshold
    int venationThd; //threshold for extracting venation outline
    int outlineIndex; // first landmark of outline
    double dwVersion;
    void setLandmarks(std::vector< Coord > &);
    void setLandmarks(const std::vector<realCoord> &);
    void setCoordList(const dwRCoordList & inList);
    void setFileName(QString &); //cała ścieżka czy tylko nazwa pliku??!!!!!!!!!
    void setDevice(QString &);
    void setOutline(QString inOutline);
    unsigned getOutlineThd() const;
    QString getDevice() const;
    QString getOutline() const;
    std::vector< Coord > getLandmarks() const;
    void flipHor();
    void flipVer();
    void rotate(); //Rotate points 90 degree clockwise
    void scale(double factor);
    double getScaleReference() const;
    std::vector< Coord > getScaleBar() const;
    void setScaleBar(std::vector< Coord > inVector, double inValue);
    void clearScaleBar();
    bool hasScaleBar() const;
    QString getImageText(dwImage inImg) const;
    void setResolution(double inValue);
    double getResolution() const;
    std::vector< int > getSequence() const;
//    int getFitSize() const;
private:
//    QString softName; //software name
//    double softVer; //software version
//    int exposureTime;
    std::vector< Coord > landmarks;
    std::vector< Coord > scaleBar; //two points to define scalebar - optional
    std::vector< int > sequence; //sequence in which points are fitted - optional
    QString outline; //outline to be saved
    double scaleRef; //reference corresnding to scalebar in meters
    QString device;
    double resolution; //image resolution in pixels per meter
//    int fitSize;

    QString fileName; //obecnie bez sciezki
    const QImage * image;
    QString points2txt(std::vector< Coord > inPoints) const;
    QString intVec2txt(std::vector< int > inPoints) const;
    std::vector< Coord > txt2points(QString & inTxt) const;
    std::vector< int > txt2intVec(QString & inTxt) const;
    double cubitalIndex() const; //cubital index
    double precubitalIndex() const; //precubital index
    double hantelIndex() const; //hantel index
    double discoidalShift() const; //discoidal shift in degrees
    double distance(int indexA, int indexB) const; // calculate distance between 2 landmarks of indexes A and B
    double distance1(int indexA, int indexB) const; // landmarks indexes start from 1
    double degAngle1(int indexA, int indexB, int indexC) const; // landmarks indexes start from 1
};

#endif // WINGINFO_H
