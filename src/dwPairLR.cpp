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

#include "dwPairLR.h"
#include "dwRclList.h"
#include "winginfo.h"

dwPairLR::dwPairLR()
{
}
std::vector< QString > dwPairLR::outliers(dwRclList rcl)
{
    std::vector< QString > outList;

    QStringList inStringList;
    std::vector< dwRCoordList > inVec = rcl.list();
    for(unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theCoords = inVec[i];
        inStringList.push_back(theCoords.getId());
    }

    std::vector< std::pair<double, std::pair<QString, QString> > > pairVec;
    std::vector< dwRCoordList > rclList = rcl.list();
    for(int i = 0; i < inStringList.size(); ++i)
    {
        QString theStr = inStringList[i];
        int RLindex;
        int listIndex;
        QString pairStr = findPairLR(theStr, inStringList, &RLindex, &listIndex);
        if(pairStr.isEmpty())
            continue; // no pair was found

        dwRCoordList currRcl = rclList[i];
        dwRCoordList nextRcl = rclList[listIndex];
        nextRcl.center();
        double cs = nextRcl.centroidSize();
        nextRcl.scale(1.0/cs);

        double dist = currRcl.superimpose(nextRcl);
        pairVec.push_back({dist, {theStr, pairStr}});
    }

    // sort from large to small MD
    sort(pairVec.begin(), pairVec.end(), std::greater<>());

    // // lambda version
    // sort(pairVec.begin(), pairVec.end(),
    //      [](const std::pair<double, std::pair<QString, QString> > &a,
    //         const std::pair<double, std::pair<QString, QString> > &b) {
    //          return a.first > b.first;
    //      });

    for(unsigned i = 0; i < pairVec.size(); ++i)
    {
        const QString first = pairVec.at(i).second.first;
        const QString second = pairVec.at(i).second.second;

        if (std::find(outList.begin(), outList.end(), first) == outList.end())
            outList.push_back(first);
        if (std::find(outList.begin(), outList.end(), second) == outList.end())
            outList.push_back(second);

    }
    return outList;
}
// LRindex pointer returns position of the LR letter
QString dwPairLR::findPairLR(QString inStr, QStringList inStringList,
                             int * LRindex, int * listIndex)
{
    QString outStr = QString(); // null string
    int inSize = inStr.size();

    for(int i = 0; i < inStringList.size(); ++i)
    {
        *listIndex = i;
        QString theStr = inStringList[i];
        int theSize = theStr.size();
        if(theSize != inSize)
            continue;

        *LRindex = -1;
        for (int i = 0; i < inStr.size(); ++i)
        {
            QChar theChar = theStr[i];
            theChar = theChar.toUpper();
            QChar inChar = inStr[i];
            inChar = inChar.toUpper();

            if(theChar == inChar)
                continue;

            if(*LRindex > -1)
            {
                *LRindex = -1;
                break; // more than one difference
            }

            if( !(theChar == 'L' || theChar == 'R') )
                break; // difference but not in LR
            if( !(inChar == 'L' || inChar == 'R') )
                break;
            *LRindex = i;
        }
        if(*LRindex > -1)
            return theStr;
    }
    return outStr; //retunr null string
}

QStringList dwPairLR::unPaird(QDir inDir)
{
    QStringList outList;

    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() < 2)
        return outList;

    QStringList inStringList;
    for (int i = 0; i < localFileInfoList.size(); ++i)
    {
        QFileInfo theFileInfo = localFileInfoList.at(i);
        QString theStr = theFileInfo.baseName();
        inStringList.push_back(theStr);
    }
    for (int i = 0; i < inStringList.size(); ++i)
    {
        QString theStr = inStringList[i];
        int RLindex;
        int listIndex;
        QString pairStr = findPairLR(theStr, inStringList, &RLindex, &listIndex);
        if(pairStr.isNull())
            outList.push_back(theStr); // no pair was found
    }
    return outList;
}