// Class for reading and manipulation configuration of landmarks.
// The operations include Procrustes suprposition
// validSize determines number of coordinates used in superposition,
// other coordinates are translated, scaled and rotated but not superimposed
// number of coord in all lists cannot be smaller than validSize
// All public methods need to compare reference and validSize

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <math.h>

#include "dwRCoordList.h"
#include "winginfo.h"
#include "dwvector.h"

dwRCoordList::dwRCoordList(void)
{
    id = "";
    validSize = 0;
}

dwRCoordList::dwRCoordList(const QString & resources)
{
    id = resources;
    validSize = 0;
    QFile in_file(resources);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&in_file);
    while (!in.atEnd()) {
        double x;
        double y;
        in >> x >> y;
        realCoord pxl(x,y);
        theList.push_back(pxl);
    }
    validSize = theList.size();
}

//New version of reading from file
//Reads name followed by vector of real coordinates.
//Names are in seperate lines. Pairs of X Y coordinates are in seperate lines. 
//consequitive rcl are separated by '<'
dwRCoordList::dwRCoordList(QTextStream & in)
{
    while (!in.atEnd()) {
        QString ln = in.readLine();
        if(ln.isEmpty())
            continue;
        if( ln.at(0) == QChar('<') ){ //start of the next rcl
            validSize = theList.size();
            return;
        }
        if( ln.at(0).isLetter() ){ //name (id) of the rcl
            id = ln;
        }else{
            QStringList lst = ln.split(" ");
            if(lst.size() < 2)
                continue;
            double x = lst.at(0).toDouble();
            double y = lst.at(1).toDouble();
            realCoord pxl(x,y);
            theList.push_back(pxl);
        }
    }
    validSize = theList.size();
}

dwRCoordList::dwRCoordList(const std::vector< Coord > & coords)
{
    id = "";
    std::vector< Coord >::const_iterator iter = coords.begin();
    for(unsigned i = 0; i < coords.size(); ++i, iter++){ // punkt 19 nie moze byc użyty do obliczen
        realCoord pxl(iter->dx(), iter->dy());
        theList.push_back(pxl);
    }
    validSize = theList.size();
}

void dwRCoordList::setList(const std::vector< Coord > & inList)
{
    theList.clear();
    for(unsigned i = 0; i < inList.size(); ++i){
        Coord thePnt = inList[i];
        realCoord pxl(thePnt);
        theList.push_back(pxl);
    }
    validSize = theList.size();
}

dwRCoordList::dwRCoordList(const std::vector< realCoord > & coords)
{
    id = "";
    std::vector< realCoord >::const_iterator iter = coords.begin();
    for(unsigned i = 0; i < coords.size(); ++i, iter++){ // punkt 19 nie moze byc użyty do obliczen
        theList.push_back(*iter);
    }
    validSize = theList.size();
}

//remove iterator as in lower method
void dwRCoordList::fromRealPxls(const std::vector< realCoord > & pixels)
{
    theList.clear();
    id = "";
    std::vector< realCoord >::const_iterator iter = pixels.begin();
    for(unsigned i = 0; i < pixels.size(); ++i, iter++){
        theList.push_back(*iter);
    }
    validSize = theList.size();
}

void dwRCoordList::fromPxls(const std::vector< Coord > & inList)
{
    theList.clear();
    id = "";
    for(unsigned i = 0; i < inList.size(); ++i){
        Coord pxl = inList[i];
        realCoord realPxl(pxl);
        theList.push_back(realPxl);
    }
    validSize = theList.size();
}

void dwRCoordList::fromCsv(const QString &inStr, const QChar sep)
{
    QStringList inList = inStr.split(sep, Qt::SkipEmptyParts);
    if (inList.size() % 2 != 1 || inList.size() == 0)
        return;

    theList.clear();
    id = inList.first();
    id.remove('"');
    inList.removeFirst();

    int i = 0;
    while(i < inList.size())
    {
        QString xStr = inList.at(i);
        xStr.remove('"');
        double x = xStr.toDouble();
        ++i;
        QString yStr = inList.at(i);
        yStr.remove('"');
        double y = yStr.toDouble();
        ++i;
        realCoord realPxl(x,y);
        theList.push_back(realPxl);
    }
    validSize = theList.size();
}

void dwRCoordList::push_back(const realCoord & pxl)
{
    theList.push_back(pxl);
    ++validSize;
}

void dwRCoordList::pop_back()
{
    theList.pop_back();
    --validSize;
}

void dwRCoordList::clear()
{
    theList.clear();
    validSize = 0;
    id = "";

}

std::vector< realCoord > dwRCoordList::list(void) const
{
    return theList;
}

std::vector< Coord > dwRCoordList::coordList(void) const
{
    std::vector< Coord > outList;
    for(unsigned i = 0; i < theList.size(); ++i){
        Coord coordPxl(theList[i].dx(), theList[i].dy());
        outList.push_back(coordPxl);
    }
    return outList;
}

// Calculate centroid of the list of points.
realCoord dwRCoordList::centroid(void) const
{
    double meanX = 0.0;
    double meanY = 0.0;
    for(int i = 0; i < validSize; i++){
        meanX += theList[i].dx();
        meanY += theList[i].dy();
    }
    meanX /= validSize;
    meanY /= validSize;

    realCoord center(meanX, meanY);
    return center;
}

void dwRCoordList::translate(realCoord thePnt)
{
    std::vector< realCoord >::iterator iter;
    for(iter = theList.begin(); iter != theList.end(); ++iter){
        *iter -= thePnt;
    }
}

void dwRCoordList::add(realCoord thePnt)
{
    std::vector< realCoord >::iterator iter;
    for(iter = theList.begin(); iter != theList.end(); ++iter){
        *iter += thePnt;
    }
}

// does no uses validSize!!!!
void dwRCoordList::center()
{
    realCoord centerPnt = centroid();
    //mozna dac translate
    std::vector< realCoord >::iterator iter;
    for(iter = theList.begin(); iter != theList.end(); iter++){
        *iter -= centerPnt;
    }
}

// Find the centroid size.
double dwRCoordList::centroidSize(void) const
{
    realCoord centerPnt = centroid();

    double sum = 0.0;
    for(int i = 0; i < validSize; ++i){
        realCoord centered = theList[i] - centerPnt;
        sum += centered.dx() * centered.dx();
        sum += centered.dy() * centered.dy();
    }

    double size = sqrt(sum);
    return size;
}

//mean distance from centroid to points
double dwRCoordList::meanDistance(void) const
{
    realCoord center = centroid();
    double sum = 0.0;
    for(int i = 0; i < validSize; i++){
        double dist = distance(theList[i], center);
        sum += dist;
    }

    double meanDist = sum / validSize;
    return meanDist;
}

// find angle that minimize distance between 2 configurations
// both this and reference need to be scaled and centered
double dwRCoordList::rotationAngle(const std::vector< realCoord > & reference) const
{
    double numerator = 0.0;
    double denominator = 0.0;
    for(int i = 0; i < validSize; ++i){
        double xr = reference[i].dx();
        double yr = reference[i].dy();
        double xt = theList[i].dx();
        double yt = theList[i].dy();
        double ntemp = yr*xt - xr*yt;
        double dtemp = xr*xt + yr*yt;
        numerator += ntemp;
        denominator += dtemp;
    }
    double angle = atan2(numerator, denominator); // atan crushes
    return angle;
}

// find angle that minimize distance between 2 configurations
// both configurations need to be scaled and centered
double dwRCoordList::rotationAngle(const dwRCoordList & reference) const
{
    const std::vector< realCoord > refVec = reference.list();
    return rotationAngle(refVec);
}

// //find angle between unscaled and untranslated configuratins
// // both this and reference need to be scaled and centered
// double dwRCoordList::rotationAngleRaw(const dwRCoordList & reference) const
// {
//     dwRCoordList ref = reference;
//     ref.center();
//     double refCS = ref.centroidSize();
//     ref.scale(1.0/refCS);

//     dwRCoordList tmp = *this;
//     tmp.center();
//     double tmpCS = tmp.centroidSize();
//     tmp.scale(1.0/tmpCS);

//     return tmp.rotationAngle(ref);
// }

void dwRCoordList::rotate(const double angle)
{
    std::vector< realCoord >::iterator iter;
    for(iter = theList.begin(); iter != theList.end(); iter++){
        iter->rotate(angle);
    }
}

void dwRCoordList::scale(const double factor)
{
    std::vector< realCoord >::iterator iter;
    for(iter = theList.begin(); iter != theList.end(); iter++){
        (*iter) *= factor;
    }
}

double dwRCoordList::squaredDist(const dwRCoordList &reference) const
{
    double distance = 0.0;
    std::vector< realCoord > refVec = reference.list();
    for(int i = 0; i < validSize; ++i)
        distance += pow( refVec[i].dx()-theList[i].dx(), 2.0 ) +
                    pow( refVec[i].dy()-theList[i].dy(), 2.0 );
    return distance;
}

//find Procrustes distance between the 2 configurations
double dwRCoordList::partialProcrustesDistance(const dwRCoordList &reference) const
{
    double distance = squaredDist(reference);
    // double distance = 0.0;
    // std::vector< realCoord > refVec = reference.list();
    // for(int i = 0; i < validSize; i++){
    //     double xr = refVec[i].dx();
    //     double yr = refVec[i].dy();
    //     double xt = theList[i].dx();
    //     double yt = theList[i].dy();
    //     distance+=(xt-xr)*(xt-xr)+(yt-yr)*(yt-yr);
    // }
    return sqrt(distance);
}

double dwRCoordList::procrustesDistance(const dwRCoordList &reference) const
{
    double partial = partialProcrustesDistance(reference);
    if( partial > 2.0 )
        return -1.0;
    double full = 2.0 * asin(partial/2.0);
    return full;
}

// Superimposes partially the configuration over the reference.
// Returns partial procrustes distance.
double dwRCoordList::superimposePart(const dwRCoordList & reference)
{
    if(reference.theList.size() < (unsigned)validSize)
        return -1.0;
    center();
    double cs = centroidSize();
    scale(1.0/cs);
    double angle = rotationAngle(reference.theList);
    rotate(angle);
    return partialProcrustesDistance(reference); //partial procrustes distance
}

// Superimposes partially the configuration over the reference.
// Returns full procrustes distance.
double dwRCoordList::superimpose(const dwRCoordList & reference)
{
    if(reference.theList.size() < (unsigned)validSize)
        return -1.0;
    superimposePart(reference);
    double distance = procrustesDistance(reference.theList); //full procrustes distance
    scale(cos(distance));
    return distance;
}

// superimpose the list over reference by rotation and translation only
void dwRCoordList::superimposeNoScaling(const dwRCoordList & reference)
{
    if(reference.theList.size() < (unsigned)validSize)
        return;
    center();
    dwRCoordList tmpRef = reference;
    tmpRef.center();
    double angle = rotationAngle(tmpRef.theList);
    rotate(angle);
}

// aling with reference and scale to reference size
// aligned configurations is centered and scaled
// reference is unscaled and not centered
void dwRCoordList::alignRawRef(const dwRCoordList & reference)
{
    dwRCoordList refScaled = reference; // copy of const reference
    refScaled.center();
    double csRef = reference.centroidSize();
    refScaled.scale(1.0/csRef);
    double angle = rotationAngle(refScaled);
    rotate(angle);
    scale(csRef);
    realCoord centroidRef = reference.centroid();
    centroidRef*=-1;
    translate(centroidRef);
}

void dwRCoordList::preshape()
{
    center();
    double cs = centroidSize();
    scale(1.0/cs);
}

unsigned dwRCoordList::size(void) const
{
    return theList.size();
}

// assumes cartesian coordinates
QString dwRCoordList::toTps(void) const
{
    QString out = QString("LM=%1\n").arg( theList.size() );
    for(unsigned i=0; i < theList.size(); i++){
        realCoord pixel = theList[i];
        out += QString("%1 %2\n").arg(pixel.dx()).arg(pixel.dy());
    }
    out += "ID=" + id + "\n"; //ID= needs to be at the end of the coordinates
    return out;
}

// assumes cartesian coordinates
QString dwRCoordList::toTxt(void) const
{
    QString out =  id + "\n";
    for(unsigned i=0; i < theList.size(); i++)
    {
        realCoord pixel = theList[i];
        out += QString(" %1 %2\n").arg(pixel.dx(), 10).arg(pixel.dy(),10); //zmienic na pixel.toTxt
    }
    return out;
}

QString dwRCoordList::toCsv(void) const
{
    QString out =  id + ";";
    for(unsigned i=0; i < theList.size(); i++)
    {
        realCoord pixel = theList[i];
        out += QString("%1;%2;").arg(pixel.dx(), 10).arg(pixel.dy(),10); //zmienic na pixel.toTxt
    }
    out += "\n";
    return out;
}

// assumes cartesian coordinates
QString dwRCoordList::toXml(void) const
{
    QString out = QString( "<pointList>\n" );
    out += QString( "<pointListName>%1</pointListName>\n" ).arg( id );
    for(unsigned i=0; i < theList.size(); i++){
        realCoord pixel = theList[i];
        out += QString( "<point pointNo=\"%1\"> <x>%2</x> <y>%3</y> </point>\n" ).arg(i).arg(pixel.dx(), 10).arg(pixel.dy(),10);
    }
    out += "</pointList>\n";
    return out;
}

//nie czyta nazwy pliku i komentarzy
//lepiej gdyby byl konstrktorem 
//dane z pliku dopisywane do konca listy
void dwRCoordList::fromTps(const QString & resources)
{
    QFile in_file(resources);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&in_file);
    QString ln = in.readLine();
    if( ln.startsWith("LM=") ){
        ln.remove(0,3); //first 3 characters
        bool ok;
        int lm = ln.toInt(&ok);
        if(ok){
            for(int i = 0; i < lm; i++){
                double x;
                double y;
                in >> x >> y; //brak kontroli czy cokolwiek czyta
                realCoord pxl(x,y);
                theList.push_back(pxl);
            }
            validSize = theList.size();
        }
    }
}

void dwRCoordList::fromTxt(const QString & resources)
{
    QFile inFile(resources);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    double x;
    double y;
    QTextStream in(&inFile);
    while(!in.atEnd()) {
        in >> x >> y;
        realCoord pxl(x,y);
        theList.push_back(pxl);
    }

    validSize = theList.size();
}

bool dwRCoordList::setValidSize(const int size)
{
    if((unsigned)size > theList.size())
        return false;
    validSize = size;
    return true;
}

void dwRCoordList::setId(const QString & newId)
{
    id = newId;
}

QString dwRCoordList::getId(void) const
{
    return id;
}

void dwRCoordList::setCoord(unsigned coordNo, realCoord inCoord)
{
    theList[coordNo] = inCoord;
}

//Find distances between thisList and ref in landmark at index
double dwRCoordList::find_distance(const dwRCoordList & ref, unsigned index) const
{
    if(ref.size() < index || theList.size() < index)
        return -1.0;

    const realCoord p1 = theList.at(index);
    const realCoord p2 = ref.theList.at(index);
    double dist = distance(p1, p2);
    return dist;
}

//Find distances between landmarks of ref and landmarks of theList. 
std::vector< double > dwRCoordList::find_distances(const dwRCoordList & ref) const
{
    std::vector< double > out_list(ref.size(), 0.0);
    if(ref.size() < theList.size()) return out_list;

    for(unsigned i = 0; i < theList.size(); i++){
        const realCoord p1 = theList.at(i);
        const realCoord p2 = ref.theList.at(i);
        double dist = distance(p1, p2);
        out_list[i]=dist;
    }
    return out_list;
}

//Flip points verticaly for transformation between bitmap and cartesian coordinates
void dwRCoordList::flip(const int height){
    for(unsigned i = 0; i < theList.size(); ++i)
        theList.at(i).setY(height - theList.at(i).dy());
}

//Flip points horizontaly
void dwRCoordList::flipHor(const int width)
{
    for(unsigned i = 0; i < theList.size(); ++i){
        theList.at(i).setX(width - theList.at(i).dx());
    }
}

//Rotate points 90 degree clockwise
void dwRCoordList::rotate90(const int height)
{
    for(unsigned i = 0; i < theList.size(); i++){
        realCoord before = theList[i];
        realCoord after(height-before.dy(), before.dx());
        theList.at(i) = after;
    }
}

dwRCoordList::~dwRCoordList(void)
{
    theList.clear();
}

void dwRCoordList::rotate2reference(const dwRCoordList & reference)
{
    double angle = rotationAngle(reference.theList);
    rotate(angle);
}

// output single list as vector graphics in svg format
QString dwRCoordList::listToSVG() const
{
    QString outString("<g> \n");
    for(unsigned i = 0; i < theList.size(); ++i){
        outString += QString("<circle cx=\"%1\" cy=\"%2\" r=\"1\" fill=\"black\" stroke=\"none\" />\n").arg(theList[i].dx()).arg(theList[i].dy());
    }
    outString += QString("</g> \n");
    return outString;
}

// output single list as pairs of number in single line
QString dwRCoordList::listToTxt() const
{
    QString outString;
    for(unsigned i = 0; i < theList.size(); ++i){
        outString += QString("%1 %2\n").arg(theList[i].dx()).arg(theList[i].dy());
    }
    return outString;
}

// output coordinates as vector graphics in svg format
QString dwRCoordList::toSVG(const QString & fileName, dwImage & image) const
{
    QString outString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> \n");
    outString += QString("<svg \n");
    outString += QString("xmlns:xlink=\"http://www.w3.org/1999/xlink\" \n");
    outString += QString("xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\" \n");
    outString += QString("xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\" \n");
    outString += QString("width=\"%1\"  \n").arg(image.width());
    outString += QString("height=\"%2\" \n").arg(image.height());
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

    outString += listToSVG();

    outString += QString("</g> \n");

    outString += "</svg> \n";
    return outString;
}

// output coord list to image text
// id needs to contain complete image path
void dwRCoordList::toImg() const
{
    dwImage img(id);
    WingInfo info;
    info.fromImg(img);
    info.setLandmarks(theList);
    info.toImg(img);
    img.save(id);
}

// Trace of matix multiplication A*Bt. Bt is transposed B.
double traceABt(const dwRCoordList & a, const dwRCoordList & b)
{
    double sum = 0.0;
    if(a.theList.size() != b.theList.size())
        return sum;
    for(unsigned i = 0; i < a.theList.size(); ++i){
        sum += a.theList[i].dx() * b.theList[i].dx();
        sum += a.theList[i].dy() * b.theList[i].dy();
    }
    return sum;
}

//takes configuration of pixels and finds outliers
std::vector< Coord > dwRCoordList::find_outliers_list(std::vector< Coord > mean) {
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
    std::vector< Coord > outList;
    if(theList.size() != 19 || mean.size() != 19)
        return outList;

    dwRCoordList meanRcl(mean);
    meanRcl.superimpose(theList);
    //superimpose(meanRcl);
    std::vector< double > distList = find_distances(meanRcl);
    for(unsigned i = 0; i < theList.size(); i++){
        if(distList[i] > thd[i]){
            outList.push_back(mean[i]);
        }
    }
    return outList;
}

// Bookstein superimposition of two configurations over the reference.
// First two points are used as baseline
void dwRCoordList::superimposeBC(dwRCoordList & reference)
{
    if(theList.size() < 2)
        return;
    std::vector< realCoord > refList = reference.list();
    if(refList.size() < 2)
        return;
    double refSize = distance(refList[0], refList[1]);

    double size = distance(theList[0], theList[1]);
    scale(refSize/size);

    //    realCoord transPnt = theList[0] - refList[0];
    translate(theList[0]);
    reference.translate(refList[0]);

    dwVector theVec(theList[0], theList[1]);
    dwVector refVec(refList[0], refList[1]);
    double theAngle = angle(refVec, theVec);
    rotate(-theAngle);
}

// find distance between points ot two configurations of different length
double dwRCoordList::distanceTo(const dwRCoordList & reference)
{
    std::vector< realCoord > refList = reference.list();

    // distance to reference
    double distTo = 0.0;
    for(unsigned i = 0; i < theList.size(); ++i){
        //find the closes point
        double minDist = 999999.0;
        for(unsigned j = 0; j < refList.size(); ++j){
            double dist = distance(theList[i], refList[j]);
            if(dist < minDist){
                minDist = dist;
            }
        }
        distTo += minDist;
    }

    return distTo;
}

// find distance between points ot two configurations of different length
double dwRCoordList::differenceVarLgh(const dwRCoordList & reference)
{
    std::vector< realCoord > refList = reference.list();
    // the first 0 and second 1 point is baseline
    // distance from reference
    double distFrom = 0.0;
    for(unsigned i = 2; i < refList.size(); ++i){
        //find the closes point
        double minDist = 999999.0;
        for(unsigned j = 2; j < theList.size(); ++j){
            double dist = distance(theList[j], refList[i]);
            if(dist < minDist){
                minDist = dist;
            }
        }
        distFrom += minDist;
    }
    distFrom /= refList.size();

    // distance to reference
    double distTo = 0.0;
    for(unsigned i = 2; i < theList.size(); ++i){
        //find the closes point
        double minDist = 999999.0;
        for(unsigned j = 2; j < refList.size(); ++j){
            double dist = distance(theList[i], refList[j]);
            if(dist < minDist){
                minDist = dist;
            }
        }
        distTo += minDist;
    }
    distTo /= theList.size();

    double outDif = (distFrom+distTo)/2.0;
    return outDif;
}

//distance between list and image
double dwRCoordList::distance2image(dwImage & inImg, int radius)
{
    double outDist = 0;

    for(unsigned i = 0; i < theList.size(); ++i)
    {
        outDist += inImg.toNeighbour(theList[i], radius);

    }
    return outDist;
}

// return index of point in the list which is closes to inPoint
int dwRCoordList::findCloses(const realCoord & inPoint)
{
    double minDist = 999999.0;
    int minIndex = 0;

    for(unsigned i = 0; i < theList.size(); ++i)
    {
        double dist = distance(theList[i], inPoint);
        if(dist < minDist){
            minDist = dist;
            minIndex = i;
        }

    }
    return minIndex;
}

bool dwRCoordList::isNaN()
{
    for(unsigned i = 0; i < theList.size(); ++i)
    {
        realCoord thePnt = theList[i];
        double x = thePnt.dx();
        double y = thePnt.dy();
        if( (x != x) || (y != y) ){ //is NaN
            return true;
        }
    }
    return false;
}

double dwRCoordList::xMin() const
{
    double min = theList[0].dx();
    for(unsigned i = 1; i < theList.size(); ++i)
    {
        if( theList[i].dx() < min )
            min = theList[i].dx();
    }
    return min;
}

double dwRCoordList::xMax() const
{
    double max = theList[0].dx();
    for(unsigned i = 1; i < theList.size(); ++i)
    {
        if( theList[i].dx() > max )
            max = theList[i].dx();
    }
    return max;
}

double dwRCoordList::yMin() const
{
    double min = theList[0].dy();
    for(unsigned i = 1; i < theList.size(); ++i)
    {
        if( theList[i].dy() < min )
            min = theList[i].dy();
    }
    return min;
}

double dwRCoordList::yMax() const
{
    double max = theList[0].dy();
    for(unsigned i = 1; i < theList.size(); ++i)
    {
        if( theList[i].dy() > max )
            max = theList[i].dy();
    }
    return max;
}

void dwRCoordList::transform(const realCoord corner, const double angle, const double inScale)
{
    translate(corner);
    rotate(angle);
    scale(inScale);
}

// change order from last to first
dwRCoordList dwRCoordList::reverse() const
{
    dwRCoordList v2;
    size_t i = theList.size();
    while (i > 0)
        v2.push_back (theList[--i]);
    return v2;
}

//find in the pixel list one closes to thePxl
realCoord dwRCoordList::findNearest(realCoord toPxl)
{
    double min = theList[0].distanceTo(toPxl);
    realCoord outPxl = theList[0];
    for(unsigned i = 1; i < theList.size(); ++i)
    {
        double distance = theList[i].distanceTo(toPxl);
        if(distance < min)
        {
            min = distance;
            outPxl = theList[i];
        }
    }
    return outPxl;
}

