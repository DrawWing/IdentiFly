#ifndef TPS_H
#define TPS_H

#include <vector>
#include <QString>
#include <QTextStream>

#include "coord.h"

class TPS
{
public:
	TPS(void);
    TPS(QStringList &inTxtList);
    std::vector< Coord > getLandmarks();
    QString getComment() const;
    QString getError() const;
    ~TPS(void);
private:
    void initKeywords();
    bool isKeyword(QString & inTxt);
    //    void procKeyword(QString & inTxt);
    Coord readCoord(QString & ln, bool *ok);
    std::vector< Coord > readCoordList(int size);
    std::vector< Coord > readChain(QTextStream & in, int size);
    std::vector< std::vector< Coord > > readCurves(int inSize);
    std::vector< std::vector< Coord > > readOutlines(int inSize);

    QStringList keywords;
	QString image;
	QString id;
	QString comment;
	double scale;
    std::vector< Coord > lmLst;
    std::vector< std::vector< Coord > > curvesLst;
    std::vector< std::vector< Coord > > outlinesLst;
    QString errorStr;
    QString warningStr;
    int lineNo;
    QStringList txtList;
};
#endif // !defined(TPS_H)
