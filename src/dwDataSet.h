#ifndef DWDATASET_H
#define DWDATASET_H

#include <opencv2/core/core.hpp>
#include <vector>
#include <QString>
#include <QTextStream>
#include <QDomDocument> // for xml

#include "dwData.h"
#include "dwRclList.h"

class dwDataSet
{
public:
    dwDataSet();
    void fromCsv(const QString & resources);
    void fromTps(const QString & resources);
    void fromXml(const QDomElement & inElement);
    void fromOneDir(const QString & inDirName);
    void fromDirStr(const QDir & inDir);
    void fromCvMatDouble(const cv::Mat & inMat);
//    void fromCvMat2Cov(const cv::Mat & inMat, QString &inId, QString &preId, QString &preHead);
    void fromRclList(const dwRclList & inList);
    QString toCsv() const;
    QString toXml() const;
    QString toTable() const;
    QString toHtmlTable() const;
    QString toHtmlTableTranspose() const;
    QString toHtmlMaxValue() const;
    QString indexVector2html(std::vector< unsigned > inList, QString inTxt) const;
    cv::Mat toCvMatFloat() const;
    cv::Mat toCvMatDouble() const;
    dwRclList toRclList() const;
    cv::Mat id2CvMat() const;
    dwData at(unsigned i) const;
    double valueXY(unsigned i, unsigned j) const;
    double atBottomRight() const;
    void push_back(dwData & inData);
    void setHeader(const std::vector< QString > &inHeader);
    void setHeaderNames();
    void setHeaderCoord();
    void setHeaderPrefix(QString & prefix);
    std::vector< QString > getHeader() const;
    std::vector< QString > getRowNames() const;
    void rename(QStringList renameBefore, QStringList renameAfter);
    void setId(QString & inId);
    void setDataId(const std::vector< QString > &inVec);
//    void setIdName();
    void setIdCoord();
    void setIdPrefix(QString & prefix);
    QString getId() const;
    void setClasName(const std::vector< QString > &inVec);
    unsigned rows() const;
    unsigned size() const;
    unsigned columns() const;
    dwData CvaScores(dwData & inData);
    std::vector< unsigned > rowMaxIndex() const;
    std::vector< unsigned > rowMinIndex() const;
    int pcaDim();
    void pca(int idSize);
    QString getDwXml(int idSize, int dim = 0);
    void lda();
    std::vector< int > extractClas() const;
    void extractClasId();
    void extractClasIdTrunc(int position);
    dwDataSet mean() const;
    double Mahalanobis(dwData inVec, dwData inMean) const;
    dwData eDistance(const dwData &inData) const;
    dwData removeRow(unsigned rowNo);
    std::vector< QString > outliersMD(void);

private:
    QString id;
//    unsigned dataSize; // horizontal size detaSet[0].size();
    std::vector< QString > header;
    std::vector< dwData > dataSet;
    std::vector< QString > clasName;
    std::vector< int > clasVec;

    void readHeader(QTextStream & inStream);
    std::vector< QString > strListHtmlEscaped(std::vector< QString > &inList);
};

#endif // DWDATASET_H
