#ifndef DWIMG_H
#define DWIMG_H

#include <qimage.h>

#include "coord.h"
#include "pxllist.h"
// #include <vector>
// #include <list>

class dwImage : public QImage   
{
public:
    dwImage();
    dwImage(const QImage & img);
    dwImage(const dwImage & img);
    dwImage(int w, int h, Format format = QImage::Format_Indexed8);
    dwImage( const QString & fileName, const char * format=0 );
    dwImage dwCopy();
    void copyText(dwImage & toImg) const;
    void from(QImage *);
    unsigned countNeighbours(unsigned row, unsigned col);
    unsigned countNotWhiteNeighbours(unsigned row, unsigned col);
    void findNeighbour(unsigned &row, unsigned &col);
    void threshold(unsigned);
    void markPxl(Coord, unsigned char color);
    void markNeighbours(const Coord & pxl,  unsigned char color);
    void markList(const pxlList&, unsigned char color);
    void markList(const pxlList *, unsigned char color = 0);
    dwImage cropOutline(const pxlList * outline, double angle);
    void mask(dwImage & maskImg, int thd);

    int outlineThd() const;
    void fillRow(int, unsigned char);
    void fillColumn(int, unsigned char);
    void clearRightMargin(int margin);
    void clearBorder(double thdFraction = 0.5, unsigned char colorThd = 150);
    void mean(int);
    void blur(int);
    void thresholdFast(int thd);
    void setGrayTable();
    bool isGrayTable() const;
    void qimageInfo();
    dwImage convertToGray8() const;
    // int getMargin() const;
    void addMargin(int, unsigned char color = 0xFF);
    unsigned interpolate(const realCoord pxl) const;
    dwImage simpleCrop( pxlList * outline, const int margin = 10) const;
    dwImage crop(const realCoord corner, const unsigned xSize, const unsigned ySize, const double angle) const;
    dwImage scale(const double ratio) const;
    dwImage getRotated() const;
    double meanImg(Coord thePnt, int size) const;
    double meanImg();
    void add(int); //add the same value to all pixel
    int difference(dwImage toCompare);
    double toNeighbour(Coord inPxl, int radius); //distance to neighbour in radius arond inPxl
    double toNeighbour(realCoord inPxl, int radius); //distance to neighbour in radius arond inPxl
    bool isCorner(unsigned x, unsigned y) const;
    bool isEdge(unsigned x, unsigned y) const;

    dwImage & operator=( const dwImage & );
    dwImage & operator=( const QImage & );
private:
    // int margin;
};

#endif // DWIMG_H
