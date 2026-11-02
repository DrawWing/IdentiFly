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
    std::vector< Coord > readCoordList();
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