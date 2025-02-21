#include <QRegularExpression>

#include "dwData.h"
#include "winginfo.h"

dwData::dwData()
{
    id = "";
}

void dwData::clear()
{
    id = "";
    data.clear();
}

void dwData::fromCsv(QTextStream & inStream)
{
    data.clear();

    QString theLine = inStream.readLine();
    QStringList theLineList = theLine.split(";");
    id = removeQuote(theLineList.at(0));
    for (int i = 1; i < theLineList.size(); ++i) // start with second
    {
        QString valueString = removeQuote(theLineList.at(i));
        double value = valueString.toDouble();
        data.push_back(value);
    }
}

void dwData::fromDwPng(QString &fileName)
{
    data.clear();

    dwImage img(fileName);
    if(img.isNull()) return;

    WingInfo info;
    info.fromImg(img);
    std::vector< Coord > landmarks = info.getLandmarks();
    if(landmarks.size() == 0) return;

    for (unsigned i = 0; i < landmarks.size(); ++i)
    {
        Coord point = landmarks[i];
        data.push_back(point.dx());
        data.push_back(point.dy());
    }

    id = fileName;
}

void dwData::fromXml(const QDomElement & inElement)
{
    data.clear();

    id = inElement.attribute("id");
    id = id.simplified();

    QString inLine = inElement.text();
    inLine = inLine.simplified(); //regex s+ is not working as expected
    QStringList inLineList = inLine.split(QRegularExpression("\\s+")); //split at white spaces
    for (int i = 0; i < inLineList.size(); ++i)
    {
        QString valStr = inLineList.at(i);
        double val = valStr.toDouble();
        data.push_back(val);
    }
}

void dwData::fromRCoordList(const dwRCoordList & inList)
{
    data.clear();

    std::vector< realCoord > inVec = inList.list();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        realCoord theCoord = inVec[i];
        data.push_back(theCoord.dx());
        data.push_back(theCoord.dy());
    }
//    double constant = inList.getConstant();
//    if(constant!=0.0)
//        data.push_back(constant);
    id=inList.getId();
}

cv::Mat dwData::toCvMatDouble() const
{
    cv::Mat mtx = cv::Mat::zeros(1, data.size(), CV_64FC1);

    for (unsigned j=0; j<data.size(); ++j)
    {
        mtx.at< double >(0,j) = data[j];
    }

    return mtx;
}

double dwData::at(unsigned i) const
{
    return data.at(i);
}

void dwData::setAt(unsigned i, double inVal)
{
    if( i > data.size()-1)
        return;
    data[i]=inVal;
}

// the size od the data needs to be odd number
dwRCoordList dwData::toRCoordList() const
{
    dwRCoordList outList;

    unsigned i = 0;
    while(i < data.size())
    {
        double x = data[i];
        double y;
        ++i;
        if(i < data.size())
        {
            y = data[i];
            realCoord theCoord(x,y);
            outList.push_back(theCoord);
        }
        ++i;
    }
    outList.setId(id);
    return outList;
}

QString dwData::toCsv() const
{
    QString outTable;
    outTable = id;
    outTable += ";";
    for (unsigned j=0; j<data.size(); ++j)
    {
        outTable += QString::number(data[j], 'g', 6);
        outTable += ";";
    }
    outTable += "\n";
    return outTable;
}

QString dwData::toTPS() const
{
    if (data.size() % 2 != 0 || data.size() == 0)
        return QString();

    unsigned n = data.size()/2;
    QString outString = QString("LM=%1/n").arg(n);

    unsigned i = 0;
    while(i < data.size())
    {
        double x = data[i];
        double y;
        ++i;
        if(i < data.size())
        {
            y = data[i];
            outString += QString("%1 %2/n").arg(x, y);
        }
        ++i;
    }
    outString += QString("IMAGE=%1/n").arg(id);
    return outString;
}

QString dwData::toXml() const
{
    QString outString = "<vector id=\t\""+id+"\"\t>\t";
    for (unsigned j=0; j<data.size(); ++j)
    {
        outString += QString::number(data[j], 'g', 6);
        outString += "\t";
    }
    outString += "</vector>\n";
    return outString;
}

QString dwData::toTable(int idSize) const
{
    QString outTable;
    outTable = id;
    while(outTable.length() < idSize)
        outTable.append(" ");

    outTable += "\t";
    for (unsigned j=0; j<data.size(); ++j)
    {
        outTable += QString::number(data[j], 'f', 6);
        outTable += "\t";
    }
    outTable += "\n";
    return outTable;
}

QString dwData::toHtmlTable() const
{
    QString outTable;
    outTable += "<tr>";

    outTable += "<td align=\"center\">";
    outTable += id;
    outTable += "</td>";
    for (unsigned j=0; j<data.size(); ++j)
    {
        outTable += "<td align=\"center\">";
        outTable += QString::number(data[j], 'g', 8);  // e for all scientific
        outTable += "</td>";
    }
    outTable += "</tr>\n";
    return outTable;
}

unsigned dwData::size() const
{
    return data.size();
}

void dwData::push_back(double inVal)
{
    data.push_back(inVal);
}

std::vector< double > dwData::getData() const
{
    return data;
}

void dwData::setId(const QString &inString)
{
    id = inString;
}

QString dwData::getId() const
{
    return id;
}

void dwData::setClas(QString & inString)
{
    clasInfo = inString;
}

QString dwData::getClas() const
{
    return clasInfo;
}

//retun index of maximum value
unsigned dwData::maxIndex()
{
    if(data.size() == 0)
        return 0;

    unsigned max = 0;
    double maxValue = data[0];
    for (unsigned j=1; j<data.size(); ++j)
    {
        if (data[j] > maxValue)
        {
            maxValue = data[j];
            max = j;
        }
    }
    return max;
}

//retun index of minimum value
unsigned dwData::minIndex()
{
    if(data.size() == 0)
        return 0;

    unsigned min = 0;
    double minValue = data[0];
    for (unsigned j=1; j<data.size(); ++j)
    {
        if (data[j] < minValue)
        {
            minValue = data[j];
            min = j;
        }
    }
    return min;
}

void dwData::add(const dwData &inData)
{
    if(data.size() != inData.size())
        return;
    for (unsigned i=0; i < data.size(); ++i)
        data[i] += inData.at(i);
}

void dwData::subtract(const dwData &inData)
{
    if(data.size() != inData.size())
        return;
    for (unsigned i=0; i < data.size(); ++i)
        data[i] -= inData.at(i);
}

void dwData::subtract(const double inVal)
{
    for (unsigned i=0; i < data.size(); ++i)
        data[i] -= inVal;
}

void dwData::divide(double factor)
{
    for (unsigned i=0; i < data.size(); ++i)
        data[i] /= factor;
}

void dwData::square()
{
    for (unsigned i=0; i < data.size(); ++i)
        data[i] = data[i]*data[i];
}

double dwData::sum() const
{
    double outSum = 0;
    for (unsigned i=0; i < data.size(); ++i)
        outSum += data[i];
    return outSum;
}

double dwData::mean() const
{
    double outVal = sum();
    return outVal/(double)data.size();
}

double dwData::sd() const
{
    double theMean = mean();
    dwData localCopy = *this;
    localCopy.subtract(theMean);
    localCopy.square();
    double theSum = localCopy.sum();
    theSum = theSum/(double)data.size();
    return sqrt(theSum);
}

//euclidean distance
double dwData::eDistance(const dwData & inMean) const
{
    if(inMean.size() != size())
        return(-1.0);
    dwData localCopy = *this;
    localCopy.subtract(inMean);
    localCopy.square();
    double sum = localCopy.sum();
    return sqrt(sum);
}

QString dwData::removeQuote(const QString & inTxt)
{
    QString outTxt = inTxt;
    if(outTxt.startsWith("\""))
        outTxt.remove(0,1);
    if(outTxt.endsWith("\""))
        outTxt.remove(outTxt.size()-1,1);
    return outTxt;
}
