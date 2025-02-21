#include "winginfo.h"
#include "dwvector.h"
#include <QtGui>

WingInfo::WingInfo(void)
{
    clear();
}

WingInfo::~WingInfo(void)
{
}

void WingInfo::clear()
{
    image = 0;
    landmarks.clear();
    scaleBar.clear();
    sequence.clear();
    fileName = "";
    device = "";
    outline = "";
    outlineThd = 0;
    venationThd = 0;
    outlineIndex = 0;
    scaleRef = 0.0;
    resolution = 0.0;
//    exposureTime = 0;
//    fitSize = 0;
}


// read the wing info from image
void WingInfo::fromImg(const QImage &img)
{
    clear();
    image = &img;

    QString inTxt = img.text("IdentiFly");
    if(inTxt.isEmpty())
    {
        QString inStr;

        QString inPoints = img.text("junctions");
        if(!inPoints.isEmpty())
            landmarks = txt2points(inPoints);

        inStr = img.text("wing_threshold");
        if(!inStr.isEmpty())
            outlineThd = inStr.toUInt();
        inStr = img.text("outline-threshold"); //synonim of wing_threshold
        if(!inStr.isEmpty())
            outlineThd = inStr.toUInt();
        inStr = img.text("threshold1"); //synonim of outline-threshold
        if(!inStr.isEmpty())
            outlineThd = inStr.toUInt();

        QString venationThdStr = img.text("venation_threshold");
        if(!venationThdStr.isEmpty())
            venationThd = venationThdStr.toInt();
        venationThdStr = img.text("threshold2"); //synonim of venation_threshold
        if(!venationThdStr.isEmpty())
            venationThd = venationThdStr.toInt();

        inStr = img.text("resolution");
        if(!inStr.isEmpty())
            resolution = inStr.toDouble();

        inPoints = img.text("scale_bar");
        if(!inPoints.isEmpty())
            scaleBar = txt2points(inPoints);

        inStr = img.text("scale_reference");
        if(!inStr.isEmpty())
            scaleRef = inStr.toDouble();

//        inStr = img.text("exposure_time");
//        if(!inStr.isEmpty())
//            exposureTime = inStr.toInt();

        device = img.text("device");
        outline = img.text("outline");

        inStr = img.text("sequence");
        if(!inStr.isEmpty())
            sequence = txt2intVec(inStr);

//        inStr = img.text("fit_size");
//        if(!inStr.isEmpty())
//            fitSize = inStr.toInt();

        QString outlineIndexStr = img.text("outline_index");
        if(!outlineIndexStr.isEmpty())
            outlineIndex = outlineIndexStr.toInt();
    }
    else
    {
        fromTxt(inTxt);
    }
}

void WingInfo::fromTxt(const QString & inTxt)
{
    QStringList inList = inTxt.split(';', Qt::SkipEmptyParts);
    for(int i = 0; i < inList.size(); ++i)
    {
        QString keyTxt = inList[i];
        QStringList keyList = keyTxt.split(':', Qt::SkipEmptyParts);
        if(keyList.size() < 2)
            continue;
        QString theKey = keyList[0].simplified();

        if(theKey == "landmarks")
            landmarks = txt2points(keyList[1]);
        else if(theKey == "threshold1")
            outlineThd = keyList[1].toUInt();
        else if(theKey == "threshold2")
            venationThd = keyList[1].toUInt();
        else if(theKey == "resolution")
            resolution = keyList[1].toDouble();
        else if(theKey == "scale_bar")
            scaleBar = txt2points(keyList[1]);
        else if(theKey == "scale_reference")
            scaleRef = keyList[1].toDouble();
//        else if(theKey == "exposure_time")
//            exposureTime = keyList[1].toUInt();
        else if(theKey == "device")
            device = keyList[1].simplified();
        else if(theKey == "outline")
            outline = keyList[1].simplified();

        else if(theKey == "sequence")
            sequence = txt2intVec(keyList[1]);
//        else if(theKey == "fit_size")
//            fitSize = keyList[1].toInt();
        else if(theKey == "outline_index")
            outlineIndex = keyList[1].toInt();
    }
}

// write the wing info to image, before file is saved
// new version in one keyword
// keywords are separated with semicolon, between keyword and values colon
void WingInfo::toImg(QImage & img) const
{
    QString outTxt;

    QString value;
    if(landmarks.size() > 0){
        QString value = points2txt(landmarks);
        QString outKey = "landmarks:" + value;
        outTxt += outKey + ";";
    }

    if(outlineThd > 0){ // outline threshold
        QString value = QString::number(outlineThd);
        QString outKey = "threshold1:" + value;
        outTxt += outKey + ";";
    }
    if(venationThd > 0){ // venation threshold
        QString value = QString::number(venationThd);
        QString outKey = "threshold2:" + value;
        outTxt += outKey + ";";
    }

    if(resolution > 0){

        QString value = QString::number(resolution,'f',6);
        QString outKey = "resolution:" + value;
        outTxt += outKey + ";";
    }

    if(hasScaleBar())
    {
        QString value = points2txt(scaleBar);
        QString outKey = "scale_bar:" + value;
        outTxt += outKey + ";";

        value = QString::number(scaleRef);
        outKey = "scale_reference:" + value;
        outTxt += outKey + ";";
    }

    if(!outline.isEmpty())
    {
        QString outKey = "outline:" + outline;
        outTxt += outKey + ";";
    }

//    if(exposureTime > 0)
//    {
//        QString value = QString::number(exposureTime);
//        QString outKey = "exposure_time:" + value;
//        outTxt += outKey + ";";
//    }

    if(!device.isEmpty())
    {
        QString outKey = "device:" + device;
        outTxt += outKey + ";";
    }

    if(sequence.size() > 0)
    {
        QString value = intVec2txt(sequence);
        QString outKey = "sequence:" + value;
        outTxt += outKey + ";";
    }

//    if(fitSize > 0)
//    {
//        QString value = QString::number(fitSize);
//        QString outKey = "fit_size:" + value;
//        outTxt += outKey + ";";
//    }

    if(outlineIndex > 0)
    {
        QString value = QString::number(outlineIndex);
        QString outKey = "outline_index:" + value;
        outTxt += outKey + ";";
    }

    img.setText("IdentiFly", outTxt);
}

dwRCoordList WingInfo::toCoordList() const
{
    dwRCoordList outList(landmarks);
    outList.setId(fileName);
    return outList;
}

QString WingInfo::toTxt() const
{
    int imgHeight = image->height();

    QString data = "file:\t" + fileName + "\n";
    data += "\n";
    data += "landmark\tx\ty\n";
    for(unsigned i=0; i < landmarks.size(); i++){
        Coord pixel = landmarks[i];
        QString s = QString("%1\t%2\t%3\n").arg(i+1).arg(pixel.dx()).arg(imgHeight - pixel.dy());
//        s.asprintf("%3d\t%3d\t%3d\n",i+1, pixel.dx(), imgHeight - pixel.dy());
        data+=s;
    }
    data += "\n";
    return data;
}

QString WingInfo::XmlExportHeader(QDomDocument XmlDoc) const
{
    QString outString = "file";
    outString += ";";

    QDomElement docElem = XmlDoc.namedItem("export").toElement();
    if ( docElem.isNull() ) {
      qWarning() << "No <export> element found at the top-level "
                 << "of the XML file!";
    }

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            QString tag = e.tagName();
            if( tag == "distance"){
                outString += "distance ";
                QString fromString = e.attribute("from_landmark");
                QString toString = e.attribute("to_landmark");

                outString += fromString;
                outString += "-";
                outString += toString;
                outString += ";";
            }
            else if( tag == "angle"){
                outString += "angle ";
                QString aString = e.attribute("a");
                QString bString = e.attribute("b");
                QString cString = e.attribute("c");

                outString += aString;
                outString += "-";
                outString += bString;
                outString += "-";
                outString += cString;
                outString += ";";
            }

        }
        n = n.nextSibling();
    }

    outString += "\n";
    return outString;
}

// csv export based on xml
QString WingInfo::XmlExport(QDomDocument XmlDoc) const
{
    QString outString = fileName;
    outString += ";";

    QDomElement docElem = XmlDoc.namedItem("export").toElement();
    if ( docElem.isNull() ) {
      qWarning() << "No <export> element found at the top-level "
                 << "of the XML file!";
    }

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            QString tag = e.tagName();
            if( tag == "distance"){
                QString fromString = e.attribute("from_landmark");
                int from = fromString.toInt();
                QString toString = e.attribute("to_landmark");
                int to = toString.toInt();

                int theSize = landmarks.size(); // index starts with 1 no need to subtruct 1
                if( from > theSize  || to > theSize ){
                    qWarning() << "Attribute out of range.";
//                    continue;
                    break;
                }

                double value = distance1(from, to);
                QString valueString;
                valueString.setNum(value,'f',6);
                outString += valueString;
                outString += ";";
            }
            else if( tag == "angle"){
                QString aString = e.attribute("a");
                int a = aString.toInt();
                QString bString = e.attribute("b");
                int b = bString.toInt();
                QString cString = e.attribute("c");
                int c = cString.toInt();

                int theSize = landmarks.size(); // index starts with 1 no need to subtruct 1
                if( a > theSize  || b > theSize || c > theSize ){
                    qWarning() << "Attribute out of range.";
//                    continue;
                    break;
                }

                double value = degAngle1(a, b, c);
                QString valueString;
                valueString.setNum(value,'f',6);
                outString += valueString;
                outString += ";";
            }

        }
        n = n.nextSibling();
    }

    outString += "\n";
    return outString;
}

// Reconfiguration of landmarks based on xml
void WingInfo::XmlReconfiguration(QDomDocument XmlDoc)
{
    QDomElement docElem = XmlDoc.namedItem("reconfiguration").toElement();
    if ( docElem.isNull() ) {
      qWarning() << "No <reconfiguration> element found at the top-level "
                 << "of the XML file!";
    }

    std::vector< Coord > oldLandmarks = landmarks;

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            QString tag = e.tagName();
            if( tag == "move"){
                QString fromString = e.attribute("from");
                int from = fromString.toInt()-1;// array starts from 0 and numbering from 1
                QString toString = e.attribute("to");
                int to = toString.toInt()-1;// array starts from 0 and numbering from 1

                int oldSize = oldLandmarks.size() - 1;
                int newSize = landmarks.size() - 1;
                if( from < 0  || to < 0  || from > oldSize  || to > newSize ){
                    qWarning() << "Attribute out of range.";
                    break;
                }
                landmarks[to] = oldLandmarks[from];
            }
            // add needs to be done first
            if( tag == "add"){
                QString nString = e.attribute("n");
                int n = nString.toInt();// array starts from 0 and numbering from 1
                Coord empty(-1, -1);
                for(int i = 0; i < n; ++i)
                    landmarks.push_back(empty);
            }
            //remove needs to be in decreasing order otherwise wrong index is removed
            if( tag == "remove"){
                QString fromString = e.attribute("from");
                int from = fromString.toInt()-1;// array starts from 0 and numbering from 1

                int newSize = landmarks.size() - 1; // array starts from 0
                if( from < 0  || from > newSize ){
                    qWarning() << "Attribute out of range.";
                    break;
                }
                landmarks.erase(landmarks.begin()+from);
            }
        }
        n = n.nextSibling();
    }
}

// Reconfiguration of landmarks based on xml
void WingInfo::XmlReconfiguration1(QDomDocument XmlDoc)
{

    QDomElement docElem = XmlDoc.namedItem("reconfiguration").toElement();
    if ( docElem.isNull() ) {
      qWarning() << "No <reconfiguration> element found at the top-level "
                 << "of the XML file!";
    }

    std::vector< Coord > oldLandmarks = landmarks;
    landmarks.clear();
    int count = 0;
    int oldSize = oldLandmarks.size() - 1;

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            QString tag = e.tagName();
            if( tag == "move"){
                QString toString = e.attribute("to");
                int to = toString.toInt()-1;// array starts from 0 and numbering from 1
                if(to!=count)
                    continue;
                QString fromString = e.attribute("from");
                int from = fromString.toInt()-1;// array starts from 0 and numbering from 1

                if( from < 0  || from > oldSize ){
                    qWarning() << "Attribute out of range.";
                    continue;
                }
                landmarks.push_back(oldLandmarks[from]);
                ++count;
            }
        }
        n = n.nextSibling();
    }
}

QString WingInfo::apisDistances() const
{
    QString outString;

    double value;
    QString valueString;

    value = distance1(1,2);
    valueString.setNum(value,'f',6);
    outString+=valueString;
    outString+=";";

    value = distance1(1,3);
    valueString.setNum(value,'f',6);
    outString+=valueString;
    outString+=";";

    value = distance1(2,4);
    valueString.setNum(value,'f',6);
    outString+=valueString;
    outString+=";";

    value = distance1(2,5);
    valueString.setNum(value,'f',6);
    outString+=valueString;
    outString+=";";

    outString+="\n";
    return outString;
}

// height = img.height coordinates in tps are cartesian not bmp.  
QString WingInfo::toTps() const
{
    QString str("LM=");
    QString str_num = QString("%1\n").arg(landmarks.size());
//    str_num.asprintf("%d\n", landmarks.size());
	str+=str_num;
    int imgHeight = image->height();
    for(unsigned i=0; i < landmarks.size(); i++){
        Coord pixel = landmarks[i];
        QString s = QString("%1 %2\n").arg(pixel.dx()).arg(imgHeight - pixel.dy());
//        s.asprintf("%d %d\n", pixel.dx(), imgHeight - pixel.dy());
		str+=s;
	}
    str+="IMAGE="+fileName+"\n";
    if(resolution > 0.0)
    {
        double scale = 1.0/resolution;
        QString scaleStr = QString::number(scale,'g',6);
        str+="SCALE="+scaleStr+"\n";
    }

//    if(!outline.isEmpty())
//    {
//        QString outlineStr;
//        QStringList strLst = outline.split(" ");
//        if(strLst.size() != 2)
//            return str;

//        QString coordStr = strLst.at(0);
//        Coord pxl(-9999,-9999);
//        pxl.fromTxt(coordStr);

//        QString chainStr = strLst.at(1);

//        QString strNum;
//        outlineStr+="OUTLINES=1\n";
//        strNum.setNum(chainStr.size());
//        outlineStr+="CHAIN="+strNum+"\n";

//        strNum.setNum(pxl.dx());
//        outlineStr+=strNum+" ";
//        strNum.setNum(pxl.dy());
//        outlineStr+=strNum+"\n";

////        outlineStr+=chainStr+"\n";
//        while(chainStr.size())
//        {
//            QString subStr = chainStr.left(100);
//            outlineStr+=subStr+"\n";
//            chainStr.remove(0, 100);
//        }

//        if(strLst.size() == 2 && pxl.dx()!=-9999 && pxl.dx()!=-9999 )
//            str+=outlineStr;
//    }
	return str;
}

QString
WingInfo::toCsvHead() const
{
    QString str;
    str = "\"file\"";
    for(unsigned i=0; i < landmarks.size(); i++){
        QString s = QString(";\"x%1\";\"y%2\"").arg(i+1).arg(i+1);
//        s.asprintf(";\"x%d\";\"y%d\"", i+1, i+1);
        str+=s;
    }
    str+="\n";
    return str;
}

// ??? check output
QString WingInfo::toTxtHead() const
{
    QString str;
    str = "file";
    for(unsigned i=0; i < landmarks.size(); i++){
        QString s = QString("\tx%1\ty%2").arg(i+1).arg(i+1);
//        s.asprintf("\tx%d\ty%d", i+1, i+1);
        str+=s;
    }
    str+="\n";
    return str;
}

QString
WingInfo::toCsv() const
{
//    QString str;
    QString str = toCsvHead();
    str += toCsvRow();
//    str = str + "\"" + fileName + "\"";
//	for(unsigned i=0; i < landmarks.size(); i++){
//		Coord pixel = landmarks[i];
//		QString s;
//        s.sprintf(";\"%3d\";\"%3d\"", pixel.dx(), pixel.dy());
//        str+=s;
//	}
//	str+="\n";
	return str;
}

QString WingInfo::toCsvRow() const
{
    QString str;
    int imgHeight = image->height();
    str = str + "\"" + fileName + "\"";
    for(unsigned i=0; i < landmarks.size(); i++){
        Coord pixel = landmarks[i];
        QString s = QString(";\"%1\";\"%2\"").arg(pixel.dx()).arg(imgHeight - pixel.dy());
//        s.asprintf(";\"%3d\";\"%3d\"", pixel.dx(), imgHeight - pixel.dy());
        str+=s;
    }
    str+="\n";
    return str;
}

QString WingInfo::toTxtRow() const
{
    QString str;
    int imgHeight = image->height();
    str = str + fileName;
    for(unsigned i=0; i < landmarks.size(); i++){
        Coord pixel = landmarks[i];
        QString s = QString("\t%1\t%2").arg(pixel.dx()).arg(imgHeight - pixel.dy());
//        s.asprintf("\t%3d\t%3d", pixel.dx(), imgHeight - pixel.dy());
        str+=s;
    }
    str+="\n";
    return str;
}

void WingInfo::setLandmarks(std::vector< Coord > & newJ)
{
    landmarks.clear();
	std::vector< Coord >::iterator iter;
	for(iter = newJ.begin(); iter != newJ.end(); iter++){
        landmarks.push_back(*iter);
	}
}

void WingInfo::setLandmarks( const std::vector< realCoord > & newJ)
{
    landmarks.clear();
    std::vector< realCoord >::const_iterator iter;
    for(iter = newJ.begin(); iter != newJ.end(); iter++){
        Coord pxl(iter->dx(),iter->dy());
        landmarks.push_back(pxl);
    }
}

void WingInfo::setCoordList(const dwRCoordList & inList)
{
//    std::vector< realCoord > inVec = inList.list();
    setLandmarks(inList.list());
}

//obecnie bez sciezki
void WingInfo::setFileName(QString & fName)
{
    fileName = fName;
}

//number of camera used to obtain the image
void WingInfo::setDevice(QString & inDevice)
{
    device = inDevice;
}

//number of camera used to obtain the image
void WingInfo::setOutline(QString inOutline)
{
    outline = inOutline;
}

unsigned WingInfo::getOutlineThd()const
{
    return outlineThd;
}

QString WingInfo::getDevice()const
{
    return device;
}

QString WingInfo::getOutline() const
{
    return outline;
}

//Convert list of points to text string for image text.
QString WingInfo::points2txt(std::vector< Coord > inPoints) const
{
    QString str;
    for(unsigned i = 0; i < inPoints.size(); ++i){
        Coord pxl = inPoints[i];
        QString s = QString("%1 %2 ").arg(pxl.dx()).arg(pxl.dy());
//        s.asprintf("%3d %3d ", pxl.dx(), pxl.dy());
        str+=s;
    }
    str.remove(str.size()-1, 1); //remove the final character
    return str;
}

QString WingInfo::intVec2txt(std::vector< int > inPoints) const
{
    QString str;
    for(unsigned i = 0; i < inPoints.size(); ++i)
    {
        str += QString::number(inPoints[i]) + " ";
    }
    str.remove(str.size()-1, 1); //remove the final character
    return str;
}

std::vector< Coord > WingInfo::txt2points(QString & inTxt) const
{
    std::vector< Coord > outPoints;
    QStringList inList = inTxt.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts); // separated by whithe spaces
    int i = 0;
    while(i < inList.size())
    {
        int x = inList[i].toInt();
        ++i;
        if( i == inList.size())
            break;
        int y = inList[i].toInt();
        ++i;
        Coord pxl(x, y);
        outPoints.push_back(pxl);
    }
    return outPoints;
}

std::vector< int > WingInfo::txt2intVec(QString & inTxt) const
{
    std::vector< int > outPoints;
    QStringList inList = inTxt.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts); // separated by whithe spaces
    for(int i = 0; i < inList.size(); ++i)
    {
        int x = inList[i].toInt();
        outPoints.push_back(x);
    }
    return outPoints;
}

std::vector< Coord > WingInfo::getLandmarks() const
{
    return landmarks;
}

void WingInfo::flipHor()
{
    for(unsigned i = 0; i < landmarks.size(); ++i)
    {
        landmarks.at(i).setX(image->width() - landmarks.at(i).dx());
    }
}

void WingInfo::flipVer()
{
    for(unsigned i = 0; i < landmarks.size(); ++i)
    {
        landmarks.at(i).setY(image->height() - landmarks.at(i).dy());
    }
}

//Rotate points 90 degree clockwise
void WingInfo::rotate()
{
    for(unsigned i = 0; i < landmarks.size(); ++i)
    {
        Coord before = landmarks[i];
        Coord after(image->height()-before.dy(), before.dx());
        landmarks.at(i) = after;
    }
}

void WingInfo::scale(double factor)
{
    for(unsigned i = 0; i < landmarks.size(); ++i)
    {
        landmarks[i] *= factor;
    }

    resolution *= factor;
}

double WingInfo::getScaleReference() const
{
    return scaleRef;
}

std::vector< Coord > WingInfo::getScaleBar() const
{
    return scaleBar;
}

void WingInfo::setScaleBar(std::vector< Coord > inVector, double inValue)
{
    scaleBar = inVector;
    scaleRef = inValue;
    double distance = coordDistance(scaleBar[0], scaleBar[1]);
    resolution = distance/scaleRef;
}

void WingInfo::clearScaleBar()
{
    scaleBar.clear();
    scaleRef = 0.0;
    resolution = 0.0;
}

bool WingInfo::hasScaleBar() const
{
    if(scaleRef > 0)
        return true;
    else
        return false;
}

// get all text associated with an image
QString WingInfo::getImageText(dwImage inImg) const
{
    QString outTxt = inImg.text();
    return outTxt;
}

void WingInfo::setResolution(double inValue)
{
    resolution = inValue;
}

double WingInfo::getResolution() const
{
    return resolution;
}

std::vector< int > WingInfo::getSequence() const
{
    return sequence;
}

//calculate cubital index
double WingInfo::cubitalIndex() const
{
    if(landmarks.size() < 4)
		return 999.0;
    dwVector vecA(landmarks[1], landmarks[3]);
	double a = vecA.magnitude();
    dwVector vecB(landmarks[0], landmarks[1]);
	double b = vecB.magnitude();
	if(a==0.0 || b == 0.0) return 999.0;
	return a/b;
}

//calculate precubital index
double WingInfo::precubitalIndex() const
{
    if(landmarks.size() < 10)
		return 999.0;
    dwVector vecE(landmarks[3], landmarks[8]);
	double e = vecE.magnitude();
    dwVector vecF(landmarks[7], landmarks[9]);
	double f = vecF.magnitude();
	if(e==0.0 || f == 0.0) return 999.0;
	return e/f;
}

//calculate Hantel index
double WingInfo::hantelIndex() const
{
    if(landmarks.size() < 6)
		return 999.0;
    dwVector vecG(landmarks[0], landmarks[3]);
	double h = vecG.magnitude();
    dwVector vecH(landmarks[2], landmarks[5]);
	double g = vecH.magnitude();
	if(g == 0.0 || h==0.0) return 999.0;
	return h/g;
}

//calculate discoidal shift angle
double WingInfo::discoidalShift() const
{
    if(landmarks.size() < 19)
		return 999.0;
	//prosta  6-18
    int a1 = landmarks[18].dy()-landmarks[6].dy();
    int b1 = landmarks[6].dx()-landmarks[18].dx();
    int c1 = a1*landmarks[6].dx() + b1*landmarks[6].dy();
	//prosta prostopadla do 6-18 przechodząca przez punkt 2
	int a2 = -b1;
	int b2 = a1;
    int c2 = a2*landmarks[2].dx() + b2*landmarks[2].dy();
	//punkt przeciecia
	double det = a1*b2-a2*b1;
	if (det == 0)
		return 999.0;
	double x = (b2*c1 - b1*c2)/det;
    double y = (a1*c2 - a2*c1)/det;
	realCoord crossing(x, y);
    realCoord p2(landmarks[2]);
    realCoord p4(landmarks[4]);

	dwVector vec2(p2, crossing);
	dwVector vec4(p4, crossing);
	return angle360(vec4, vec2)*180/M_PI;
}

// calculate distance between 2 landmarks of indexes A and B
// landmarks indexes start from 0
double WingInfo::distance(int indexA, int indexB) const
{
    dwVector vec(landmarks[indexA], landmarks[indexB]);
    double dist = vec.magnitude();
    return dist;
}

// calculate distance between 2 landmarks of indexes A and B
// landmarks indexes start from 1
double WingInfo::distance1(int indexA, int indexB) const
{
    dwVector vec(landmarks[indexA-1], landmarks[indexB-1]);
    double dist = vec.magnitude();
    if(resolution > 0)
        dist/=resolution;
    return dist;
}

// calculate angle between 3 landmarks of indexes A, B and C
// landmarks indexes start from 1
double WingInfo::degAngle1(int indexA, int indexB, int indexC) const
{
    dwVector vec1(landmarks[indexB-1], landmarks[indexA-1]);
    dwVector vec2(landmarks[indexB-1], landmarks[indexC-1]);
    double radAngle = angle(vec1,vec2);
    return radAngle*180/M_PI;
}
