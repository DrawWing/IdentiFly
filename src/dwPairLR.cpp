#include "dwPairLR.h"
#include "dwRclList.h"
#include "winginfo.h"

dwPairLR::dwPairLR()
{
}

std::vector< QString > dwPairLR::outliers(QDir inDir)
{
    std::vector< QString > outList;

    QStringList inStringList;
    dwRclList rcl;
    rcl.setId(inDir.dirName());
    QStringList extList = (QStringList() << "*.dw.png");
    QFileInfoList hivList = inDir.entryInfoList(extList, QDir::Files, QDir::Name );
    for (int j = 0; j < hivList.size(); ++j) {
        QFileInfo localFileInfo = hivList.at(j);
        QString fileName = localFileInfo.absoluteFilePath();
        //        statusBar()->showMessage(tr("Reading file: %1").arg(fileName));
        dwImage img(fileName);
        WingInfo localInfo;
        localInfo.fromImg(img);
        dwRCoordList coordinates(localInfo.getLandmarks());
        coordinates.setId(localFileInfo.absoluteFilePath());
        rcl.push_back(coordinates);
        inStringList.push_back(localFileInfo.fileName());
    }

    if(inStringList.size() < 2)
        return outList;

    std::vector< std::pair<double, QString> > pairVec;
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
        // !!! crash in line below if number of landmarks differ in pair
        dwRCoordList nextRcl = rclList[listIndex];
        nextRcl.center();
        double cs = nextRcl.centroidSize();
        nextRcl.scale(1.0/cs);

        double dist = currRcl.superimpose(nextRcl);
        std::pair< double, QString > valStr = {dist, theStr};
        pairVec.push_back(valStr);
    }

    // sort from large to small MD
    sort(pairVec.begin(), pairVec.end(), std::greater<>());
    for(unsigned i = 0; i < pairVec.size(); ++i)
    {
        outList.push_back(pairVec.at(i).second);
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
