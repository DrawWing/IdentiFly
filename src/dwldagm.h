#ifndef DWLDAGM_H
#define DWLDAGM_H

#include <QDomDocument> // for xml
#include <QImage>

#include "dwRCoordList.h"
#include "dwRclList.h"
#include "dwDataSet.h"

class dwLdaGm
{
public:
    dwLdaGm();
    dwLdaGm(QDomDocument xmlDoc);
    void fromXml(QDomDocument XmlDoc);
    void renameInData();
    QString getError() const;
    QString getPrototype() const;
    unsigned getReferenceSize() const;
    bool isSane();
    bool hasLda();
    QString classify(dwRCoordList inList);
    int classifyCv(dwData inData);
    int classifyCvDist(dwData inData); // probably to be removed
    QStringList classify(dwRclList & inList);
    QString classifyTab(dwRclList & inList);
    dwDataSet cvaMD(dwRclList & inList);
    dwDataSet cvaProb(dwRclList & inList);
    dwDataSet cvaDistProb(dwRclList & inList);
    dwDataSet cvaScores(dwRclList & inList);
    dwDataSet ldaScores(dwRclList & inList);
    QImage cvaDiagram(dwRclList & inList, unsigned cvX = 1, unsigned cvY = 2);
//    void svm();
//    void bayes();
    dwDataSet confusionMat(dwDataSet inData, int idSize);
    dwDataSet confusionMatLoo(dwDataSet inData, int idSize);
    double mahDist(dwData inData, dwData inMean, dwDataSet inCov);
    double cdfProb(double inVal) const;

private:
    dwRCoordList reference;
    dwDataSet cf; //classification functions
    std::vector< double > constants;
    QString error;
    dwDataSet cvMeans;
    dwDataSet rawMeans;
    std::vector< dwDataSet > cvCovVec;
    dwDataSet cvCoe;
    dwDataSet cvDist; // probably to be removed
    dwDataSet cvProb;
    QString prototypeFile;
    std::vector< QString > clasName;
    QStringList renameShort;
    QStringList renameLong;
    double version;

    void extractLdaGmData(QDomElement & ldaGmElement);
    void extractCvaData(QDomElement & cvaElement);
    void extractLdaData(QDomElement & inElement);
    void extractRename(QDomElement & inElement);
    dwDataSet extractMatrix(QDomElement inElement);
};

#endif // DWLDAGM_H
