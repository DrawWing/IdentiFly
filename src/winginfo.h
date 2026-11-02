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
    QString XmlExportHeader(QDomDocument XmlDoc) const;
    QString XmlExport(QDomDocument XmlDoc) const;
    void XmlReconfiguration(QDomDocument XmlDoc);
    void XmlReconfiguration1(QDomDocument XmlDoc);
    void fromImg(const QImage &);
    void fromTxt(const QString & inTxt);

    unsigned outlineThd; // synonim of wingThd wing_threshold
    int venationThd; //threshold for extracting venation outline
    int outlineIndex; // first landmark of outline
    double dwVersion;
    void setLandmarks(std::vector< Coord > &);
    void setLandmarks(const std::vector<realCoord> &);
    void setFileName(QString &); //full path or only file name?
    void setOutline(QString inOutline);
    unsigned getOutlineThd() const;
    QString getOutline() const;
    std::vector< Coord > getLandmarks() const;
    void scale(double factor);
    double getScaleReference() const;
    std::vector< Coord > getScaleBar() const;
    void setScaleBar(std::vector< Coord > inVector, double inValue);
    void clearScaleBar();
    bool hasScaleBar() const;
    QString getImageText(dwImage inImg) const;
    void setResolution(double inValue);
    double getResolution() const;

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

    QString fileName; //currently without path
    const QImage * image;
    QString points2txt(std::vector< Coord > inPoints) const;
    QString intVec2txt(std::vector< int > inPoints) const;
    std::vector< Coord > txt2points(QString & inTxt) const;
    std::vector< int > txt2intVec(QString & inTxt) const;
    double distance1(int indexA, int indexB) const; // landmarks indexes start from 1
    double degAngle1(int indexA, int indexB, int indexC) const; // landmarks indexes start from 1
};

#endif // WINGINFO_H