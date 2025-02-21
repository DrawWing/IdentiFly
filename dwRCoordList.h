#ifndef DWRCOORDLIST_H
#define DWRCOORDLIST_H

#include <vector>
#include <QString>
#include <QTextStream>

#include "realcoord.h"
#include "dwImg.h"

class dwRCoordList
{
public:
    dwRCoordList(void);
    dwRCoordList(const QString & resources);
    dwRCoordList(QTextStream & in);
    dwRCoordList(const std::vector< Coord > & coords);
    dwRCoordList(const std::vector< realCoord > &);
    void fromRealPxls(const std::vector< realCoord > & pixels);
    void fromPxls(const std::vector< Coord > & inList);
    void fromCsv(const QString &inStr, const QChar sep);
    void push_back(const realCoord & pxl);
    void pop_back();
    void clear();
    std::vector< realCoord > list() const;
    std::vector< Coord > coordList() const;
    double superimpose(const dwRCoordList & reference); // fully superimpose the list over reference
    double superimposePart(const dwRCoordList & reference); // partially superimpose the list over reference
    void superimposeNoScaling(const dwRCoordList & reference); // superimpose the list over reference by rotation and translation only
    void preshape();
    QString classify(const QString & resources);
    //double distanceCVA(const dwRCoordList & cv1, const dwRCoordList & cv2, const realCoord & ref) const;
    realCoord CVAcoefficients(const dwRCoordList & cv1, const dwRCoordList & cv2) const;
    unsigned size(void) const;
    QString toTps(void) const;
    QString toTxt(void) const;
    QString toCsv(void) const;
    QString toXml(void) const;
//    dwData toData(void) const;
    void fromTps(const QString &);
    void fromTxt(const QString & resources);
    void translate(realCoord thePnt);
    void add(realCoord thePnt);
    void center(void);
    void scale(const double factor);
    void rotate(const double angle);
    double centroidSize(void) const ;
    double rotationAngle(const std::vector< realCoord > & reference) const; //find angle that minimize distance between 2 configurations
    double rotationAngle(const dwRCoordList & reference) const; //find angle that minimize distance between 2 configurations
    double rotationAngleRaw(const dwRCoordList & reference) const; //find angle between unscaled and untranslated configuratins
    double meanDistance(void) const; //mean distance from centroid to points
    bool setValidSize(const int);
    void setList(const std::vector< Coord > & inList);
    void setId(const QString & newId);
    QString getId(void) const;
    void setCoord(unsigned coordNo, realCoord inCoord);
    double ci(void) const;
    double ci_alpatov(void) const;
    realCoord centroid() const;
    double find_distance(const dwRCoordList & ref, unsigned index) const;
    std::vector< double > find_distances(const dwRCoordList &) const;
    void flip(const int height = 0); //Flip points verticaly for transformation between bitmap and cartesian coordinates
    void flipHor(const int width = 0); //Flip points horizontaly
    void rotate90(const int height = 0); //Rotate points 90 degree clockwise
    void rotate2reference(const dwRCoordList &reference);
    QString listToSVG() const;
    QString listToTxt() const;
    QString toSVG(const QString & fileName, dwImage & image) const;
    void toImg() const;
    friend double traceABt(const dwRCoordList & a, const dwRCoordList & b);
    std::vector< Coord > find_outliers_list(std::vector< Coord > mean) ;
    void superimposeBC(dwRCoordList & reference);
    double distanceTo(const dwRCoordList & reference);
    double differenceVarLgh(const dwRCoordList &);
    double distance2image(dwImage & inImg, int radius); //distance between list and image
    int findCloses(const realCoord &inPoint);
    bool isNaN();
    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;
    void transform(const realCoord corner, const double angle = 0.0, const double inScale = 1.0);
    // double procrustesDistance(const std::vector< realCoord > & reference) const;
    double procrustesDistance(const dwRCoordList & reference) const;
    // double partialProcrustesDistance(const std::vector< realCoord > & reference) const;
    double partialProcrustesDistance(const dwRCoordList & reference) const;
    dwRCoordList reverse() const; // change order from last to first
    realCoord findNearest(realCoord toPxl);

private:
    QString id;
    int validSize;
    std::vector< realCoord > theList;

public:
    ~dwRCoordList(void);
};

#endif // DWRCOORDLIST_H
