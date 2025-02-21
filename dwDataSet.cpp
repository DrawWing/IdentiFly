#include <QFile>
#include <QRegularExpression>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>

#include "dwDataSet.h"
#include "dwData.h"

dwDataSet::dwDataSet()
{
    id = "";
}

void dwDataSet::fromCsv(const QString & resources)
{
    QFile inFile(resources);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream inStream(&inFile);
    readHeader(inStream);

    while(!inStream.atEnd())
    {
        dwData data;
        data.fromCsv(inStream);
        dataSet.push_back(data);
    }
    inFile.close();
    id = resources;
}

void dwDataSet::fromTps(const QString & resources)
{
    dwRclList rcl;
    rcl.fromTps(resources);
    fromRclList(rcl);
}

void dwDataSet::fromXml(const QDomElement & inElement)
{
    id = inElement.attribute("id");
    id = id.simplified();

    QDomNode headerNode = inElement.firstChild();
    QDomElement headerElem = headerNode.toElement();
    if(headerElem.isNull())
        return;
    QString headLine = headerElem.text();
    headLine = headLine.simplified(); //regex s+ is not working as expected
    QStringList headLineList = headLine.split(QRegularExpression("\\s+")); //split at white spaces
    for (int i = 0; i < headLineList.size(); ++i)
         header.push_back(headLineList.at(i));

    QDomNode vecNode = headerElem.nextSibling();
    while(!vecNode.isNull()) {
        QDomElement vecElement = vecNode.toElement();
        if(!vecElement.isNull()) {
            dwData inData;
            inData.fromXml(vecElement);
            dataSet.push_back(inData);
        }
        vecNode = vecNode.nextSibling();
    }

//    dwData data = dataSet[0]; //poprawic error handling
//    dataSize = data.size();
}

void dwDataSet::fromOneDir(const QString & inDirName)
{
    if (inDirName.isEmpty())
        return;

    QDir inDir( inDirName );
    QString filePath = inDir.absolutePath();
    QFileInfo dirInfo(inDirName);
    QString clas = dirInfo.fileName();
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*.png"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0)
        return;
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString fileName = localFileInfo.absoluteFilePath();

        dwData inData;
        inData.fromDwPng(fileName);
        inData.setClas(clas);
        if(inData.size() > 0)
            dataSet.push_back(inData);
    }
    std::vector< QString > inHeader;
//    inHeader.push_back("file");
    int pointsNo = dataSet[0].size()/2;
    for(int i = 0; i < pointsNo; ++i)
    {
        QString numStr;
        numStr.setNum(i+1);
        QString colStr;
        colStr = numStr + "x";
        inHeader.push_back(colStr);
        colStr = numStr + "y";
        inHeader.push_back(colStr);
    }
    setHeader(inHeader); // should be doen only once
}

void dwDataSet::fromDirStr(const QDir &inDir)
{
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::DirsFirst );
    if(localFileInfoList.size() == 0)
        return;
    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        if( !localFileInfo.isDir() )
            continue;
        if(localFileInfo.fileName() == "trash")
            continue;
        QString fileName = localFileInfo.absoluteFilePath();
        fromOneDir(fileName);
    }
}

void dwDataSet::fromCvMatDouble(const cv::Mat & inMat)
{
    if(inMat.dims > 2)
        return;

    for(int i = 0; i < inMat.rows; ++i)
    {
        const double* Mi = inMat.ptr<double>(i);
        dwData inData;
        for(int j = 0; j < inMat.cols; ++j)
            inData.push_back(Mi[j]);
        dataSet.push_back(inData);
    }
}

//void dwDataSet::fromCvMat2Cov(const cv::Mat & inMat, QString & inId, QString & preId, QString &preHead)
//{
//    if(inMat.dims > 2)
//        return;
//    dataSet.clear();
//    fromCvMatDouble(inMat);

//    header.clear();
//    if(preId == "")
//    {
//        QString prefix = "x";
//        int count = 1;
//        for (unsigned i=0; i < dataSet[0].size(); ++i)
//        {
//            QString valString = QString::number(count);
//            QString output = prefix+valString;
//            dataSet[i].setId(output);
//            if(prefix == "x")
//                prefix = "y";
//            else
//            {
//                prefix = "x";
//                ++count;
//            }
//        }
//    }else{
//        for (unsigned i=0; i < dataSet[0].size(); ++i)
//        {
//            QString valString = QString::number(i+1);
//            QString output = preId+valString;
//            dataSet[i].setId(output);
//        }
//    }
//    if(preHead == "")
//    {
//        QString prefix = "x";
//        int count = 1;
//        for (unsigned i=0; i < dataSet[0].size(); ++i)
//        {
//            QString valString = QString::number(count);
//            QString output = prefix+valString;
//            header.push_back(output);
//            if(prefix == "x")
//                prefix = "y";
//            else
//            {
//                prefix = "x";
//                ++count;
//            }
//        }
//    }else{
//        for (unsigned i=0; i < dataSet[0].size(); ++i)
//        {
//            QString valString = QString::number(i+1);
//            QString output = preId+valString;
//            header.push_back(output);
//        }
//    }

//    id = inId;
//}

void dwDataSet::fromRclList(const dwRclList & inList)
{
    dataSet.clear();
    std::vector< dwRCoordList > inVec = inList.list();
    for(unsigned i = 0; i < inVec.size(); ++i)
    {
        dwData inData;
        inData.fromRCoordList(inVec[i]);
        dataSet.push_back(inData);
    }

    header.clear();
    dwRCoordList pointList = inVec[0];
    for (unsigned i=0; i < pointList.size(); ++i)
    {
        QString valString = QString::number(i+1);
        QString output = "x"+valString;
        header.push_back(output);
        output = "y"+valString;
        header.push_back(output);
    }

    id = inList.getId();
}

void dwDataSet::readHeader(QTextStream & inStream)
{
    QString headLine = inStream.readLine();
    QStringList headLineList = headLine.split(";");
    for (int i = 0; i < headLineList.size(); ++i)
         header.push_back(headLineList.at(i));
}

QString dwDataSet::toCsv() const
{
    QString outTable = "id";
    outTable += ";";
    for (unsigned i=0; i<header.size(); ++i)
    {
        outTable += header[i];
        outTable += ";";
    }
    outTable += "\n";

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        QString theTable = data.toCsv();
        outTable += theTable;
    }
    return outTable;
}

QString dwDataSet::toXml() const
{
    QString outString = "<matrix id=\t\""+id+"\"\t>\n";

    outString += "<header>\t\t\t";
    for (unsigned i=0; i<header.size(); ++i)
    {
        outString += header[i];
        outString += "\t";
    }
    outString += "</header>\n";

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        QString outData = data.toXml();
        outString += outData;
    }

    outString += "</matrix>\n";
    return outString;
}

QString dwDataSet::toTable() const
{
    int rowIdMaxSize = 0;
    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        QString rowId = data.getId();
        int rowIdSize = rowId.length();
        if(rowIdSize > rowIdMaxSize)
            rowIdMaxSize = rowIdSize;
    }
    ++rowIdMaxSize;

    QString outTable;
    for( unsigned i = 0; i < header.size(); ++i)
    {
        outTable += QString::number(i+1);
        outTable += " \t";
        outTable += header[i];
        outTable += " \n";
    }
    outTable += " \n";

    QString idString = "id";
    while(idString.length() < rowIdMaxSize)
        idString.append(" ");

    outTable += idString;
    outTable += "\t";
    for (unsigned i=0; i<header.size(); ++i)
    {
//        outTable += header[i];
        outTable += QString::number(i+1);
        outTable += " \t";
    }
    outTable += "\n";

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        QString theTable = data.toTable(rowIdMaxSize);
        outTable += theTable;
    }
    return outTable;
}

QString dwDataSet::toHtmlTable() const
{
    QString outTable = "<table border=\"1\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">\n";

    outTable += "<tr>";

    outTable += "<td align=\"center\">";
    outTable += "id";
    outTable += "</td>";
    for (unsigned i=0; i<header.size(); ++i)
    {
        outTable += "<td align=\"center\">";
        outTable += header[i];
        outTable += "</td>";
    }
    outTable += "</tr>\n";

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        QString theTable = data.toHtmlTable();
        outTable += theTable;
    }
    outTable += "</table>\n";

    return outTable;
}

QString dwDataSet::toHtmlTableTranspose() const
{
    QString outTable = "<table border=\"1\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">\n";

    outTable += "<tr>";

    outTable += "<td align=\"center\">";
    outTable += "id";
    outTable += "</td>";
    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        outTable += "<td align=\"center\">";
        outTable += dataSet[i].getId();
        outTable += "</td>";
    }
    outTable += "</tr>\n";

    for (unsigned i=0; i<header.size(); ++i)
    {
        outTable += "<tr>";

        outTable += "<td align=\"center\">";
        outTable += header[i];
        outTable += "</td>";
        for (unsigned j=0; j<dataSet.size(); ++j)
        {
            outTable += "<td align=\"center\">";
            outTable += QString::number(valueXY(j,i), 'g', 8);  // e for all scientific
            outTable += "</td>";
        }

        outTable += "</tr>\n";
    }
    outTable += "</table>\n";

    return outTable;
}

QString dwDataSet::toHtmlMaxValue() const
{
    std::vector< unsigned > maxList = rowMaxIndex();

    QString outTable = "<table border=\"1\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">\n";

    outTable += "<tr>";

    outTable += "<td align=\"center\">";
//    outTable += "nr";
    outTable += "id";
    outTable += "</td>";

    outTable += "<td align=\"center\">";
//    outTable += "klasa";
    outTable += "most similar";
    outTable += "</td>";

    outTable += "<td align=\"center\">";
//    outTable += "prawdopodobieństwo";
    outTable += "probability";
    outTable += "</td>";

    outTable += "</tr>\n";

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        outTable += "<tr>";

        outTable += "<td align=\"center\">";
        outTable += data.getId();
        outTable += "</td>";

        unsigned max = maxList[i];
        outTable += "<td align=\"center\">";
        outTable += header[max];
        outTable += "</td>";

        double maxVal = data.at(max);
        outTable += "<td align=\"center\">";
        outTable += QString::number(maxVal, 'g', 6);
        outTable += "</td>";

        outTable += "</tr>\n";

    }
    outTable += "</table>\n";

    return outTable;
}

// format one row of classification data
QString dwDataSet::indexVector2html(std::vector< unsigned > inList, QString inTxt) const
{
    QString outTable = "<table border=\"1\" cellpadding=\"1\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">\n";

    outTable += "<tr>";

    outTable += "<td align=\"center\">";
    outTable += "id";
    outTable += "</td>";

    outTable += "<td align=\"center\">";
    outTable += "classified as";
    outTable += "</td>";

    outTable += "<td align=\"center\">";
    outTable += inTxt;
    outTable += "</td>";

    outTable += "</tr>\n";

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        outTable += "<tr>";

        outTable += "<td align=\"center\">";
        outTable += data.getId();
        outTable += "</td>";

        unsigned max = inList[i];
        outTable += "<td align=\"center\">";
        outTable += header[max];
        outTable += "</td>";

        double maxVal = data.at(max);
        outTable += "<td align=\"center\">";
        outTable += QString::number(maxVal, 'g', 6);
        outTable += "</td>";

        outTable += "</tr>\n";

    }
    outTable += "</table>\n";

    return outTable;
}

cv::Mat dwDataSet::toCvMatFloat() const
{
    unsigned dataSetSize = dataSet.size();
    unsigned dataSize = dataSet[0].size();

    cv::Mat mtx = cv::Mat::zeros(dataSetSize, dataSize, CV_32FC1);

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        std::vector< double > dataVec = data.getData();

        for (unsigned j=0; j<dataSize; ++j)
        {
            mtx.at< float >(i,j) = (float)dataVec[j];
        }
    }

    return mtx;
}

cv::Mat dwDataSet::toCvMatDouble() const
{
    unsigned dataSetSize = dataSet.size();
    unsigned dataSize = dataSet[0].size();

    cv::Mat mtx = cv::Mat::zeros(dataSetSize, dataSize, CV_64FC1);

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        std::vector< double > dataVec = data.getData();

        for (unsigned j=0; j<dataSize; ++j)
        {
            mtx.at< double >(i,j) = dataVec[j];
        }
    }

    return mtx;
}

dwRclList dwDataSet::toRclList() const
{
    dwRclList outList;
    outList.setId(id);
    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData outData = dataSet[i];
        dwRCoordList outRcl = outData.toRCoordList();
        outList.push_back(outRcl);
    }
    return outList;
}

cv::Mat dwDataSet::id2CvMat() const
{
    unsigned dataSetSize = dataSet.size();

    cv::Mat mtx = cv::Mat::zeros(dataSetSize, 1, CV_32FC1);

//    int count = 0;
    float labelNr;
    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        QString theLabel = data.getId();

        if(theLabel=="car")
            labelNr = -1.0;
        else if(theLabel=="cau")
            labelNr = 0.0;
        else if(theLabel=="mel")
            labelNr = 1.0;
        else
            labelNr = 4.0;

//        if(theLabel!=labelTxt)
//        {
//            ++count;
//            labelNr = count;
//            labelTxt = theLabel;
//        }
        mtx.at< float >(i,0) = labelNr;
    }

    return mtx;
}

dwData dwDataSet::at(unsigned i) const
{
    return dataSet.at(i);
}

double dwDataSet::valueXY(unsigned i, unsigned j) const
{
    dwData theData = dataSet.at(i);
    return theData.at(j);
}

double dwDataSet::atBottomRight() const
{
    dwData theData = dataSet.at(dataSet.size()-1);
    return theData.at(theData.size()-1);
}

void dwDataSet::push_back(dwData & inData)
{
    dataSet.push_back(inData);
}

void dwDataSet::setHeader(const std::vector<QString> & inHeader)
{
    header = inHeader;
}

void dwDataSet::setHeaderNames()
{
    header.clear();
    if(clasName.size() != dataSet[0].size())
        return;
    for (unsigned i=0; i < dataSet[0].size(); ++i)
    {
        header.push_back(clasName[i]);
    }
}

void dwDataSet::setHeaderCoord()
{
    header.clear();
    QString prefix = "x";
    int count = 1;
    for (unsigned i=0; i < dataSet[0].size(); ++i)
    {
        QString valString = QString::number(count);
        QString output = prefix+valString;
        header.push_back(output);
        if(prefix == "x")
            prefix = "y";
        else
        {
            prefix = "x";
            ++count;
        }
    }
}

void dwDataSet::setHeaderPrefix(QString & prefix)
{
    header.clear();
    for (unsigned i=0; i < dataSet[0].size(); ++i)
    {
        QString valString = QString::number(i+1);
        QString output = prefix+valString;
        header.push_back(output);
    }
}

std::vector< QString > dwDataSet::getHeader() const
{
    return header;
}

std::vector< QString > dwDataSet::getRowNames() const
{
    std::vector< QString > outList;
    for( unsigned i = 0; i < dataSet.size(); ++i)
    {
        dwData inData = dataSet[i];
        QString inClas = inData.getId();
        outList.push_back(inClas);
    }
    return outList;
}

void dwDataSet::rename(QStringList renameBefore, QStringList renameAfter)
{
    for( unsigned i = 0; i < dataSet.size(); ++i)
    {
        dwData theData = dataSet[i];
        QString theId = theData.getId();
        int index = renameBefore.indexOf(theId);
        if( index != -1)
            dataSet[i].setId(renameAfter[index]);
    }
}

void dwDataSet::setId(QString & inId)
{
    id = inId;
}

void dwDataSet::setDataId(const std::vector< QString > &inVec)
{
    if(inVec.size() != dataSet.size())
        return;
    for (unsigned i=0; i < dataSet.size(); ++i)
    {
        dataSet[i].setId(inVec[i]);
    }
}

//void dwDataSet::setIdName()
//{
//    if(clasName.size() != dataSet.size())
//        return;
//    for (unsigned i=0; i < dataSet.size(); ++i)
//    {
//        dataSet[i].setId(clasName[i]);
//    }
//}

void dwDataSet::setIdCoord()
{
    QString prefix = "x";
    int count = 1;
    for (unsigned i=0; i < dataSet[0].size(); ++i)
    {
        QString valString = QString::number(count);
        QString output = prefix+valString;
        dataSet[i].setId(output);
        if(prefix == "x")
            prefix = "y";
        else
        {
            prefix = "x";
            ++count;
        }
    }
}

void dwDataSet::setIdPrefix(QString & prefix)
{
    for (unsigned i=0; i < dataSet.size(); ++i)
    {
        QString valString = QString::number(i+1);
        QString output = prefix+valString;
        dataSet[i].setId(output);
    }
}

QString dwDataSet::getId() const
{
    return id;
}

void dwDataSet::setClasName(const std::vector< QString > & inVec)
{
    clasName = inVec;
}

unsigned dwDataSet::rows() const
{
    return dataSet.size();
}

unsigned dwDataSet::size() const
{
    return dataSet.size();
}

unsigned dwDataSet::columns() const
{
    if(rows() == 0)
        return 0;
    return dataSet.at(0).size();
}

dwData dwDataSet::CvaScores(dwData & inData)
{
    dwData outData;
    dwData first = dataSet[0];
//    if(inData.size()+1 != first.size())
//        return outData;

    if(inData.size() == first.size()) // without constant
    {
        std::vector< double > inVec = inData.getData();
        for( unsigned i = 0; i < dataSet.size(); ++i)
        {
            dwData cvaCoe = dataSet[i];
            std::vector< double > cvaCoeVec = cvaCoe.getData();
            double score = 0.0;
            for( unsigned j = 0; j < inVec.size(); ++j)
            {
                score += cvaCoeVec[j] * inVec[j];
            }
            outData.push_back(score);
        }
    }else if(inData.size() == first.size() - 1 ) // with constant
    {
        std::vector< double > inVec = inData.getData();
        for( unsigned i = 0; i < dataSet.size(); ++i)
        {
            dwData cvaCoe = dataSet[i];
            std::vector< double > cvaCoeVec = cvaCoe.getData();
            double score = 0.0;
            for( unsigned j = 0; j < inVec.size(); ++j)
            {
                score += cvaCoeVec[j] * inVec[j];
            }
            unsigned last = inVec.size();
            score += cvaCoeVec[last]; //add constant
            outData.push_back(score);
        }
    }else
        return outData; // wrong size

    QString theId = inData.getId();
    outData.setId(theId);
    return outData;
}

std::vector< unsigned >  dwDataSet::rowMaxIndex() const
{
    std::vector< unsigned >  outList;

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        unsigned index = data.maxIndex();
        outList.push_back(index);
    }

    return outList;
}

std::vector< unsigned >  dwDataSet::rowMinIndex() const
{
    std::vector< unsigned >  outList;

    for (unsigned i=0; i<dataSet.size(); ++i)
    {
        dwData data = dataSet[i];
        unsigned index = data.minIndex();
        outList.push_back(index);
    }

    return outList;
}

QString dwDataSet::getDwXml(int idSize, int dim)
{
    cv::Mat inMat = toCvMatDouble();
    cv::PCA pca;
    if(dim == 0){
        double retainedVar = 0.99;
        pca(inMat, // pass the data
            cv::Mat(), // we do not have a pre-computed mean vector, so let the PCA engine to compute it
            cv::PCA::DATA_AS_ROW, // DATA_AS_COL = 1  data stored as columns alternative PCA::DATA_AS_ROW
//            CV_PCA_DATA_AS_ROW, // DATA_AS_COL = 1  data stored as columns alternative PCA::DATA_AS_ROW
//            components //maxComponents // specify, how many principal components to retain
            retainedVar
            );
    }else
        pca(inMat, // pass the data
            cv::Mat(), // we do not have a pre-computed mean vector, so let the PCA engine to compute it
            cv::PCA::DATA_AS_ROW, // DATA_AS_COL = 1  data stored as columns alternative PCA::DATA_AS_ROW
//            CV_PCA_DATA_AS_ROW, // DATA_AS_COL = 1  data stored as columns alternative PCA::DATA_AS_ROW
            dim //maxComponents // specify, how many principal components to retain
            //                retainedVar
            );

    cv::Mat pcaValues = pca.eigenvalues;
    cv::Mat pcaVector = pca.eigenvectors;
    cv::Mat pcaMean = pca.mean;

//    cv::Mat pcaScores = pca.project(inMat); // projection results are different from mulitplication below
    cv::Mat pcaScores = inMat * pcaVector.t();

    extractClasIdTrunc(idSize); // class in first 9 letters
    cv::Mat clasMat(clasVec);
    clasMat = clasMat.t();

    cv::LDA lda(pcaScores, clasMat);
    cv::Mat ldaValues = lda.eigenvalues();
    cv::Mat ldaVector = lda.eigenvectors();

//    cv::Mat ldaScores = lda.project(pcaScores);
    cv::Mat ldaScores = pcaScores * ldaVector;

    std::vector< cv::Mat > splitVec;
    for(unsigned i = 0; i < clasName.size(); ++i)
        splitVec.push_back(cv::Mat());
    for(unsigned i = 0; i < clasVec.size(); ++i)
    {
        int index = clasVec[i];
        if(index < 0)
            continue;
        if(index > (int)splitVec.size()-1)
            continue;
        splitVec[index].push_back(ldaScores.row(i));
    }
    std::vector< cv::Mat > covaVec;
    cv::Mat meanVec;
    for(unsigned i = 0; i < splitVec.size(); ++i)
    {
        cv::Mat covar, mean;
        cv::calcCovarMatrix(splitVec[i], covar, mean, cv::COVAR_NORMAL | cv::COVAR_ROWS, CV_64F );
        covar = covar / (splitVec[i].rows - 1);
        covaVec.push_back(covar);
        meanVec.push_back(mean.row(0));
    }

    cv::Mat coeffMat = pcaVector.t() * ldaVector;
    coeffMat = coeffMat.t();

//    ///////////// test output
//    std::ofstream testFile;
//    testFile.open("E:/wings/test/debug/test.xml", std::ofstream::out);
//    dwDataSet testSet;
//    testSet.fromCvMatDouble(pcaScores); // was testMat
//    QString testTxt = testSet.toXml();
//    testFile<<testTxt.toStdString();
//    testFile.close();

    // format output
    std::vector< QString > clasNameXml = strListHtmlEscaped(clasName);

    QString outTxt= "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    outTxt += "<identifly version=\"\t1.0\t\">\n"; //add version from qapplication
    outTxt += "<prototype file=\t\"error.png\"\t/>\n";
    outTxt += "<cva>\n";
    setHeaderCoord();
    dwDataSet reference = mean();
    QString tmpStr = "reference";
    reference.setId(tmpStr);
    outTxt += reference.toXml();

    dwDataSet coeff;
    coeff.fromCvMatDouble(coeffMat);
    tmpStr = "coefficients";
    coeff.setId(tmpStr);
    coeff.setHeaderCoord();
    tmpStr = "CV";
    coeff.setIdPrefix(tmpStr);
    outTxt += coeff.toXml();

    dwDataSet means;
    means.fromCvMatDouble(meanVec);
    tmpStr = "means";
    means.setId(tmpStr);
    tmpStr = "CV";
    means.setHeaderPrefix(tmpStr);
    means.setDataId(clasNameXml);
    outTxt += means.toXml();

    outTxt += "<covariances>\n";
    for(unsigned i = 0; i < splitVec.size(); ++i)
    {
        dwDataSet cov;
        cov.fromCvMatDouble(covaVec[i]);
        cov.setId(clasNameXml[i]);
        cov.setHeaderPrefix(tmpStr);
        cov.setIdPrefix(tmpStr);
        outTxt += cov.toXml();
    }
    outTxt += "</covariances>\n";

    ///
//        dwDataSet distances;
//        tmpStr = "distances";
//        distances.setId(tmpStr);

//        for(unsigned i = 0; i < splitVec.size(); ++i)
//        {
//            cv::Mat scoresMat = splitVec[i];
//            dwDataSet scoresSet;
//            scoresSet.fromCvMatDouble(scoresMat);
//            dwData distData = scoresSet.eDistance(means.at(i));
//            double theMean = distData.mean();
//            double theSD = distData.sd();
//            double theSize = distData.size();
//            dwData theData;
//            theData.push_back(theMean);
//            theData.push_back(theSD);
//            theData.push_back(theSize);
//            distances.push_back(theData);
//        }
//        std::vector< QString > theHeader;
//        theHeader.push_back("mean");
//        theHeader.push_back("sd");
//        theHeader.push_back("n");
//        distances.setHeader(theHeader);
//        distances.setDataId(clasName);
//        outTxt += distances.toXml();
    ///

    outTxt += "</cva>\n";
    outTxt += "</identifly>\n";

//    cv::FileStorage fs( "E:/wings/test/debug/pca.xml", cv::FileStorage::WRITE );
//    fs << "Size" << 26;
//    if( !pcaScores.empty() )
//        fs << "PCAscores" << pcaScores;
//    if( !pcaValues.empty() )
//        fs << "PCAeigenvalues" << pcaValues;
//    if( !pcaVector.empty() )
//        fs << "PCAeigenvector" << pcaVector;
//    if( !pcaMean.empty() )
//        fs << "PCAmeans" << pcaMean;
//    if( !clasMat.empty() )
//        fs << "Classificationdata" << clasMat;
//    if( !ldaValues.empty() )
//        fs << "LDAeigenvalues" << ldaValues;
//    if( !ldaVector.empty() )
//        fs << "LDAeigenvectors" << ldaVector;
//    if( !ldaScores.empty() )
//        fs << "LDAscores" << ldaScores;
//    for(unsigned i = 0; i < splitVec.size(); ++i)
//    {
//        if( !covaVec[i].empty() )
//            fs << "covarianceMatrix" << covaVec[i];
//    }
//    if( !meanVec.empty() )
//        fs << "LDAmeans" << meanVec;
//    fs.release();

    return outTxt;
}

// return maximum number of components of pca
int dwDataSet::pcaDim()
{
    cv::Mat inMat = toCvMatDouble();
    cv::PCA pca(inMat, // pass the data
            cv::Mat(), // we do not have a pre-computed mean vector, so let the PCA engine to compute it
                cv::PCA::DATA_AS_ROW, // DATA_AS_COL = 1  data stored as columns alternative PCA::DATA_AS_ROW
//                CV_PCA_DATA_AS_ROW, // DATA_AS_COL = 1  data stored as columns alternative PCA::DATA_AS_ROW
                0 //maxComponents // specify, how many principal components to retain
            );
    cv::Mat pcaValues = pca.eigenvalues;
    return pcaValues.rows; //return number of eigenvalues
}

void dwDataSet::pca(int idSize)
{
    cv::Mat inMat = toCvMatDouble();
//    int components = 24;
    double retainedVar = 0.99;
    cv::PCA pca(inMat, // pass the data
            cv::Mat(), // we do not have a pre-computed mean vector, so let the PCA engine to compute it
            cv::PCA::DATA_AS_ROW, // DATA_AS_COL = 1  data stored as columns alternative PCA::DATA_AS_ROW
//                CV_PCA_DATA_AS_ROW, // DATA_AS_COL = 1  data stored as columns alternative PCA::DATA_AS_ROW
//                components //maxComponents // specify, how many principal components to retain
                retainedVar
            );

    cv::Mat pcaValues = pca.eigenvalues;
    cv::Mat pcaVector = pca.eigenvectors;
    cv::Mat pcaMean = pca.mean;

    cv::Mat testMat;
//    cv::Mat pcaScores = pca.project(inMat); // projection results are different from mulitplication below
    cv::Mat pcaScores = inMat * pcaVector.t();

    extractClasIdTrunc(idSize); // class in first 9 letters
    cv::Mat clasMat(clasVec);
    clasMat = clasMat.t();
    cv::LDA lda(pcaScores, clasMat);
    cv::Mat ldaValues = lda.eigenvalues();
    cv::Mat ldaVector = lda.eigenvectors();

//    cv::Mat ldaScores = lda.project(pcaScores);
    cv::Mat ldaScores = pcaScores * ldaVector;

    // covariance matrix
//    cv::Mat covar, mean;
//    cv::calcCovarMatrix(ldaScores, covar, mean, CV_COVAR_NORMAL+CV_COVAR_SCALE+CV_COVAR_ROWS, CV_64F );

    std::vector< cv::Mat > splitVec;
    for(unsigned i = 0; i < clasName.size(); ++i)
        splitVec.push_back(cv::Mat());
    for(unsigned i = 0; i < clasVec.size(); ++i)
    {
        int index = clasVec[i];
        if(index < 0)
            continue;
        if(index > (int)splitVec.size()-1)
            continue;
        splitVec[index].push_back(ldaScores.row(i));
    }
    std::vector< cv::Mat > covaVec;
    cv::Mat meanVec;
    for(unsigned i = 0; i < splitVec.size(); ++i)
    {
        cv::Mat covar, mean;
        cv::calcCovarMatrix(splitVec[i], covar, mean, cv::COVAR_NORMAL | cv::COVAR_ROWS, CV_64F );
        covar = covar / (splitVec[i].rows - 1);
        covaVec.push_back(covar);
        meanVec.push_back(mean.row(0));
    }

    cv::Mat coeffMat = pcaVector.t() * ldaVector;
//    cv::Mat testMat = inMat * coeffMat;
    coeffMat = coeffMat.t();


    ///////////// test output
    // std::ofstream testFile;
    // testFile.open("E:/wings/test/debug/test.xml", std::ofstream::out);
    // dwDataSet testSet;
    // testSet.fromCvMatDouble(pcaScores); // was testMat
    // QString testTxt = testSet.toXml();
    // testFile<<testTxt.toStdString();
    // testFile.close();

    // format output
    QString outTxt= "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    outTxt += "<identifly version=\"\t1.0\t\">\n"; //add version from qapplication
    outTxt += "<prototype file=\t\"eristalis-prototype.dw.png\"\t/>\n";
    outTxt += "<cva>\n";
    setHeaderCoord();
    dwDataSet reference = mean();
    QString tmpStr = "reference";
    reference.setId(tmpStr);
    outTxt += reference.toXml();

    dwDataSet coeff;
    coeff.fromCvMatDouble(coeffMat);
    tmpStr = "coefficients";
    coeff.setId(tmpStr);
    coeff.setHeaderCoord();
    tmpStr = "CV";
    coeff.setIdPrefix(tmpStr);
    outTxt += coeff.toXml();

    dwDataSet means;
    means.fromCvMatDouble(meanVec);
    tmpStr = "means";
    means.setId(tmpStr);
    tmpStr = "CV";
    means.setHeaderPrefix(tmpStr);
    means.setDataId(clasName);
    outTxt += means.toXml();

    outTxt += "<covariances>\n";
    for(unsigned i = 0; i < splitVec.size(); ++i)
    {
        dwDataSet cov;
        cov.fromCvMatDouble(covaVec[i]);
        cov.setId(clasName[i]);
        cov.setHeaderPrefix(tmpStr);
        cov.setIdPrefix(tmpStr);
        outTxt += cov.toXml();
    }
    outTxt += "</covariances>\n";
    outTxt += "</cva>\n";
    outTxt += "</identifly>\n";
//    outTxt += toTable();

    // std::ofstream outFile;
    // outFile.open("E:/wings/test/debug/cva.dw.xml", std::ofstream::out);
    // //    QString xmlDir = QCoreApplication::applicationDirPath();
    // //    xmlDir += "/dwxml/classification_.dw.xml";
    // //    outFile.open(qPrintable(xmlDir), std::ofstream::out);
    // outFile<<outTxt.toStdString();
    // outFile.close();

    // cv::FileStorage fs( "E:/wings/test/debug/pca.xml", cv::FileStorage::WRITE );
    // fs << "Size" << 26;
    // if( !pcaScores.empty() )
    //     fs << "PCAscores" << pcaScores;
    // if( !pcaValues.empty() )
    //     fs << "PCAeigenvalues" << pcaValues;
    // if( !pcaVector.empty() )
    //     fs << "PCAeigenvector" << pcaVector;
    // if( !pcaMean.empty() )
    //     fs << "PCAmeans" << pcaMean;
    // if( !clasMat.empty() )
    //     fs << "Classificationdata" << clasMat;
    // if( !ldaValues.empty() )
    //     fs << "LDAeigenvalues" << ldaValues;
    // if( !ldaVector.empty() )
    //     fs << "LDAeigenvectors" << ldaVector;
    // if( !ldaScores.empty() )
    //     fs << "LDAscores" << ldaScores;
    // if( !testMat.empty() )
    //     fs << "LDAtest" << testMat;
    // for(unsigned i = 0; i < splitVec.size(); ++i)
    // {
    //     if( !covaVec[i].empty() )
    //         fs << "covarianceMatrix" << covaVec[i];
    // }
    // if( !meanVec.empty() )
    //     fs << "LDAmeans" << meanVec;
    // fs.release();
}

void  dwDataSet::lda()
{
    double d[11][2] = {
                {2, 3},
                {3, 4},
                {4, 5},
                {5, 6},
                {5, 7},
                {2, 1},
                {3, 2},
                {4, 2},
                {4, 3},
                {6, 4},
                {7, 6}};
    cv::Mat inMat = cv::Mat(11, 2, CV_64FC1, &d);
    cv::Mat inLabels = (cv::Mat_<int>(1,11) << 0,0,0,0,0,1,1,1,1,1,1);

    cv::LDA lda(inMat, inLabels);

    cv::Mat pcaValues = lda.eigenvalues();
    dwDataSet eigenVal;
    eigenVal.fromCvMatDouble(pcaValues);

    cv::Mat pcaVector = lda.eigenvectors();
    dwDataSet eigenVec;
    eigenVec.fromCvMatDouble(pcaVector);

    cv::Mat projected = lda.project(inMat);
}

std::vector< int > dwDataSet::extractClas() const
{
    std::vector< int > outVec;

    QString prevClas = "";
    int index = -1;
    for( unsigned i = 0; i < dataSet.size(); ++i)
    {
        dwData inData = dataSet[i];
        QString inClas = inData.getClas();
        if(inClas != prevClas)
            ++index;
        outVec.push_back(index);
        prevClas = inClas;
    }
    return outVec;
}

void dwDataSet::extractClasId()
{
    clasVec.clear();
    clasName.clear();
    QString prevClas = "";
    for( unsigned i = 0; i < dataSet.size(); ++i)
    {
        dwData inData = dataSet[i];
        QString inClas = inData.getId();
        QStringList clasList = inClas.split('/');
        clasList.pop_back();
        inClas = clasList.back();
        if(inClas != prevClas)
        {
            clasName.push_back(inClas);
            prevClas = inClas;
        }
        clasVec.push_back(clasName.size()-1);
    }
}

void dwDataSet::extractClasIdTrunc(int position)
{
    clasVec.clear();
    clasName.clear();
    QString prevClas = "";
    for( unsigned i = 0; i < dataSet.size(); ++i)
    {
        dwData inData = dataSet[i];
        QString inClas = inData.getId();
        inClas.truncate(position);
        if(inClas != prevClas)
        {
            clasName.push_back(inClas);
            prevClas = inClas;
        }
        clasVec.push_back(clasName.size()-1);
    }
}

dwDataSet dwDataSet::mean() const
{
    dwData outData = dataSet[0]; //first row

    for( unsigned i = 1; i < dataSet.size(); ++i) //start from second row
    {
        outData.add(dataSet[i]);
    }
    outData.divide( dataSet.size());

    QString outId = "mean";
    outData.setId(outId);
    dwDataSet outDataSet;
    outDataSet.push_back(outData);
    outDataSet.setHeader(header);
    return outDataSet;
}

// dwDataSet contains inverse of covariance matrix
double dwDataSet::Mahalanobis(dwData inVec, dwData inMean) const
{
    double result = -1.0;
    if(inVec.size() != inMean.size())
        return result;
    if(inVec.size() != dataSet.size())
        return result;
    if(inVec.size() != dataSet[0].size())
        return result;
    inVec.subtract(inMean); //inVec stres difference between the vectors
    dwData mult;
    for(unsigned i = 0; i < inVec.size(); ++i)
    {
        double tmp = 0;
        dwData column = dataSet[i]; // inMean is symetrical
        for(unsigned j = 0; j < column.size(); ++j)
        {
            tmp += inVec.at(j)*column.at(j);
        }
        mult.push_back(tmp);
    }

    result  = 0;
    //second multiplication
    for(unsigned i = 0; i < mult.size(); ++i)
    {
        result += mult.at(i)*inVec.at(i);
    }

    return result;
}

// calculate distance information from cva scores
// dwDataSet contains cva scores inData contains mean
dwData dwDataSet::eDistance(const dwData & inData) const
{
    dwData outData;
    for( unsigned i = 0; i < dataSet.size(); ++i)
    {
        dwData theData = dataSet[i];
        double dist = theData.eDistance(inData);
        outData.push_back(dist);
    }
    return outData;
}

// remove one row and return it
dwData dwDataSet::removeRow(unsigned rowNo)
{
    dwData outRow = dataSet[rowNo];
    dataSet.erase(dataSet.begin()+rowNo);
    return outRow;
}

std::vector< QString > dwDataSet::strListHtmlEscaped(std::vector< QString > &inList)
{
    std::vector< QString > outList;
    for( unsigned i = 0; i < inList.size(); ++i)
    {
        QString theString = inList[i];
        theString = theString.toHtmlEscaped();
        outList.push_back(theString);
    }
    return outList;
}

// the the data set has to be earlier aligned
// calculate Mahalanobis distance for earch row
// and sort row ids accordint to MD
std::vector< QString > dwDataSet::outliersMD(void)
{
    // calculate covariance matrix
    cv::Mat dataSetMat = toCvMatDouble();
    cv::Mat covar, mean;
    cv::calcCovarMatrix(dataSetMat, covar, mean, cv::COVAR_NORMAL | cv::COVAR_ROWS, CV_64F );
    covar = covar / (dataSetMat.rows - 1);

    std::vector< std::pair<double, QString> > pairList;
    for(unsigned i = 0; i < dataSet.size(); ++i)
    {
        dwData theData = dataSet.at(i);
        cv::Mat theDataMat = theData.toCvMatDouble();
        double theDist = cv::Mahalanobis(theDataMat, mean, covar);
        std::pair< double, QString > thePair = {theDist, theData.getId()};
        pairList.push_back(thePair);
    }

    // sort from large to small MD
    sort(pairList.begin(), pairList.end(), std::greater<>());

    std::vector< QString > outStrList;
    for(unsigned i = 0; i < pairList.size(); ++i)
    {
        outStrList.push_back(pairList.at(i).second);
    }
    return outStrList;
}
