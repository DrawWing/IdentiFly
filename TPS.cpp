#include <QFile>
#include <QStringList>

#include "TPS.h"

TPS::TPS(void)
{
}

//nie czyta outlines
TPS::TPS(QStringList & inTxtList)
{
    txtList = inTxtList;

    initKeywords();
    image = "";
    id = "";
    comment = "";
    scale = 0.0; //default scale

//	QFile in_file(resources);
//	if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
//		return;

//	QTextStream in(&in_file);
//	while (!in.atEnd()) {
    for(lineNo = 0; lineNo < txtList.size(); ++lineNo)
    {
        QString ln = txtList[lineNo];
        if( ln.startsWith("LM=", Qt::CaseInsensitive)){
			ln.remove(0,3); //first 3 characters
			bool ok;
			int lm = ln.toInt(&ok);
			if(ok){
                lmLst = readCoordList(lm);
                if((int)lmLst.size() != lm)
                    warningStr += "Number of landmarks different than declared in LM=";
			}else{
                errorStr+="Incorrect LM value./n";
			}
			continue;
		}
		if( (ln.startsWith("IMAGE=")) || (ln.startsWith("image=")) ){
			ln.remove(0,6); //first 6 characters
			image = ln;
			continue;
		}
		if( (ln.startsWith("ID=")) || (ln.startsWith("id=")) ){
			ln.remove(0,3); //first 3 characters
			id = ln;
			continue;
		}
		if( (ln.startsWith("COMMENT=")) || (ln.startsWith("comment=")) ){
			ln.remove(0,8); //first 8 characters
			comment = ln;
			continue;
		}
		if( (ln.startsWith("SCALE=")) || (ln.startsWith("scale=")) ){
			ln.remove(0,6); //first 6 characters
			bool ok;
			scale = ln.toDouble(&ok);
			if(!ok){
				scale = 0.0;
                errorStr+="Incorrect SCALE value./n";
			}
			continue;
		}
        if( (ln.startsWith("CURVES=")) || (ln.startsWith("curves=")) ){
            ln.remove(0,7); //first 7 characters
            bool ok;
            int inNr = ln.toInt(&ok);
            if(ok){
                curvesLst = readCurves(inNr);
            }else{
                errorStr+="Incorrect CURVES value./n";
            }
            continue;
        }
        if( (ln.startsWith("OUTLINES=")) || (ln.startsWith("outlines=")) ){
            ln.remove(0,9); //first 7 characters
            bool ok;
            int inNr = ln.toInt(&ok);
            if(ok){
                outlinesLst = readOutlines(inNr);
            }else{
                errorStr+="Incorrect OUTLINES value./n";
            }
            continue;
        }
    }
}

std::vector< Coord > TPS::getLandmarks()
{
    return lmLst;
}

void TPS::initKeywords()
{
    keywords.push_back("LM=");
    keywords.push_back("LM3=");
    keywords.push_back("CURVES=");
    keywords.push_back("POINTS=");
    keywords.push_back("OUTLINES=");
    keywords.push_back("CHAIN=");
    keywords.push_back("RADII=");
    keywords.push_back("RADIIXY=");
    keywords.push_back("IMAGE=");
    keywords.push_back("ID=");
    keywords.push_back("COMMENT=");
    keywords.push_back("SCALE=");
    keywords.push_back("VARIABLES=");
}

bool TPS::isKeyword(QString & inTxt)
{
    for(int i = 0; i < keywords.size(); ++i)
    {
        QString theWord = keywords.at(i);
        if( inTxt.startsWith(theWord, Qt::CaseInsensitive) )
            return true;
    }
    return false;
}

//void TPS::procKeyword(QString & inTxt)
//{
//    if( inTxt.startsWith("LM=", Qt::CaseInsensitive) ){
//        inTxt.remove(0,3); //first 3 characters
//        bool ok;
//        int inNr = inTxt.toInt(&ok);
////        if(ok){
////            lmLst = readCoordList(in, inNr);
////        }else{
////            errorStr+="Incorrect LM value. Cannot read landmarks./n";
////        }
//    }
//}

Coord TPS::readCoord(QString & ln, bool * ok)
{
    Coord outPxl;

    if(ln.isEmpty())
    {
        *ok = false;
        return outPxl;
    }
    QStringList lst = ln.split(" ");  // tabulation
    if(lst.size() < 2){
        errorStr+= ln +" is not a pair of coordinates./n";
        *ok = false;
        return outPxl;
    }
    bool okx, oky;
    double x = lst.at(0).toDouble(&okx);
    double y = lst.at(1).toDouble(&oky);
    if( !okx || !oky){
        errorStr+= ln +" is not a pair of coordinates./n";
        *ok = false;
        return outPxl;
    }
    outPxl.setXY(x,y);
    *ok = true;
    return outPxl;
}

std::vector< Coord > TPS::readCoordList(int size)
{
    std::vector< Coord > theList;

    ++lineNo;

    for(; lineNo < txtList.size(); ++lineNo)
    {
        QString ln = txtList[lineNo];
        if(isKeyword(ln))
            return theList;

        if(ln.isEmpty())
			continue;

        QStringList lst = ln.split(" ");  // tabulation as separator
        if(lst.size() < 2){
			errorStr+= ln +" is not a pair of coordinates./n";
			return theList;
		}
		bool okx, oky;
		double x = lst.at(0).toDouble(&okx);
		double y = lst.at(1).toDouble(&oky);
		if( !okx || !oky){
			errorStr+= ln +" is not a pair of coordinates./n";
			return theList;
		}
        Coord pxl(x,y);
		theList.push_back(pxl);
	}
	return theList;
}

std::vector< Coord >
TPS::readChain(QTextStream & in, int size)
{
    std::vector< Coord > theList;

    QString ln = in.readLine();
    bool ok;
    Coord startPxl = readCoord(ln, &ok);
    if(ok)
        theList.push_back(startPxl);
    else
    {
        errorStr+= ln +"No first chain pixel./n";
        return theList;
    }
    int count = 1;
    int x = startPxl.dx();
    int y = startPxl.dy();
    while(count < size){
        if(in.atEnd()){
            errorStr+="Unexpected end of file./n";
            return theList;
        }
        ln = in.readLine();
        if(ln.isEmpty())
            continue;
        for(int i = 0; i < ln.size(); ++i)
        {
            QChar theChar = ln.at(i);
            QString theStr(theChar);
            int inNr = theStr.toInt(&ok);
            if(ok)
            {
                if(inNr == 0){
                    ++x; // right
                    Coord pxl(x,y);
                    theList.push_back(pxl);
                    ++count;
                }else if(inNr == 1){
                    ++x; // right
                    --y; // cartesian up
                    Coord pxl(x,y);
                    theList.push_back(pxl);
                    ++count;
                }else if(inNr == 2){
                    --y; // cartesian up
                    Coord pxl(x,y);
                    theList.push_back(pxl);
                    ++count;
                }else if(inNr == 3){
                    --x; // left
                    --y; // cartesian up
                    Coord pxl(x,y);
                    theList.push_back(pxl);
                    ++count;
                }else if(inNr == 4){
                    --x; // left
                    Coord pxl(x,y);
                    theList.push_back(pxl);
                    ++count;
                }else if(inNr == 5){
                    --x; // left
                    ++y; // cartesian down
                    Coord pxl(x,y);
                    theList.push_back(pxl);
                    ++count;
                }else if(inNr == 6){
                    ++y; // cartesian down
                    Coord pxl(x,y);
                    theList.push_back(pxl);
                    ++count;
                }else if(inNr == 7){
                    ++x; // right
                    ++y; // cartesian down
                    Coord pxl(x,y);
                    theList.push_back(pxl);
                    ++count;
                }
            }

        }
    }
    return theList;
}

std::vector< std::vector< Coord > > TPS::readCurves(int inSize)
{
    std::vector< std::vector< Coord > > theClList;
    for(int i = 0; i < inSize; ++i){
        std::vector< Coord > lst;
        ++lineNo;
        if(lineNo > txtList.size())
        {
            errorStr+="Unexpected end of file./n";
            return theClList;
        }
        QString ln = txtList[lineNo];
        if( (ln.startsWith("POINTS=")) || (ln.startsWith("points=")) ){
			ln.remove(0,7); //first 7 characters
			bool ok;
            int inNr = ln.toInt(&ok);
			if(ok){
                lst = readCoordList(inNr);
                theClList.push_back(lst);
            }else{
				errorStr+="Incorrect POINTS value. Cannot read points./n";
				return theClList;
			}
			continue;
		}else{
			errorStr+="Incorrect number of curves.";
			return theClList;
		}
	}
	return theClList;
}

std::vector< std::vector< Coord > > TPS::readOutlines(int inSize)
{
    std::vector< std::vector< Coord > > theClList;
    for(int i = 0; i < inSize; ++i){
        std::vector< Coord > lst;
        ++lineNo;
        if(lineNo > txtList.size())
        {
            errorStr+="Unexpected end of file./n";
            return theClList;
        }
        QString ln = txtList[lineNo];
        if( (ln.startsWith("POINTS=")) || (ln.startsWith("points=")) ){
            ln.remove(0,7); //first 7 characters
            bool ok;
            int inNr = ln.toInt(&ok);
            if(ok){
                lst = readCoordList(inNr);
                theClList.push_back(lst);
            }else{
                errorStr+="Incorrect POINTS value. Cannot read points./n";
                return theClList;
            }
            continue;
        }else if( (ln.startsWith("CHAIN=")) || (ln.startsWith("chain=")) ){
//            ln.remove(0,6); //first 7 characters
//            bool ok;
//            int inNr = ln.toInt(&ok);
//            if(ok){
////                lst = readChain(inNr);
////                theClList.push_back(lst);
//            }else{
//                errorStr+="Incorrect CHAIN value. Cannot read points./n";
//                return theClList;
//            }
//            continue;
        }else{
            errorStr+="Incorrect number of curves.";
            return theClList;
        }
    }
    return theClList;
}

QString TPS::getComment() const
{
    return comment;
}

QString TPS::getError() const
{
    return errorStr;
}

TPS::~TPS(void)
{
}
