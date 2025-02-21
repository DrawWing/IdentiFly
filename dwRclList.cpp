#include <QFile>
#include <QTextStream>

#include "dwRclList.h"
#include "dwImg.h"
#include "winginfo.h"

dwRclList::dwRclList(void)
{
	id = "";
	minRcListSize = 0;
	maxRcListSize = 0;
}

dwRCoordList dwRclList::at(unsigned i)
{
    return rclList.at(i);
}

void dwRclList::push_back(const dwRCoordList & rcl)
{
	int size = rcl.size();
	if(size == 0)
		return;
	rclList.push_back(rcl);

	if(minRcListSize == 0){
		minRcListSize = size;
		maxRcListSize = size;
		return;
	}
	if(size < minRcListSize)
		minRcListSize = size;
	if(size > maxRcListSize)
		maxRcListSize = size;
}

//nie czyta nazwy pliku i komentarzy
//coordinates are in cartesian coordinates; convert here or elswere
void dwRclList::fromTps(const QString & resources)
{
    QFile in_file(resources);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&in_file);
    id = resources;
    while (!in.atEnd())
    {
        QString ln = in.readLine();
        //ln = ln.toUpper();
        if( (ln.startsWith("LM=")) || (ln.startsWith("lm=")) )
        {
            ln.remove(0,3); //first 3 characters
            bool ok;
            int lm = ln.toInt(&ok);
            if(ok)
            {
                std::vector< realCoord > theVec;
                for(int i = 0; i < lm; i++)
                {
                    double x;
                    double y;
                    in >> x >> y; //brak kontroli czy cokolwiek czyta
                    realCoord pxl(x,y);
                    theVec.push_back(pxl);
                }
                dwRCoordList rcl(theVec);

                QString next_ln = in.readLine();
                next_ln = in.readLine();
                //next_ln = next_ln.toUpper();
                if( (next_ln.startsWith("COMMENT=")) || (next_ln.startsWith("comment=")) )
                    next_ln = in.readLine(); //skip comment

                if( (next_ln.startsWith("IMAGE=")) || (next_ln.startsWith("image=")) ){
                    next_ln.remove(0,6); //first 6 characters
                    rcl.setId(next_ln);
                }else{
                    rcl.setId("");
                }
                rclList.push_back(rcl);
            }
        }
    }
    std::vector< dwRCoordList >::const_iterator rclIter;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        int size = rclIter->size();
        if(size > maxRcListSize)
            maxRcListSize = size;
    }
    minRcListSize = maxRcListSize;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        int size = rclIter->size();
        if(size < minRcListSize)
            minRcListSize = size;
    }
}

QString dwRclList::readTps(const QString & resources)
{
    QString error;

    QFile in_file(resources);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return error;

    int lineCount = 0;
    QTextStream in(&in_file);
    id = resources;

    dwRCoordList rcl;
    int lm = 0;
    QString image = "";
    QString id = "";

    while (!in.atEnd())
    {
        QString ln = in.readLine();
        ln = ln.simplified();
        ++lineCount;
        if( (ln.startsWith("LM=")) || (ln.startsWith("lm=")) )
        {
            if(rcl.size() > 0) // not first configuration
            {
                if(lm != (int) rcl.size())
                    error += QString("No match between LM and landmarks number before line %1.\n").arg(lineCount);
                if(!image.isEmpty())
                    rcl.setId(image);
                else if(!id.isEmpty())
                    rcl.setId(id);
                rclList.push_back(rcl);

                rcl.clear();
                lm = 0;
                image.clear();
                id.clear();
            }
            ln.remove(0,3); //first 3 characters
            bool ok;
            lm = ln.toInt(&ok);
            if(!ok)
                error += QString("Incorrect LM argument in line %1.\n").arg(lineCount);
        }
        else if( ln[0].isDigit() ) // coordinates
        {
            if(ln.contains(","))
                ln.replace(",",".");

            QStringList coordList = ln.split(" ", Qt::SkipEmptyParts, Qt::CaseInsensitive);
            if(coordList.size() > 2)
                error += QString("Too many coordinates in line %1.\n").arg(lineCount);

            bool ok;
            double x = coordList[0].toDouble(&ok);
            if(!ok)
                error += QString("Error reading coordinates in line %1.\n").arg(lineCount);

            double y = coordList[1].toDouble(&ok);
            if(!ok)
                error += QString("Error reading coordinates in line %1.\n").arg(lineCount);

            realCoord pxl(x,y);
            rcl.push_back(pxl);
        }
        else if( (ln.startsWith("COMMENT=")) || (ln.startsWith("comment=")) )
            continue;
        else if( (ln.startsWith("IMAGE=")) || (ln.startsWith("image=")) )
        {
            ln.remove(0,6); //first 6 characters
            image = ln;
        }
        else if( (ln.startsWith("ID=")) || (ln.startsWith("id=")) )
        {
            ln.remove(0,3); //first 3 characters
            id = ln;
        }
    }
    if(lm != (int) rcl.size())
        error += QString("No match between LM and landmarks number before line %1.\n").arg(lineCount);
    if(!image.isEmpty())
        rcl.setId(image);
    else if(!id.isEmpty())
        rcl.setId(id);
    rclList.push_back(rcl);

    std::vector< dwRCoordList >::const_iterator rclIter;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        int size = rclIter->size();
        if(size > maxRcListSize)
            maxRcListSize = size;
    }
    minRcListSize = maxRcListSize;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        int size = rclIter->size();
        if(size < minRcListSize)
            minRcListSize = size;
    }

    return error;
}

void dwRclList::fromCsv(const QString & resources)
{
    QFile in_file(resources);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&in_file);
    if(!in.atEnd())
        in.readLine();

    id = resources;
    while (!in.atEnd())
    {
        QString ln = in.readLine();
        dwRCoordList rcl;
        rcl.fromCsv(ln,',');
        rclList.push_back(rcl);
    }
    std::vector< dwRCoordList >::const_iterator rclIter;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        int size = rclIter->size();
        if(size > maxRcListSize)
            maxRcListSize = size;
    }
    minRcListSize = maxRcListSize;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        int size = rclIter->size();
        if(size < minRcListSize)
            minRcListSize = size;
    }
}

void dwRclList::fromVector(const std::vector< dwRCoordList > &inVec)
{
    rclList = inVec;
    minRcListSize = rclList[0].size();
    maxRcListSize = minRcListSize;
}

QString dwRclList::toTps(void) const
{
	QString output;
	std::vector< dwRCoordList >::const_iterator rclIter;
	for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
		output += rclIter->toTps();
	}
	return output;
}

QString dwRclList::toXml(void) const
{
	QString output;
	std::vector< dwRCoordList >::const_iterator rclIter;
	for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
		output += rclIter->toXml();
	}
	return output;
}

QString dwRclList::toCsv(void) const
{
    QString output = "id";
    output += ";";
    for (int i=0; i < maxRcListSize; ++i)
    {
        QString valString = QString::number(i+1);
        output += "x"+valString+";";
        output += "y"+valString+";";
    }
    output += "\n";

    std::vector< dwRCoordList >::const_iterator rclIter;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        output += rclIter->toCsv();
    }
    return output;
}

//dwDataSet dwRclList::toDataSet(void) const
//{
//    dwDataSet outData;

//    outData.setId(id);

//    std::vector< QString > header;
//    for (int i=0; i < maxRcListSize; ++i)
//    {
//        QString valString = QString::number(i+1);
//        QString output = "x"+valString;
//        header.push_back(output);
//        output = "y"+valString;
//        header.push_back(output);
//    }
//    outData.setHeader(header);

//    std::vector< dwRCoordList >::const_iterator rclIter;
//    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
//        dwData outData;
//        outData.fromRCoordList(*rclIter);
//    }
//    return outData;
//}

//Reads name followed by vector of real coordinates.
//Names are in seperate lines. Pairs of X Y coordinates are in seperate lines. 
//consequitive rcl are separated by '<'
void dwRclList::fromFile(const QString & resources)
{
	QFile in_file(resources); 
	if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream in(&in_file);
	id = resources;
	while (!in.atEnd()) {
		dwRCoordList rcl(in);
		rclList.push_back(rcl);
	}
	std::vector< dwRCoordList >::const_iterator rclIter;
	for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
		int size = rclIter->size();
		if(size > maxRcListSize)
			maxRcListSize = size;
	}
	minRcListSize = maxRcListSize;
	for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
		int size = rclIter->size();
		if(size < minRcListSize)
			minRcListSize = size;
	}
}

//2010-10-27 clear list at the beginning
void dwRclList::fromImgDir(const QDir & inDir)
{
    rclList.clear();
    id = inDir.dirName();
    QStringList extList = (QStringList() << "*.dw.png");
    QFileInfoList hivList = inDir.entryInfoList(extList, QDir::Files, QDir::Name );
    for (int j = 0; j < hivList.size(); ++j) {
        QFileInfo localFileInfo = hivList.at(j);
        QString fileName = localFileInfo.absoluteFilePath();
        dwImage img(fileName);
        WingInfo localInfo;
        localInfo.fromImg(img);
        dwRCoordList rcl(localInfo.getLandmarks());
        rcl.setId(localFileInfo.absoluteFilePath());
        rclList.push_back(rcl);
    }
    std::vector< dwRCoordList >::const_iterator rclIter;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        int size = rclIter->size();
        if(size > maxRcListSize)
            maxRcListSize = size;
    }
    minRcListSize = maxRcListSize;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        int size = rclIter->size();
        if(size < minRcListSize)
            minRcListSize = size;
    }
}

// Superimpose landmarks to provided reference.
// Return procrustes distance to reference.
double dwRclList::superimpose(const dwRCoordList & reference)
{
	center();
	scale();
	return superimpose_once(reference); 
}

// Superposition according to Rohlf and Slice 1990.
dwRCoordList dwRclList::superimposeGPA(void)
{
	if(rclList.size() == 0)
		return dwRCoordList();
	const double diff_thd = 0.0000001;
	center();
	scale();
	rotate2first(); // step 2
	dwRCoordList Y = average();	//step 3
    double Sr = rclList.size() * (1 - traceABt(Y,Y));  // step4
	double SrStarStar = Sr; 
	std::vector< double > ro(rclList.size(), 1.0); //step5
	do{ // step 6
		Sr = SrStarStar;
		rotate2reference(Y);
		scale(ro);
		dwRCoordList Ystar = average();	
		std::vector< double > roStar_ro(rclList.size(), 1.0); 
		for(unsigned i = 0; i < rclList.size(); ++i){
            double traceXiYstar = traceABt(rclList[i], Ystar);
            double traceXiXi =  traceABt(rclList[i], rclList[i]);
            double traceYstarYstar = traceABt(Ystar, Ystar);

            Q_ASSERT(traceXiXi != 0);
            Q_ASSERT(traceYstarYstar != 0);
            Q_ASSERT(traceXiYstar > 0);
            Q_ASSERT(traceXiXi * traceYstarYstar > 0);

            roStar_ro[i] = sqrt(traceXiYstar /( traceXiXi * traceYstarYstar ));
		}
		scale(roStar_ro);
		std::vector< double > roStar(rclList.size(), 1.0); 
		for(unsigned i = 0; i < rclList.size(); ++i){
			roStar[i] = roStar_ro[i] * ro[i]; //roStar_ro is now roStar
		}

		dwRCoordList YstarStar = average();
		SrStarStar = rclList.size() * (1 - traceABt(YstarStar,YstarStar));
		Y = YstarStar;
		ro = roStar;
	}while(Sr - SrStarStar > diff_thd);
	return Y;
}

// Step 2 of Rohlf and Slice 1990.
void dwRclList::rotate2first(void)
{
	for(unsigned i = 1; i < rclList.size(); ++i){
		rclList[i].rotate2reference(rclList[0]);
	}
}

void dwRclList::rotate2reference(dwRCoordList & reference)
{
	for(unsigned i = 0; i < rclList.size(); ++i){
		rclList[i].rotate2reference(reference);
	}
}

void dwRclList::scale( std::vector< double > & factor)
{
        if(factor.size() != rclList.size())
                return;
        for(unsigned i = 0; i < rclList.size(); ++i){
                rclList[i].scale(factor[i]);
        }
}

//Flip points verticaly for transformation between bitmap and cartesian coordinates
void dwRclList::flip( const int height)
{
    for(unsigned i = 0; i < rclList.size(); ++i){
        rclList[i].flip(height);
    }
}

double dwRclList::superimpose_once(const dwRCoordList & reference)
{
	double diff = 0.0;
	std::vector< dwRCoordList >::iterator rclIter;
	for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
		diff += rclIter->superimpose(reference);
	}

	return diff;
}

// Calculate average configuration of landmarks. 
dwRCoordList dwRclList::average(void) const
{	
	realCoord nullCoord(0.0, 0.0);
	std::vector< realCoord > mean(minRcListSize, nullCoord);

    for(unsigned j = 0; j < rclList.size(); ++j)
    {
        dwRCoordList theList = rclList[j];
        std::vector< realCoord > rclVec = theList.list();
        for(int i = 0; i < minRcListSize; ++i)
            mean[i]+=rclVec[i];
    }

    for(unsigned i = 0; i < mean.size(); ++i){
        mean[i] *= 1.0/rclList.size();
	}
	dwRCoordList meanList(mean);
	return meanList;
}

void dwRclList::center(void)
{
    for(unsigned i = 0; i < rclList.size(); ++i)
        rclList[i].center();
}

void dwRclList::scale(void)
{
    for(unsigned i = 0; i < rclList.size(); ++i){
        double cs = rclList[i].centroidSize();
        if(cs == 0)
            continue;
        rclList[i].scale(1.0/cs);
    }

}

int dwRclList::getMinSize(void) const
{
	return minRcListSize;
}

bool dwRclList::setValidSize(const int size)
{
	std::vector< dwRCoordList >::iterator rclIter;
	for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
		bool succes = rclIter->setValidSize(size);
		if(!succes) 
			return false;
	}
	return true;
}

void dwRclList::setId(QString inId)
{
    id = inId;
}

QString dwRclList::getId() const
{
    return id;
}

std::vector< QString >  dwRclList::getIdList() const
{
    std::vector< QString > outList;

    for(unsigned i = 0; i < rclList.size(); ++i)
    {
        dwRCoordList theList = rclList[i];
        outList.push_back(theList.getId());
    }
    return outList;
}

//Find errors in landmarks position.
//Error is when destance from mean is greater than threshold thd.
QString dwRclList::find_outliers(const double thd) const
{
	QString outStr;
	dwRCoordList mean = average();
	/////
	//std::vector< std::vector< double > > distances;
	//std::vector< double > sdSumLst(minRcListSize, 0.0);
	//for(int wingNo = 0; wingNo < rclList.size(); ++wingNo){
	//	std::vector< double > distanceList = rclList[wingNo].find_distances(mean);
	//	distances.push_back(distanceList);
	//	for(int i = 0; i < minRcListSize; i++){
	//		sdSumLst[i] += distanceList[i];
	//	}
	//}
	//std::vector< double > sdMeanLst(minRcListSize, 0.0);
	//for(int i = 0; i < minRcListSize; i++){
	//	sdMeanLst[i] = sdSumLst[i]/rclList.size();
	//}
	//std::vector< double > sdLst(minRcListSize, 0.0);
	//for(int wingNo = 0; wingNo < rclList.size(); ++wingNo){
	//	std::vector< double > distanceList = distances[wingNo];
	//	for(int i = 0; i < minRcListSize; i++){
	//		double diff = sdMeanLst[i] - distanceList[i];
	//		sdLst[i] += diff * diff;
	//	}
	//}
	//for(int i = 0; i < minRcListSize; i++){
	//	sdLst[i] += sdLst[i]/rclList.size();
	//}
	/////
	std::vector< double > maxValList(minRcListSize, 0.0);
	std::vector< QString > maxStrList(minRcListSize, "");

	std::vector< dwRCoordList >::const_iterator rclIter;
	int cnt = 0; //do usunięcia
	for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++, cnt++){
		std::vector< double > nextList = rclIter->find_distances(mean);
		for(int i = 0; i < minRcListSize; i++){
			if(nextList[i] > maxValList[i]){
				maxValList[i] = nextList[i];
				QString maxStr = QString("wing: %1, lanmark %2, distance %3").arg(rclIter->getId()).arg(i).arg(nextList[i]);
				maxStrList[i] = maxStr;
			}
		}
	}
	for(int i = 0; i < minRcListSize; i++){
		if(maxValList[i] > thd){
			QString maxStr = maxStrList[i];
			outStr = outStr + maxStr + "\n";
		}
	}
	return outStr;
}

//Find errors in landmarks position. 
//Error is when destance from mean is greater than threshold thd.
//Threshold is different for different points.
QString dwRclList::find_outliers(void) const
{
    const double thd[]={
        0.013, //0
        0.0125, //1
        0.015, //2
        0.0125, //3
        0.014, //4
        0.013, //5
        0.012, //6
        0.013, //7
        0.014, //8
        0.014, //9
        0.012, //10
        0.012, //11
        0.012, //12
        0.012, //13
        0.013, //14
        0.012, //15
        0.0125, //16
        0.013, //17
        0.013, //18
    };

    QString outStr;
    dwRCoordList mean = average();
    std::vector< double > maxValList(minRcListSize, 0.0);
    std::vector< QString > maxStrList(minRcListSize, "");

    std::vector< dwRCoordList >::const_iterator rclIter;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        std::vector< double > nextList = rclIter->find_distances(mean);
        for(int i = 0; i < minRcListSize; i++){
            if(nextList[i] > maxValList[i]){
                maxValList[i] = nextList[i];
                //: translation optional
                QString maxStr = QString("wing: %1, landmark %2, distance %3").arg(rclIter->getId()).arg(i).arg(nextList[i]);
                maxStrList[i] = maxStr;
            }
        }
    }
    for(int i = 0; i < minRcListSize; i++){
        if(maxValList[i] > thd[i]){
            QString maxStr = maxStrList[i];
            outStr = outStr + maxStr + "\n";
        }
    }
    return outStr;
}

std::vector< QString > dwRclList::oneOutlier(unsigned index)
{
    index = index - 1; // indexing from 0

    dwRCoordList mean = superimposeGPA();
    std::vector< double > maxValList;
    std::vector< QString > maxStrList;

    for(unsigned i = 0; i < rclList.size(); ++i)
    {
        dwRCoordList theRcl = rclList[i];
        double maxDist = theRcl.find_distance(mean, index);
        maxValList.push_back(maxDist);
        maxStrList.push_back(theRcl.getId());
    }
//    qSort;
    //nieefektywne sortowanie
    std::vector< double > sortValList;
    std::vector< QString > sortStrList;
    while( maxValList.size() ){
        std::vector< double >::iterator valIter = maxValList.begin();
        std::vector< QString >::iterator strIter = maxStrList.begin();

        double maxVal = *(valIter);
        std::vector< double >::iterator maxValIter = valIter;
        std::vector< QString >::iterator maxStrIter = strIter;
        ++valIter;
        ++strIter;

        for(; valIter != maxValList.end(); ++valIter, ++strIter){
            double theVal = *(valIter);
            if( theVal > maxVal){
                maxVal = theVal;
                maxValIter = valIter;
                maxStrIter = strIter;
            }
        }
        sortValList.push_back(maxVal);
        QString maxStr = *(maxStrIter);
        sortStrList.push_back(maxStr);
        maxValList.erase(maxValIter);
        maxStrList.erase(maxStrIter);
    }
    return sortStrList;
}

std::vector< QString > dwRclList::outliers(void)
{
    dwRCoordList mean = superimposeGPA();
    std::vector< double > maxValList;
    std::vector< QString > maxStrList;

    for(unsigned i = 0; i < rclList.size(); ++i)
    {
        dwRCoordList theRcl = rclList[i];
        std::vector< double > nextList = theRcl.find_distances(mean);
        double maxDist = 0.0;
        for(int j = 0; j < minRcListSize; ++j)
        {
            if(nextList[j] > maxDist)
            {
                maxDist = nextList[j];
            }
        }
        maxValList.push_back(maxDist);
        maxStrList.push_back(theRcl.getId());
    }
//    qSort;
    //nieefektywne sortowanie
    std::vector< double > sortValList;
    std::vector< QString > sortStrList;
    while( maxValList.size() ){
        std::vector< double >::iterator valIter = maxValList.begin();
        std::vector< QString >::iterator strIter = maxStrList.begin();

        double maxVal = *(valIter);
        std::vector< double >::iterator maxValIter = valIter;
        std::vector< QString >::iterator maxStrIter = strIter;
        ++valIter;
        ++strIter;

        for(; valIter != maxValList.end(); ++valIter, ++strIter){
            double theVal = *(valIter);
            if( theVal > maxVal){
                maxVal = theVal;
                maxValIter = valIter;
                maxStrIter = strIter;
            }
        }
        sortValList.push_back(maxVal);
        QString maxStr = *(maxStrIter);
        sortStrList.push_back(maxStr);
        maxValList.erase(maxValIter);
        maxStrList.erase(maxStrIter);
    }
    return sortStrList;
}

bool dwRclList::isSizeEqual(void) const
{
    if(minRcListSize == maxRcListSize)
        return true;
    else
        return false;
}

QString dwRclList::sizeOutliers(void) const
{
    QString outString;
    unsigned theSize = rclList[0].size();
    for(unsigned i = 1; i < rclList.size(); ++i){
        if(rclList[i].size() == theSize)
            continue;
        outString += rclList[i].getId();
        outString += "\n";
    }
    return outString;
}

QString dwRclList::findZeroCS(void) const
{
    QString outString;
    int count = 0;
    for(unsigned i = 1; i < rclList.size(); ++i){
        double cs = rclList[i].centroidSize();
        if(cs == 0)
        {
            if(count > 20) //truncate too long output
            {
                outString += "...\n";
                return outString;
            }
            outString += rclList[i].getId();
            outString += "\n";
            ++count;
        }
    }
    return outString;
}

//Calculate average centroid size of the list.
double dwRclList::meanCS(void) const
{
    double totalCS = 0;
    int count = 0;
    std::vector< dwRCoordList >::const_iterator rclIter;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        dwRCoordList rcl = *rclIter; //konieczne bo centroidSize nie jest const.
        totalCS += rcl.centroidSize();
        ++count;
    }
    if(count)
        return totalCS/count;
    else
        return 0.0;
}

//Size of the rclList.
unsigned dwRclList::size(void) const
{
	return rclList.size();
}

std::vector< double > dwRclList::find_distances(const dwRCoordList & constLst) const
{
	std::vector< double > outLst(rclList.size(), -1.0);
	for(unsigned i = 0; i < rclList.size(); i++){
		dwRCoordList theLst = constLst;
		outLst[i] = theLst.superimpose(rclList[i]);
	}
	return outLst;
}

std::vector< dwRCoordList > dwRclList::list(void) const
{
	return rclList;
}

std::vector< realCoord > dwRclList::points() const
{
    std::vector< realCoord > outVec;
    for(unsigned i = 0; i < rclList.size(); ++i){
        dwRCoordList theLst = rclList[i];
        std::vector< realCoord > theVec = theLst.list();
        outVec.insert(outVec.end(), theVec.begin(), theVec.end());
    }

    return outVec;
}

dwRclList::~dwRclList(void)
{
	rclList.clear();
}

//Convert data from directory tree to xml.
//zniezalezne od tej klasy przeniesc???
QString dir2xml(const QDir & inDir) 
{
    // QString dirName = inDir.dirName();//debug
	QString out = QString( "<directory>\n" );
	out += QString( "<directoryName>%1</directoryName>\n" ).arg( inDir.dirName() );

	dwRclList localRcll;
	localRcll.fromImgDir(inDir);
	out += localRcll.toXml();

	QFileInfoList dirList = inDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
	for (int j = 0; j < dirList.size(); ++j) {
		QFileInfo localFileInfo = dirList.at(j);
        // QString localDirName = localFileInfo.fileName();//debug
		QDir localDir(localFileInfo.absoluteFilePath());
		out += dir2xml(localDir);
	}
	out += "</directory>\n";
	return out;
}
//Convert data from directory tree to tps.
//zniezalezne od tej klasy przeniesc???
QString dir2tps(const QDir & inDir) 
{
    // QString dirName = inDir.dirName();//debug
	QString out;

	dwRclList localRcll;
	localRcll.fromImgDir(inDir);
	out += localRcll.toTps();

	QFileInfoList dirList = inDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
	for (int j = 0; j < dirList.size(); ++j) {
		QFileInfo localFileInfo = dirList.at(j);
        // QString localDirName = localFileInfo.fileName();//debug
		QDir localDir(localFileInfo.absoluteFilePath());
		out += dir2tps(localDir);
	}
	return out;
}

//update one of the rcl when image changed
bool dwRclList::update(QString path)
{
    for(unsigned i = 0; i < rclList.size(); ++i){
        dwRCoordList rcl = rclList[i];
        if(rcl.getId() == path){
            dwImage img(path);
            if(img.isNull()) return false;
            WingInfo localInfo;
            localInfo.fromImg(img);
            rcl.setList(localInfo.getLandmarks());
            return true;
        }
    }
    return false;
}

//remove one of the rcl associated with path
bool dwRclList::erase(QString path)
{
    std::vector< dwRCoordList >::iterator rclIter;
    for(rclIter = rclList.begin(); rclIter != rclList.end(); rclIter++){
        //dwRCoordList rcl = *rclIter;
        if(rclIter->getId() == path){
            rclList.erase(rclIter);
            return true;
        }
    }
    return false;
}

//remove all
void dwRclList::clear()
{
    rclList.clear();
}

// output coordinates as vector graphics in svg format
QString dwRclList::toSVG(const QString & fileName, dwImage & image) const
{
    QString outString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> \n");
    outString += QString("<svg \n");
    outString += QString("xmlns:xlink=\"http://www.w3.org/1999/xlink\" \n");
    outString += QString("xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\" \n");
    outString += QString("xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\" \n");
    outString += QString("width=\"%1\"  \n").arg(image.width());
    outString += QString("height=\"%2\" \n").arg(image.height()); //%2 to prawdopodobnie pomylka
    outString += QString("> \n");

    outString += QString("<g \n");
    outString += QString("inkscape:groupmode=\"layer\" \n");
    outString += QString("id=\"layer2\" \n");
    outString += QString("inkscape:label=\"bitmap\" \n");
    outString += QString("sodipodi:insensitive=\"true\" \n");
    outString += QString("> \n");

    outString += QString("<image \n");
    outString += QString("xlink:href=\"%1\" \n").arg(fileName);
    outString += QString("width=\"%1\"  \n").arg(image.width());
    outString += QString("height=\"%2\" \n").arg(image.height());
    outString += QString("/> \n");
    outString += QString("</g> \n");

    outString += QString("<g \n");
    outString += QString("inkscape:groupmode=\"layer\" \n");
    outString += QString("id=\"layer1\" \n");
    outString += QString("inkscape:label=\"dots\" \n");
    outString += QString("> \n");

    for(unsigned i = 0; i < rclList.size(); ++i){
        outString += rclList[i].listToSVG();
    }

    outString += QString("</g> \n");

    outString += "</svg> \n";
    return outString;
}

// output coordinates as pair of number in a line
QString dwRclList::toTxt() const
{
    QString outString;

    for(unsigned i = 0; i < rclList.size(); ++i){
        outString += rclList[i].listToTxt();
    }
    return outString;
}

void dwRclList::setCoord(unsigned rclNo, unsigned coordNo, realCoord inCoord)
{
    rclList[rclNo].setCoord(coordNo, inCoord);
}
