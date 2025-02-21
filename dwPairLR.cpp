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

    std::vector< std::pair<double, QString> > stringValList;
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
        std::pair< double, QString > stringVal = {dist, theStr};

        stringValList.push_back(stringVal);
    }

    // sort from large to small MD
    sort(stringValList.begin(), stringValList.end(), std::greater<>());

    for(unsigned i = 0; i < stringValList.size(); ++i)
    {
        outList.push_back(stringValList.at(i).second);
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

//void dwFileRename::moveUnPairedLR(QDir inDir)
//{
//    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Files, QDir::Name );
//    if(localFileInfoList.size() == 0) return;

//    if(localFileInfoList.size() == 1)
//    {
//        return;
//    }

//    QFileInfo currFileInfo = localFileInfoList.at(0);
//    QString currStr = currFileInfo.baseName();
//    QString currSide = currStr.right(1);
//    currStr.chop(2); //remove last character r or l

//    QFileInfo nextFileInfo = localFileInfoList.at(1);
//    QString nextStr = nextFileInfo.baseName();
//    QString nextSide = nextStr.right(1);
//    nextStr.chop(2);

//    //        QString sideStr = fileName.right(1);
//    for (int i = 1; i < localFileInfoList.size(); ) {
//        if(currStr == nextStr) //pair was found
//        {
//            if(currSide == nextSide)
//            {
//                move2trash(currFileInfo);
//                move2trash(nextFileInfo);
//            }

//            ++i;
//            if(i == localFileInfoList.size()) return; //last pair was found
//            currFileInfo = localFileInfoList.at(i);
//            ++i;
//            if(i == localFileInfoList.size()) //last file is unpaired
//            {
//                move2trash(currFileInfo);
//                return;
//            }
//            nextFileInfo = localFileInfoList.at(i);
//        }else{ // pair does not match
//            move2trash(currFileInfo);
//            currFileInfo = nextFileInfo;

//            ++i;
//            if(i == localFileInfoList.size()) return; // last file was unpaired
//            nextFileInfo = localFileInfoList.at(i);
//        }
//        currStr = currFileInfo.baseName();
//        currSide = currStr.right(1);
//        currStr.chop(2);
//        nextStr = nextFileInfo.baseName();
//        nextSide = nextStr.right(1);
//        nextStr.chop(2);
//    }
//}
