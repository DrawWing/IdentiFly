#include "dwldagm.h"
#include "dwDataSet.h"
#include "dwChisqr.h"

#include <QGraphicsScene>
//#include <QSvgGenerator>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

using namespace cv;

dwLdaGm::dwLdaGm()
{
}

dwLdaGm::dwLdaGm(QDomDocument xmlDoc)
{
    fromXml(xmlDoc);
}

void dwLdaGm::fromXml(QDomDocument XmlDoc)
{
    QString elementName = "identifly";
    QDomElement docElement = XmlDoc.namedItem(elementName).toElement();
    if ( docElement.isNull() )
    {
        error = QObject::tr("No <%1> element found in the XML file!").arg(elementName);
        return;
    }
    QString versionTxt = docElement.attribute("version");
    version = versionTxt.toDouble();

    QDomElement ldaGmElement;
    QDomElement cvaElement;
    QDomElement renameElement;

    QDomNodeList docElemList = docElement.childNodes();
    for(int i = 0; i < docElemList.size(); ++i)
    {
        QDomNode aNode = docElemList.at(i);
        QDomElement anElement = aNode.toElement();
        if(anElement.isNull())
            continue; // report error?
        if(anElement.tagName() == "lda_gm")
        {
            ldaGmElement = anElement;
        }
        else if(anElement.tagName() == "cva")
        {
            cvaElement = anElement;
        }
        else if(anElement.tagName() == "lda")
        {
            extractLdaData(anElement);
        }
        else if(anElement.tagName() == "prototype")
        {
            prototypeFile = anElement.attribute("file").simplified();
//            if(version > 1.0)
//                prototypeFile = anElement.text().simplified();
//            else
//                prototypeFile = anElement.attribute("file").simplified();
        }
        else if(anElement.tagName() == "rename")
        {
            extractRename(anElement);
        }
    }
    if( !ldaGmElement.isNull() )
    {
        extractLdaGmData(ldaGmElement);
    }
    if( !cvaElement.isNull() )
    {
        extractCvaData(cvaElement);
    }

    renameInData();
}

void dwLdaGm::renameInData()
{
    if(renameShort.size() == 0)
        return;

    // cvMeans
    if(cvMeans.size() != 0)
    {
        cvMeans.rename(renameShort, renameLong);
        clasName = cvMeans.getRowNames(); // there is one in extractCvData
    }
    // cvCovVec
    for( unsigned i = 0; i < cvCovVec.size(); ++i )
    {
        dwDataSet theDataSet = cvCovVec[i];
        QString theId = theDataSet.getId();
        int index = renameShort.indexOf(theId);
        if( index != -1)
            cvCovVec[i].setId(renameLong[index]);
    }
}

void dwLdaGm::extractLdaGmData(QDomElement & ldaGmElement)
{
    QDomNodeList ldaElemList = ldaGmElement.childNodes();
    for(int i = 0; i < ldaElemList.size(); ++i){
        QDomNode aNode = ldaElemList.at(i);
        QDomElement anElement = aNode.toElement();
        if(anElement.isNull())
            continue; // report error?
        if(anElement.attribute("id") == "reference")
        {
            dwDataSet refData = extractMatrix(anElement);
            if ( (refData.columns() & 1) == 1 ) //is size odd
                error = "Reference data size need to be even number.";
    //        error = QObject::tr("Reference data size need to be even number.");
            reference = refData.at(0).toRCoordList();
        }else if(anElement.attribute("id") == "classification_functions")
        {
            cf = extractMatrix(anElement);
        }
    }
}

void dwLdaGm::extractCvaData(QDomElement & cvaElement)
{
    QDomNodeList cvaElemList = cvaElement.childNodes();
    for(int i = 0; i < cvaElemList.size(); ++i){
        QDomNode aNode = cvaElemList.at(i);
        QDomElement anElement = aNode.toElement();
        if(anElement.isNull())
            continue; // report error?
        if(anElement.attribute("id") == "reference")
        {
            dwDataSet refData = extractMatrix(anElement);
            if ( (refData.columns() & 1) == 1 ) //is size odd
                error = "Reference data size need to be even number.";
            //        error = QObject::tr("Reference data size need to be even number.");
            reference = refData.at(0).toRCoordList();
        }else if(anElement.attribute("id") == "coefficients")
        {
            cvCoe = extractMatrix(anElement);
        }else if(anElement.attribute("id") == "means")
        {
            cvMeans = extractMatrix(anElement);
            clasName = cvMeans.getRowNames(); // there is one in renameInData
        }else if(anElement.tagName() == "covariances")
        {
            QDomNode classNode = anElement.firstChild();
            while(!classNode.isNull()) {
                QDomElement classElement = classNode.toElement();
                if(!classElement.isNull()) {
                    dwDataSet inData;
                    inData.fromXml(classElement);
                    cvCovVec.push_back(inData);
                }
                classNode = classNode.nextSibling();
            }
        }else if(anElement.attribute("id") == "distances")
        {
            cvDist = extractMatrix(anElement);
        }
    }
}
void dwLdaGm::extractLdaData(QDomElement & inElement)
{
    QDomNodeList cvaElemList = inElement.childNodes();
    for(int i = 0; i < cvaElemList.size(); ++i){
        QDomNode aNode = cvaElemList.at(i);
        QDomElement anElement = aNode.toElement();
        if(anElement.isNull())
            continue; // report error?
        if(anElement.attribute("id") == "reference")
        {
            dwDataSet refData = extractMatrix(anElement);
            if ( (refData.columns() & 1) == 1 ) //is size odd
                error = "Reference data size need to be even number.";
            //        error = QObject::tr("Reference data size need to be even number.");
            reference = refData.at(0).toRCoordList();
        }else if(anElement.attribute("id") == "coefficients")
        {
            cvCoe = extractMatrix(anElement);
        }else if(anElement.attribute("id") == "means")
        {
            rawMeans = extractMatrix(anElement);
            clasName = cvMeans.getRowNames(); // there is one in renameInData
        }else if(anElement.tagName() == "covariances")
        {
            QDomNode classNode = anElement.firstChild();
            while(!classNode.isNull()) {
                QDomElement classElement = classNode.toElement();
                if(!classElement.isNull()) {
                    dwDataSet inData;
                    inData.fromXml(classElement);
                    cvCovVec.push_back(inData);
                }
                classNode = classNode.nextSibling();
            }
        }
    }
    // convert group means from raw to cv
    dwRclList rawMeansRcl = rawMeans.toRclList();
    cvMeans = cvaScores(rawMeansRcl);
}

void dwLdaGm::extractRename(QDomElement & inElement)
{
    QDomNodeList inElemList = inElement.childNodes();
    for(int i = 0; i < inElemList.size(); ++i){
        QDomNode aNode = inElemList.at(i);
        QDomElement anElement = aNode.toElement();
        if(anElement.tagName() == "name")
        {
            QDomNodeList nameNodeList = anElement.childNodes();
            for(int j = 0; j < nameNodeList.size(); ++j){
                QDomNode nameNode = nameNodeList.at(j);
                QDomElement nameElement = nameNode.toElement();
                if(nameElement.tagName() == "short")
                {
                    renameShort.push_back(nameElement.text().simplified());
                }
                else if(nameElement.tagName() == "long")
                {
                    renameLong.push_back(nameElement.text().simplified());
                }
            }
        }
    }
}

dwDataSet dwLdaGm::extractMatrix(QDomElement inElement)
{
    dwDataSet outData;
    outData.fromXml(inElement);
    return outData;
}

QString dwLdaGm::getError() const
{
    return error;
}

QString dwLdaGm::getPrototype() const
{
    return prototypeFile;
}

unsigned dwLdaGm::getReferenceSize() const
{
    return reference.size();
}

bool dwLdaGm::isSane()
{
    // fill the clasName
    clasName = cvMeans.getRowNames();
//    unsigned clasNumber = clasName.size();

    // can be with constant
//    if(cf.columns() != 2*reference.size())
//        error+="Size of reference and classification functions is not equal.";

    if(clasName.size() != cvCovVec.size())
    {
        error+="Number of covariance matices unequal number of classes.";
        return false;
    }

    unsigned cvNumber = cvMeans.rows();
    if(cvCoe.rows() != cvNumber)
    {
        error+="Number of canonical variates error.";
        return false;
    }
    for(unsigned i = 0; i < cvCovVec.size(); ++i)
    {
        if( cvCovVec[i].rows() != cvNumber ||
                cvCovVec[i].columns() != cvNumber )
        {
            error+="Number of canonical variates error.";
            return false;
        }
    }
    return true;
}

bool dwLdaGm::hasLda()
{
    if(cf.size() == 0)
        return false;
    if(reference.size() == 0)
        return false;
    if(cf.columns() != reference.size()+1 )
        return false;
    return true;
}

QString dwLdaGm::classify(dwRCoordList inList)
{
    QString outStr;

    if(cf.rows() == 0) // no classification functions
        return outStr;

    // Verify sizes of vectors
    unsigned refSize = reference.size();
    if(inList.list().size() < refSize)
    {
        error = QObject::tr("Number of landmarks smaller than %1.").arg(refSize);
        return outStr;
    }

    inList.setValidSize(refSize); // if inList is longer than refSize some points are not analysed

    double procrustesD = inList.superimpose(reference); //align the inList to reference which is the first configuration
    double thdDistance = 100.0;
    if (procrustesD > thdDistance)
        qDebug() << "Large procrustes distance between reference and input: " << procrustesD;

    double maxScore = -999999.9;
    int maxSpecies = 0;
    for(unsigned species = 0; species < cf.rows(); ++species){
        dwData curLst = cf.at(species);
        std::vector< double > curVec = curLst.getData();
        double score = 0;
        unsigned vecI = 0;
        for(unsigned i = 0; i < refSize; ++i){
            score += inList.list().at(i).dx() * curVec.at(vecI);
            ++vecI;
            score += inList.list().at(i).dy() * curVec.at(vecI);
            ++vecI;
        }
        score += curVec.at(vecI); //add constant
//        qDebug() << score;
        if(score > maxScore){
            maxScore = score;
            maxSpecies = species;
        }
    }
    outStr = cf.at(maxSpecies).getId();
    return outStr;
}

// return class number
int dwLdaGm::classifyCv(dwData inData)
{
//    QString outStr;
    // Verify sizes of vectors
    dwRCoordList inList = inData.toRCoordList();
    unsigned refSize = reference.size();
    if(inList.size() < refSize)
    {
        error = QObject::tr("Number of landmarks smaller than %1.").arg(refSize);
        return -1;
    }
    inList.setValidSize(refSize); // if inList is longer than refSize some points are not analysed

    inList.superimpose(reference); //align the inList to reference which is the first configuration
    inData.fromRCoordList(inList);

    dwData theMean = cvMeans.at(0);
    dwDataSet theCov = cvCovVec.at(0);
    double minDist = mahDist(inData, theMean, theCov);
    int maxSpecies = 0;
    for(unsigned species = 1; species < cvMeans.rows(); ++species){
        theMean = cvMeans.at(species);
        theCov = cvCovVec.at(species);
        double dist = mahDist(inData, theMean, theCov);
        if(dist < minDist){
            minDist = dist;
            maxSpecies = species;
        }
    }
//    outStr = clasName[maxSpecies];
    return maxSpecies;
}

// return class number
// classification based on euclidean distance
int dwLdaGm::classifyCvDist(dwData inData)
{
    // Verify sizes of vectors
    dwRCoordList inList = inData.toRCoordList();
    unsigned refSize = reference.size();
    if(inList.size() < refSize)
    {
        error = QObject::tr("Number of landmarks smaller than %1.").arg(refSize);
        return -1;
    }
    inList.setValidSize(refSize); // if inList is longer than refSize some points are not analysed

    inList.superimpose(reference); //align the inList to reference which is the first configuration
    inData.fromRCoordList(inList);

    dwData scores = cvCoe.CvaScores(inData);

    dwData theMean = cvMeans.at(0);
    dwData distData = cvDist.at(0);
    double eDist = scores.eDistance(theMean);
    double z = (distData.at(0)-eDist)/distData.at(1); // (dist-meanDist)/sd
    double minDist = cdfProb(z);
    if(distData.at(0) < eDist)
        minDist = 1.0-minDist;
    int maxSpecies = 0;
    for(unsigned species = 1; species < cvMeans.rows(); ++species){
        theMean = cvMeans.at(species);
        dwData distData = cvDist.at(species);
        double eDist = scores.eDistance(theMean);
        double z = (distData.at(0)-eDist)/distData.at(1); // (dist-meanDist)/sd
        double dist = cdfProb(z);
        if(distData.at(0) < eDist)
            dist = 1.0-dist;
        if(dist > minDist){
            minDist = dist;
            maxSpecies = species;
        }
    }
    return maxSpecies;
}

QStringList dwLdaGm::classify(dwRclList & inList)
{
    QStringList outList;

    std::vector< dwRCoordList > inVec = inList.lst();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theList = inVec[i];
        QString theStr = classify(theList);
        outList.push_back(theStr);
    }
    return outList;
}

// Calculate Mahalanobis distance between inList and all means
QString dwLdaGm::classifyTab(dwRclList & inList)
{
    QString outTable;
    if(cf.rows() == 0)
        return outTable;
    std::vector< QString >  idList = inList.getIdList();

    QStringList classList;
    std::vector< dwRCoordList > inVec = inList.lst();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theList = inVec[i];
        QString theStr = classify(theList);
        classList.push_back(theStr);
    }

    outTable = "<table border=\"1\" cellpadding=\"1\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">\n";

    outTable += "<tr>";
    outTable += "<td align=\"center\">";
    outTable += "id";
    outTable += "</td>";
    outTable += "<td align=\"center\">";
    outTable += "classified as";
    outTable += "</td>";
    outTable += "</tr>\n";

    for (unsigned i = 0; i < idList.size(); ++i)
    {
        outTable += "<tr>";
        outTable += "<td align=\"center\">";
        outTable += idList[i];
        outTable += "</td>";
        outTable += "<td align=\"center\">";
        outTable += classList[i];
        outTable += "</td>";
        outTable += "</tr>\n";
    }

    outTable += "</table>\n";

    return outTable;
}

// calculte Mahalanobis distances between inList and to means of classes
dwDataSet dwLdaGm::cvaMD(dwRclList & inList)
{
    dwDataSet outDataSet;

    std::vector< QString > outHeader;
    for(unsigned i=0; i<cvCovVec.size(); ++i)
    {
        dwDataSet theDataSet = cvCovVec.at(i);
        QString theString = theDataSet.getId();
        outHeader.push_back(theString);
    }
    outDataSet.setHeader(outHeader);

    std::vector< dwRCoordList > inVec = inList.lst();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theList = inVec[i];
        theList.superimpose(reference);
        dwData inData;
        inData.fromRCoordList(theList);

        dwData scores = cvCoe.CvaScores(inData);
        Mat scoresMat = scores.toCvMatDouble();
        dwData outData;
        QString theId = theList.getId();
        outData.setId(theId);
        for(unsigned i=0; i<cvCovVec.size(); ++i)
        {
            Mat mean1Mat = cvMeans.at(i).toCvMatDouble();
            Mat covmat = cvCovVec.at(i).toCvMatDouble();

            // eigenvalue docomposition
            // PCA of CVA scores
            cv::Mat E, V; // E - eigenvalues, V - coefficients
            cv::eigen(covmat,E,V);
            // convert eigenvalues vector to cov matrix
            // diagonal is from eigenvalues the rest is 0

            // remove pc with negative eigenvalues
            int validDim = 0;
            for(int i = 0; i < E.rows; ++i)
            {
                if(E.at< double >(0,i) > 0.0)
                    ++validDim;
                else
                    break;
            }
            cv::Mat A = Mat::zeros( validDim, validDim, CV_64FC1);
            for(int i = 0; i < validDim; ++i)
            {
                A.at< double >(i,i) = E.at< double >(0,i);
            }
            cv::Mat partV(V.t(), cv::Range(0,V.rows), cv::Range(0,validDim));
            cv::Mat theScores = scoresMat * partV;
            cv::Mat theMean = mean1Mat * partV;
            cv::Mat theIcovar;
            invert(A, theIcovar, DECOMP_SVD);
            double theDist = Mahalanobis(theScores, theMean, theIcovar);
            outData.push_back(theDist);
        }
        outDataSet.push_back(outData);
    }
    return outDataSet;
}

dwDataSet dwLdaGm::cvaProb(dwRclList & inList)
{
    dwDataSet outDataSet;

    std::vector< QString > outHeader;
    for(unsigned i=0; i<cvCovVec.size(); ++i)
    {
        dwDataSet theDataSet = cvCovVec.at(i);
        QString theString = theDataSet.getId();
        outHeader.push_back(theString);
    }
    outDataSet.setHeader(outHeader);

    std::vector< dwRCoordList > inVec = inList.lst();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theList = inVec[i];
        theList.superimpose(reference);
        dwData inData;
        inData.fromRCoordList(theList);
// debug
//        QString inStr = inData.toCsv(); //debug
//        QFile file("e:/wings/test/debug/inData.txt");
//        if (!file.open(QIODevice::WriteOnly)) {
//            std::cerr << "Cannot open file for writing: "
//                      << qPrintable(file.errorString()) << std::endl;
//        }
//        QTextStream out(&file);
//        out << inStr;
//        file.close();
// debug

        dwData scores = cvCoe.CvaScores(inData);
        Mat scoresMat = scores.toCvMatDouble();
        dwData outData;
//        dwData outDistData;
        QString theId = theList.getId();
        outData.setId(theId);
        for(unsigned i=0; i<cvCovVec.size(); ++i)
        {
            Mat mean1Mat = cvMeans.at(i).toCvMatDouble();
            Mat covmat = cvCovVec.at(i).toCvMatDouble();

//            // mahalanobis computed from CVA scores
//            // problem with covmat inverse. It is not symetrical and sometimes MD is negative
//            Mat icovar;
//            invert(covmat, icovar, DECOMP_SVD);
//            double mahDist = Mahalanobis(scoresMat, mean1Mat, icovar);
//            dwChisqr chi;
//            double probability = chi.probability(cvCoe.rows(), mahDist*mahDist);

//            dwDataSet dicovar; //debug
//            dicovar.fromCvMatDouble(icovar); //debug
//            dwDataSet dmean1Mat; //debug
//            dmean1Mat.fromCvMatDouble(mean1Mat); //debug

            // eigenvalue docomposition
            // PCA of CVA scores
            cv::Mat E, V; // E - eigenvalues, V - coefficients
            cv::eigen(covmat,E,V);
            // convert eigenvalues vector to cov matrix
            // diagonal is from eigenvalues the rest is 0

//            // tak only dimensions with fixed fraction of total variance, taking non negative is much better
//            int validDim = 2;
//            double varSum = E.at< double >(0,0) + E.at< double >(0,1);
//            for(int i = 2; i < E.rows; ++i)
//            {
//                if(E.at< double >(0,i) > varSum*0.01)
//                {
//                    varSum += E.at< double >(0,i);
//                    ++validDim;
//                }
//                else
//                    break;
//            }

            // remove pc with negative eigenvalues
            int validDim = 0;
            for(int i = 0; i < E.rows; ++i)
            {
                if(E.at< double >(0,i) > 0.0)
                    ++validDim;
                else
                    break;
            }
            cv::Mat A = Mat::zeros( validDim, validDim, CV_64FC1);
            for(int i = 0; i < validDim; ++i)
            {
                A.at< double >(i,i) = E.at< double >(0,i);
            }
            cv::Mat partV(V.t(), cv::Range(0,V.rows), cv::Range(0,validDim));
            cv::Mat theScores = scoresMat * partV;
            cv::Mat theMean = mean1Mat * partV;
            cv::Mat theIcovar;
            invert(A, theIcovar, DECOMP_SVD);
            double theDist = Mahalanobis(theScores, theMean, theIcovar);
            dwChisqr chi;
            double probability = chi.probability(validDim, theDist*theDist);

//            cv::Mat A = Mat::zeros( E.rows, E.rows, CV_64FC1);
//            for(int i = 0; i < E.rows; ++i)
//            {
//                if(E.at< double >(0,i) > 0.0)
//                    A.at< double >(i,i) = E.at< double >(0,i);
//                else
//                    A.at< double >(i,i) = 0.0; // if variance negative set it to 0
//            }
//            cv::Mat theScores = scoresMat * V.t();
//            cv::Mat theMean = mean1Mat * V.t();

//            cv::Mat theIcovar;
//            invert(A, theIcovar, DECOMP_SVD);
//            double theDist = Mahalanobis(theScores, theMean, theIcovar);
//            dwChisqr chi;
//            double probability = chi.probability(cvCoe.rows(), theDist*theDist);

            outData.push_back(probability);
//            outDistData.push_back(theDist);
        }
        outDataSet.push_back(outData);
//        outDist->push_back(outDistData);

    //    std::cout << scoresMat;
    //    cv::FileStorage storage("test.xml", cv::FileStorage::WRITE);
    //    storage << "scores" << scoresMat;
    //    storage.release();
    }
    return outDataSet;
}

dwDataSet dwLdaGm::cvaDistProb(dwRclList & inList)
{
    dwDataSet outDataSet;

    std::vector< QString > outHeader;
    for(unsigned i=0; i<cvCovVec.size(); ++i)
    {
        dwDataSet theDataSet = cvCovVec.at(i);
        QString theString = theDataSet.getId();
        outHeader.push_back(theString);
    }
    outDataSet.setHeader(outHeader);

    std::vector< dwRCoordList > inVec = inList.lst();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theList = inVec[i];
        theList.superimpose(reference);
        dwData inData;
        inData.fromRCoordList(theList);

        dwData scores = cvCoe.CvaScores(inData);

        dwData outData;
        QString theId = theList.getId();
        outData.setId(theId);
        for(unsigned i=0; i < cvMeans.size(); ++i)
        {
            dwData theMean = cvMeans.at(i);
            dwData distData = cvDist.at(i);
            double eDist = scores.eDistance(theMean);
            double z = (distData.at(0)-eDist)/distData.at(1); // (dist-meanDist)/sd
            double probability = cdfProb(z);
            if(distData.at(0) < eDist)
                probability = 1.0-probability;
            outData.push_back(probability);
        }
        outDataSet.push_back(outData);
    }
    return outDataSet;
}

dwDataSet dwLdaGm::cvaScores(dwRclList & inList)
{
    dwDataSet outDataSet;

    std::vector< QString > outHeader;
//    outHeader.push_back("id");
    for(unsigned i=0; i < cvCoe.rows(); ++i)
    {
        dwData theData = cvCoe.at(i);
        QString theString = theData.getId();
        outHeader.push_back(theString);
    }
    outDataSet.setHeader(outHeader);

//    std::vector< QString > outHeader = cvMeans.getHeader();
////    outHeader[0] = "id";
//    outDataSet.setHeader(outHeader);

    std::vector< dwRCoordList > inVec = inList.lst();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theList = inVec[i];
        theList.superimpose(reference);
        dwData inData;
        inData.fromRCoordList(theList);

        dwData scores = cvCoe.CvaScores(inData);
        outDataSet.push_back(scores);
    }
    return outDataSet;
}

dwDataSet dwLdaGm::ldaScores(dwRclList & inList)
{
    dwDataSet outDataSet;

    std::vector< QString > outHeader;
    for(unsigned i=0; i < cf.rows(); ++i)
    {
        dwData theData = cf.at(i);
        QString theString = theData.getId();
        outHeader.push_back(theString);
    }
    outDataSet.setHeader(outHeader);

    std::vector< dwRCoordList > inVec = inList.lst();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theList = inVec[i];
        theList.superimpose(reference);
        dwData inData;
        inData.fromRCoordList(theList);

        dwData scores = cf.CvaScores(inData);
        outDataSet.push_back(scores);
    }
    return outDataSet;
}

// cvX and cvY represent number of CV for x and y asis respectively
QImage dwLdaGm::cvaDiagram(dwRclList & inList,  unsigned cvX, unsigned cvY)
{
//    unsigned cvX = 0; // cv for x axis -1
//    unsigned cvY = 1; // cv for Y axis -1

    // assume the sime size of all covariance matrices
    unsigned maxSize = cvCovVec.at(0).size();
    if(cvX == cvY || cvX > maxSize || cvY > maxSize)
        return QImage();

    --cvX; // index starts from 0
    --cvY; // index starts from 0

    QGraphicsScene scene;
    QPen pen;
    pen.setWidth(1);
    pen.setCosmetic(true); //do not change width after transformations
    pen.setColor(Qt::black);
    QBrush brush(Qt::black);

//    std::vector< dwRCoordList > inVec = inList.lst();
//    for( unsigned i = 0; i < inVec.size(); ++i)
//    {
//        dwRCoordList theList = inVec[i];
////        theList.flip();//flip only coordinates in bitmap system
//        theList.superimpose(reference);
//        dwData inData;
//        inData.fromRCoordList(theList);

//        //paint data points
//        dwData scores = cvCoe.CvaScores(inData);
////        std::vector< double > scoresVec = scores.getData();
//        if(scores.size() > cvY) //can be better checked
//        {
//            QRectF marker(QPointF(0,0), QPointF(0.1,0.1));
//            QGraphicsRectItem * markerItem = scene.addRect(marker, pen, brush);
////            markerItem->setPos(scoresVec[cvX], scoresVec[cvY]);
//            markerItem->setPos(scores.at(cvX), scores.at(cvY));
//        }
//    }

    std::vector< QColor > color;
    color.push_back(Qt::black);  //1
    color.push_back(Qt::blue);
    color.push_back(Qt::red);
    color.push_back(Qt::green);
    color.push_back(Qt::darkRed);//5
    color.push_back(Qt::darkGreen);
    color.push_back(Qt::darkBlue);
    color.push_back(Qt::cyan);
    color.push_back(Qt::darkCyan);
    color.push_back(Qt::magenta); //10
    color.push_back(Qt::darkMagenta);
    color.push_back(Qt::yellow);
    color.push_back(Qt::darkYellow);
    color.push_back(Qt::gray);
    color.push_back(Qt::darkGray); //15
    color.push_back(Qt::lightGray);

    //calculate ellipse size
    Mat meansMat = cvMeans.toCvMatDouble();
    int cvSize = meansMat.rows;
    std::vector< double > halfmajoraxis(cvSize, 0.0); ///???????
    std::vector< double > halfminoraxis(cvSize, 0.0);
    std::vector< QPointF > mean(cvSize);

    for(int i=0; i<cvSize; ++i)
    {
        cv::Mat covmat(2,2,CV_64FC1); // create 2x2 covariance matrix for desplayed cvs
        dwDataSet covDataSet = cvCovVec.at(i);
        covmat.at< double >(0,0) = covDataSet.valueXY(cvX,cvX);
        covmat.at< double >(0,1) = covDataSet.valueXY(cvX,cvY);
        covmat.at< double >(1,0) = covDataSet.valueXY(cvY,cvX);
        covmat.at< double >(1,1) = covDataSet.valueXY(cvY,cvY);

        QPointF theMean( meansMat.at < double >(i,cvX) , meansMat.at < double >(i,cvY));
        mean[i] = theMean;
        //    getErrorEllipse(double chisquare_val, QPointF mean, cv::Mat covmat){

        //Get the eigenvalues and eigenvectors
        cv::Mat eigenvalues, eigenvectors;
//        cv::eigen(covmat, true, eigenvalues, eigenvectors);
        cv::eigen(covmat, eigenvalues, eigenvectors);

        //debug
//        double eigenVec00 = eigenvectors.at<double>(0,0);
//        double eigenVec01 = eigenvectors.at<double>(0,1);
//        double eigenVec10 = eigenvectors.at<double>(1,0);
//        double eigenVec11 = eigenvectors.at<double>(1,1);
//        double eigenVal0 = eigenvalues.at<double>(0);
//        double eigenVal1 = eigenvalues.at<double>(1);

        //Calculate the angle between the largest eigenvector and the x-axis
        double angleRad = atan2(eigenvectors.at<double>(0,cvY), eigenvectors.at<double>(0,cvX));

        //Convert to from radians to degrees
        double angle = 180.0*angleRad/3.14159265359;

        //Calculate the size of the minor and major axes
        double chisquare_val = 2.4477;
        halfmajoraxis[i]=chisquare_val*sqrt(eigenvalues.at<double>(0));
        halfminoraxis[i]=chisquare_val*sqrt(eigenvalues.at<double>(1));
//        halfmajoraxis[i]=chisquare_val*sqrt(eigenvalues.at<double>(cvX));
//        halfminoraxis[i]=chisquare_val*sqrt(eigenvalues.at<double>(cvY));

        pen.setColor(color[i % 15]);  //modulo - reminder of division
        QPointF topLeft(-halfmajoraxis[i], halfminoraxis[i]);
        QPointF bottomRight(halfmajoraxis[i], -halfminoraxis[i]);
        QRectF rect(topLeft, bottomRight);
        QGraphicsEllipseItem * elipseItem = scene.addEllipse(rect, pen);
        elipseItem->setRotation(angle); //The -angle is used because OpenCV ang Qt rotations is clockwise instead of anti-clockwise
        elipseItem->setPos(mean[i]);
    }

    // get size range of the elipses
    double minX = mean[0].x() - halfmajoraxis[0];
    double maxX = mean[0].x() + halfmajoraxis[0];
    double minY = mean[0].y() - halfmajoraxis[0];
    double maxY = mean[0].y() + halfmajoraxis[0];
    for(int i=1; i<cvSize; ++i)
    {
        double extremum = mean[i].x() - halfmajoraxis[i];
        if( extremum < minX)
            minX = extremum;
        extremum = mean[i].x() + halfmajoraxis[i];
        if( extremum > maxX)
            maxX = extremum;
        extremum = mean[i].y() - halfmajoraxis[i];
        if( extremum < minY)
            minY = extremum;
        extremum = mean[i].y() + halfmajoraxis[i];
        if( extremum > maxY)
            maxY = extremum;
    }
    double frameMargin = 0.05*(maxX - minX);
    minX-=frameMargin;
    maxX+=frameMargin;
    minY-=frameMargin;
    maxY+=frameMargin;
    double sizeX = maxX - minX;
    double sizeY = maxY - minY;
    double sizeMax;
    if(sizeX>sizeY)
        sizeMax = sizeX;
    else
        sizeMax = sizeY;

    double tickSize = 0.0125*sizeMax;
    double tickDistance = sizeMax/10.0;
    double theFactor;
    for(double factor = 0.0001; factor<1000; factor*=10)
    {
        theFactor = factor;
        if(theFactor>tickDistance)
            break;
        theFactor = factor*2.0;
        if(theFactor>tickDistance)
            break;
        theFactor = factor*5.0;
        if(theFactor>tickDistance)
            break;
    }
    int precission = 0;
    if(theFactor > 1.0)
        precission = 0;
    else
        precission = -log(theFactor)-1;

    double txtScale = 0.0025*sizeMax;
    //            textItem->setScale(0.04);
    //            QTransform Transform = textItem->transform();
    //            qreal m11 = 0.04;    // Horizontal scaling
    //            qreal m12 = transform.m12();    // Vertical shearing
    //            qreal m13 = transform.m13();    // Horizontal Projection
    //            qreal m21 = transform.m21();    // Horizontal shearing
    //            qreal m22 = -0.04;    // vertical scaling
    //            // minus sign for vertical flip
    //            qreal m23 = transform.m23();    // Vertical Projection
    //            qreal m31 = transform.m31();    // Horizontal Position (DX)
    //            qreal m32 = transform.m32();    // Vertical Position (DY)
    //            qreal m33 = transform.m33();    // Addtional Projection Factor
    //            transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
//    QTransform txtTransform(0.04, 0.0, 0.0, 0.0, -0.04, 0.0, 0.0, 0.0, 1.0);
    QTransform txtTransform(txtScale, 0.0, 0.0, 0.0, -txtScale, 0.0, 0.0, 0.0, 1.0);

    pen.setColor(Qt::black);

    if( minX<0.0 && maxX>0.0 )
    {
        double nextTick=0.0;
        while(nextTick<maxX)
        {
            QString tickNum;
            tickNum.setNum(nextTick,'f',precission);
            QGraphicsSimpleTextItem * textItem = scene.addSimpleText(tickNum);
            textItem->setPos(nextTick, minY+6*tickSize);
            textItem->setBrush(brush);
            textItem->setTransform(txtTransform);

            QLineF line(QPointF(nextTick,minY), QPointF(nextTick,minY+tickSize));
            scene.addLine(line, pen);
            QLineF line1(QPointF(nextTick,maxY), QPointF(nextTick,maxY-tickSize));
            scene.addLine(line1, pen);
            nextTick+=theFactor;
        }
        nextTick=0.0;
        nextTick-=theFactor;
        while(nextTick>minX)
        {
            QString tickNum;
            tickNum.setNum(nextTick,'f',precission);
            QGraphicsSimpleTextItem * textItem = scene.addSimpleText(tickNum);
            textItem->setPos(nextTick, minY+6*tickSize);
            textItem->setBrush(brush);
            textItem->setTransform(txtTransform);

            QLineF line(QPointF(nextTick,minY), QPointF(nextTick,minY+tickSize));
            scene.addLine(line, pen);
            QLineF line1(QPointF(nextTick,maxY), QPointF(nextTick,maxY-tickSize));
            scene.addLine(line1, pen);
            nextTick-=theFactor;
        }
    }else
    {
        double nextTick = floor(minX*theFactor);
        while(nextTick < minX)
            nextTick+=theFactor;
        while(nextTick<maxX)
        {
            QString tickNum;
            tickNum.setNum(nextTick,'f',precission);
            QGraphicsSimpleTextItem * textItem = scene.addSimpleText(tickNum);
            textItem->setPos(nextTick, minY+6*tickSize);
            textItem->setBrush(brush);
            textItem->setTransform(txtTransform);

            QLineF line(QPointF(nextTick,minY), QPointF(nextTick,minY+tickSize));
            scene.addLine(line, pen);
            QLineF line1(QPointF(nextTick,maxY), QPointF(nextTick,maxY-tickSize));
            scene.addLine(line1, pen);
            nextTick+=theFactor;
        }
    }

    if( minY<0.0 && maxY>0.0 )
    {
        double nextTick=0.0;
        while(nextTick<maxY)
        {
            QString tickNum;
            tickNum.setNum(nextTick,'f',precission);
            QGraphicsSimpleTextItem * textItem = scene.addSimpleText(tickNum);
            textItem->setPos(minX+2*tickSize, nextTick);
            textItem->setBrush(brush);
            textItem->setTransform(txtTransform);

            QLineF line(QPointF(minX, nextTick), QPointF(minX+tickSize, nextTick));
            scene.addLine(line, pen);
            QLineF line1(QPointF(maxX, nextTick), QPointF(maxX-tickSize, nextTick));
            scene.addLine(line1, pen);
            nextTick+=theFactor;
        }
        nextTick=0.0;
        nextTick-=theFactor;
        while(nextTick>minY)
        {
            QString tickNum;
            tickNum.setNum(nextTick,'f',precission);
            QGraphicsSimpleTextItem * textItem = scene.addSimpleText(tickNum);
            textItem->setPos(minX+2*tickSize, nextTick);
            textItem->setBrush(brush);
            textItem->setTransform(txtTransform);

            QLineF line(QPointF(minX, nextTick), QPointF(minX+tickSize, nextTick));
            scene.addLine(line, pen);
            QLineF line1(QPointF(maxX, nextTick), QPointF(maxX-tickSize, nextTick));
            scene.addLine(line1, pen);
            nextTick-=theFactor;
        }
    }else
    {
        double nextTick = floor(minY*theFactor);
        while(nextTick < minY)
            nextTick+=theFactor;
        while(nextTick<maxY)
        {
            QString tickNum;
            tickNum.setNum(nextTick,'f',precission);
            QGraphicsSimpleTextItem * textItem = scene.addSimpleText(tickNum);
            textItem->setPos(minX+2*tickSize, nextTick);
            textItem->setBrush(brush);
            textItem->setTransform(txtTransform);

            QLineF line(QPointF(minX, nextTick), QPointF(minX+tickSize, nextTick));
            scene.addLine(line, pen);
            QLineF line1(QPointF(maxX, nextTick), QPointF(maxX-tickSize, nextTick));
            scene.addLine(line1, pen);
            nextTick+=theFactor;
        }
    }

    /// ellips names
    for(int i=0; i<cvSize; ++i)
    {
        QGraphicsSimpleTextItem * textItem = scene.addSimpleText(cvMeans.at(i).getId());
        QBrush elipseBrush(color[i % 15]);
        textItem->setBrush(elipseBrush);
        textItem->setTransform(txtTransform);

        QRectF bound = textItem-> boundingRect();
//        textItem->setPos(mean[i].x()-bound.width()*txtScale/2.0, mean[i].y());
        textItem->setPos(mean[i].x()-bound.width()*txtScale/2.0, mean[i].y()+bound.height()*txtScale/2.0);
//        textItem->setPos(mean[i]);
    }
    /// ellips names end

/// points start
    std::vector< dwRCoordList > inVec = inList.lst();
    for( unsigned i = 0; i < inVec.size(); ++i)
    {
        dwRCoordList theList = inVec[i];
//        theList.flip();//flip only coordinates in bitmap system
        theList.superimpose(reference);
        dwData inData;
        inData.fromRCoordList(theList);

        //paint data points
        dwData scores = cvCoe.CvaScores(inData);
//        std::vector< double > scoresVec = scores.getData();
        double markerSize = 0.005*sizeMax;
        if(scores.size() > cvY) //can be better checked
        {
            QRectF marker(QPointF(-markerSize,-markerSize), QPointF(markerSize,markerSize));
            QGraphicsRectItem * markerItem = scene.addRect(marker, pen, brush);
//            markerItem->setPos(scoresVec[cvX], scoresVec[cvY]);
            markerItem->setPos(scores.at(cvX), scores.at(cvY));
        }
    }
/// points end

    QRectF sceneFrame(QPointF(minX, minY), QPointF(maxX, maxY));
    scene.addRect(sceneFrame, pen);
    int wdh = 400; // width is fixed
    int hgt = wdh*(maxY-minY)/(maxX-minX); //height is adjusted to width
    QRectF target(QPointF(0, 0), QPointF(wdh, hgt));

    QImage img(wdh, hgt, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);
    scene.render(&painter, target, sceneFrame);

    ///
//    QSvgGenerator generator;
//    generator.setFileName("e:/wings/test/debug/diagramCVA.svg");
//    generator.setSize(QSize(wdh, hgt));
//    generator.setViewBox(QRect(0, 0, wdh, hgt));
//    generator.setTitle("SVG Generated by IdentiFly"); //tr()removed
//    generator.setDescription("SVG Generated by IdentiFly");

//    QPainter painterSVG;
//    painterSVG.begin(&generator);
//    scene.render(&painterSVG, target, sceneFrame);
//    painterSVG.end();
    ///

    return img.mirrored();
}


//void dwLdaGm::svm()
//{
//    dwDataSet dataSet;
//    dataSet.fromCsv("car_cau_mel_aligned.csv");

//    Mat labelsMat = dataSet.id2CvMat();
//    Mat trainingDataMat = dataSet.toCvMatFloat();

////    float v[970];
////    for(int i =0; i< labelsMat.rows; ++i)
////    {
////        float a = labelsMat.at<float>(i,0);
////        v[i] = a;
////    }

//    // Set up SVM's parameters
//    CvSVMParams params;
//    params.svm_type    = CvSVM::NU_SVC;
//    params.nu    = 0.8;
//    params.kernel_type = CvSVM::LINEAR;
//    params.term_crit   = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

//    // Train the SVM
//    CvSVM SVM;
//    SVM.train_auto(trainingDataMat, labelsMat, Mat(), Mat(), params);

//    std::vector< float > results;
//    for(int i = 0; i < trainingDataMat.rows; ++i)
//    {
//        Mat sampleMat = trainingDataMat.row(i);
//        float response = SVM.predict(sampleMat);
//        results.push_back(response);
//    }

//    SVM.save("svmModel.xml", "nameString");
//}

//void dwLdaGm::bayes()
//{
//    dwDataSet dataSet;
//    dataSet.fromCsv("car_cau_mel_aligned.csv");

//    Mat labelsMat = dataSet.id2CvMat();
//    Mat trainingDataMat = dataSet.toCvMatFloat();

//    // Train the SVM
//    CvNormalBayesClassifier classifier;
//    classifier.train(trainingDataMat, labelsMat);

//    std::vector< float > results;
//    for(int i = 0; i < trainingDataMat.rows; ++i)
//    {
//        Mat sampleMat = trainingDataMat.row(i);
//        float response = classifier.predict(sampleMat);
//        results.push_back(response);
//    }

//    classifier.save("bayesModel.xml", "nameString");
//}

dwDataSet dwLdaGm::confusionMat(dwDataSet inData, int idSize)
{
    std::vector< std::vector< int > > outTab;
    for(unsigned i = 0; i < clasName.size(); ++i)
    {
        std::vector< int > outRow(clasName.size(),0);
        outTab.push_back(outRow);
    }

    for(unsigned i = 0; i < inData.rows(); ++i)
    {
        dwData theRow = inData.at(i);
        QString inClas = theRow.getId();
        inClas.truncate(idSize);

        int inClasIndex = -1; // with qstringlist would be better
        for(unsigned j = 0; j < clasName.size(); ++j)
        {
            if(clasName[j]==inClas)
            {
                inClasIndex = j;
                break;
            }
        }
        if(inClasIndex == -1)
            continue; //no class in classification data
        // classify the row
        int theClas = classifyCv(theRow);
//        int theClas = classifyCvDist(theRow);
        ++outTab[inClasIndex][theClas];
    }

    std::vector< QString > outHeader = clasName;
    outHeader.push_back("total");
    outHeader.push_back("correct");

    dwDataSet outData;
//    outData.setHeader(clasName);
    outData.setHeader(outHeader);

    std::vector< int > colTotal(clasName.size(),0);
    int total = 0;
    int correct = 0;
    for(unsigned i = 0; i < clasName.size(); ++i)
    {
        dwData outRow;
        outRow.setId(clasName[i]);
        int rowTotal = 0;
        for(unsigned j = 0; j < clasName.size(); ++j)
        {
            outRow.push_back(outTab[i][j]);
            rowTotal += outTab[i][j];
            colTotal[j] += outTab[i][j];
            total += outTab[i][j];
        }
        outRow.push_back(rowTotal);
        correct += outTab[i][i];
        double rowCorrect = outTab[i][i]/(double)rowTotal;
        outRow.push_back(rowCorrect);
        outData.push_back(outRow);
    }
    dwData outRow;
    outRow.setId("total");
    for(unsigned j = 0; j < clasName.size(); ++j)
    {
        outRow.push_back(colTotal[j]);
    }
    outRow.push_back(total);
    double rowCorrect = (double)correct/(double)total;
    outRow.push_back(rowCorrect);
    outData.push_back(outRow);
    return outData;
}

dwDataSet dwLdaGm::confusionMatLoo(dwDataSet inData, int idSize)
{
    std::vector< std::vector< int > > outTab;
    for(unsigned i = 0; i < clasName.size(); ++i)
    {
        std::vector< int > outRow(clasName.size(),0);
        outTab.push_back(outRow);
    }

    for(unsigned i = 0; i < inData.rows(); ++i)
    {
        dwDataSet theData = inData;
        dwData theRow = theData.removeRow(i);
        QString dwXmlTxt = theData.getDwXml(idSize);
        QDomDocument XmlDoc;
        XmlDoc.setContent(dwXmlTxt);
        fromXml(XmlDoc);

        QString inClas = theRow.getId();
        inClas.truncate(idSize);

        int inClasIndex = -1; // with qstringlist would be better
        for(unsigned j = 0; j < clasName.size(); ++j)
        {
            if(clasName[j]==inClas)
            {
                inClasIndex = j;
                break;
            }
        }
        if(inClasIndex == -1)
            continue; //no class in classification data
        // classify the row
        int theClas = classifyCv(theRow);
//        int theClas = classifyCvDist(theRow);
        ++outTab[inClasIndex][theClas];
    }

    std::vector< QString > outHeader = clasName;
    outHeader.push_back("total");
    outHeader.push_back("correct");

    dwDataSet outData;
//    outData.setHeader(clasName);
    outData.setHeader(outHeader);

    std::vector< int > colTotal(clasName.size(),0);
    int total = 0;
    int correct = 0;
    for(unsigned i = 0; i < clasName.size(); ++i)
    {
        dwData outRow;
        outRow.setId(clasName[i]);
        int rowTotal = 0;
        for(unsigned j = 0; j < clasName.size(); ++j)
        {
            outRow.push_back(outTab[i][j]);
            rowTotal += outTab[i][j];
            colTotal[j] += outTab[i][j];
            total += outTab[i][j];
        }
        outRow.push_back(rowTotal);
        correct += outTab[i][i];
        double rowCorrect = outTab[i][i]/(double)rowTotal;
        outRow.push_back(rowCorrect);
        outData.push_back(outRow);
    }
    dwData outRow;
    outRow.setId("total");
    for(unsigned j = 0; j < clasName.size(); ++j)
    {
        outRow.push_back(colTotal[j]);
    }
    outRow.push_back(total);
    double rowCorrect = (double)correct/(double)total;
    outRow.push_back(rowCorrect);
    outData.push_back(outRow);
    return outData;
}

// calculate Mahalanobis distance between inData and class mean
double dwLdaGm::mahDist(dwData inData, dwData inMean, dwDataSet inCov)
{
    dwData scores = cvCoe.CvaScores(inData);
    Mat scoresMat = scores.toCvMatDouble();

    Mat meanMat = inMean.toCvMatDouble();
    Mat covmat = inCov.toCvMatDouble();

    // problem with covmat inverse. It is not symetrical and sometimes MD is negative
    /// eigenvalue docomposition
    /// PCA of CVA scores
    cv::Mat E, V; // E - eigenvalues, V - coefficients
    cv::eigen(covmat,E,V);
    // convert eigenvalues vector to cov matrix
    // diagonal is from eigenvalues the rest is 0

    // remove pc with negative eigenvalues
    int validDim = 0;
    for(int i = 0; i < E.rows; ++i)
    {
        if(E.at< double >(0,i) > 0.0)
            ++validDim;
        else
            break;
    }
    cv::Mat A = Mat::zeros( validDim, validDim, CV_64FC1);
    for(int i = 0; i < validDim; ++i)
    { // czy jest uzasadnione zmienić offdiagonal na zera
        A.at< double >(i,i) = E.at< double >(0,i);
    }
    cv::Mat partV(V.t(), cv::Range(0,V.rows), cv::Range(0,validDim));
    cv::Mat theScores = scoresMat * partV;
    cv::Mat theMean = meanMat * partV;
    cv::Mat theIcovar;
    invert(A, theIcovar, DECOMP_SVD);
    double theDist = Mahalanobis(theScores, theMean, theIcovar);

    return theDist;
}

    // debug
    //        QString inStr = inData.toCsv(); //debug
    //        QFile file("e:/wings/test/debug/inData.txt");
    //        if (!file.open(QIODevice::WriteOnly)) {
    //            std::cerr << "Cannot open file for writing: "
    //                      << qPrintable(file.errorString()) << std::endl;
    //        }
    //        QTextStream out(&file);
    //        out << inStr;
    //        file.close();
    // debug

//            // tak only dimensions with fixed fraction of total variance, taking non negative is much better
//            int validDim = 2;
//            double varSum = E.at< double >(0,0) + E.at< double >(0,1);
//            for(int i = 2; i < E.rows; ++i)
//            {
//                if(E.at< double >(0,i) > varSum*0.01)
//                {
//                    varSum += E.at< double >(0,i);
//                    ++validDim;
//                }
//                else
//                    break;
//            }

// approximation of the cdf = cumulative distribution function
// of the standard normal distribution
double dwLdaGm::cdfProb(double inVal) const
{
    double outVal = -1;
    // sqrt(pi()/8) = 0.626657069
    outVal = 0.5*(1+sqrt(1-exp(-0.626657069*inVal*inVal)));
    return outVal;
}
