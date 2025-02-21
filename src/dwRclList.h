//List of landmark configurations
//Cartesian coorinates.

#ifndef DWRCLLIST_H
#define DWRCLLIST_H

#include <QtWidgets>
#include <QString>

//#include "dwDataSet.h"
#include "dwRCoordList.h"
#include "realcoord.h"

class dwRclList
{
public:
    dwRclList(void);
    dwRCoordList at(unsigned i);
    void push_back(const dwRCoordList & );
    void fromTps(const QString &);
    void fromCsv(const QString &);
    void fromVector(const std::vector< dwRCoordList > &inVec);
    QString readTps(const QString &);
    QString toTps(void) const;
    QString toXml(void) const;
    QString toCsv(void) const;
//    dwDataSet toDataSet(void) const;
    void fromFile(const QString &);
    void fromImgDir(const QDir &);
    double superimpose(const dwRCoordList & reference);
    dwRCoordList superimposeGPA(void);
    dwRCoordList average(void) const;
    void center(void);
    void scale(void);
    int getMinSize() const;
    bool setValidSize(const int);
    void setId(QString inId);
    QString getId() const;
    std::vector< QString >  getIdList() const;
    QString find_outliers(const double) const;
    QString find_outliers(void) const;
    std::vector< QString > oneOutlier(unsigned index);
    std::vector< QString > outliers(void);
    bool isSizeEqual(void) const;
    QString sizeOutliers(void) const;
    QString findZeroCS(void) const;
    double meanCS(void) const; //Calculate average centroid size of the list.
    unsigned size(void) const; //size of the rclList
    std::vector< double > find_distances(const dwRCoordList &) const;
    std::vector< dwRCoordList > list(void) const;
    std::vector< realCoord > points(void) const;
    bool update(QString path);
    bool erase(QString path);
    void clear();
    void flip(const int height = 0); //Flip points verticaly for transformation between bitmap and cartesian coordinates
    QString toSVG(const QString & fileName, dwImage & image) const;
    QString toTxt() const;
    void setCoord(unsigned rclNo, unsigned coordNo, realCoord inCoord);
    ~dwRclList(void);

private:
    QString id;
    int minRcListSize;
    int maxRcListSize;
    std::vector< dwRCoordList > rclList;
    double superimpose_once(const dwRCoordList & reference);
    void rotate2first(void);
    void rotate2reference(dwRCoordList & reference);
    void scale( std::vector< double > & factor);
};
QString dir2xml(const QDir &);
QString dir2tps(const QDir &);

#endif // DWRCLLIST_H
