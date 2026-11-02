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

#ifndef DWDATA_H
#define DWDATA_H

#include <opencv2/core/core.hpp>
#include <vector>
#include <QString>
#include <QTextStream>
#include <QDomDocument> // for xml

#include "dwRCoordList.h"

class dwData
{
public:
    dwData();
    void fromCsv(QTextStream &inStream);
    void fromDwPng(QString &fileName);
    void clear();
    void fromXml(const QDomElement & inElement);
    void fromRCoordList(const dwRCoordList & inList);
    cv::Mat toCvMatDouble() const;
    double at(unsigned i) const;
    void setAt(unsigned i, double inVal);

    dwRCoordList toRCoordList() const;
    QString toCsv() const;
    QString toTPS() const;
    QString toXml() const;
    QString toTable(int idSize) const;
    QString toHtmlTable() const;
    unsigned size() const;
    void push_back(double inVal);
    std::vector< double > getData() const;
    void setId(const QString & inString);
    QString getId() const;
    void setClas(QString & inString);
    QString getClas() const;
    unsigned maxIndex(); //retun index of maximum value
    unsigned minIndex(); //retun index of minimum value
    void add(const dwData & inData);
    void subtract(const dwData & inData);
    void subtract(const double inVal);
    void divide(double factor);
    void square();
    double sum() const;
    double mean() const;
    double sd() const;
    double eDistance(const dwData & inMean) const; //euclidean distance

private:
    QString id;
    QString clasInfo; //info for classification
    std::vector< double > data;

    QString removeQuote(const QString &inTxt);
};

#endif // DWDATA_H