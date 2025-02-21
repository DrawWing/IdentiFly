#include "dwAdjust.h"
#include "winginfo.h"

dwAdjust::dwAdjust(dwImage & inImg, const dwImage & master)
    // dwAdjust::dwAdjust(dwImage & inImg, const dwImage & master, int inRadius)
{
    WingInfo masterInfo;
    masterInfo.fromImg(master);
    masterPoints = masterInfo.getLandmarks();

//    radius = inRadius;
//    size = radius/4;
    radius = 20;
    size = 20;

//    init(inImg, master);
//}

//void dwAdjust::init(dwImage & inImg, const dwImage & master)
//{

    // tehre is no need to use vector becaue adjusted are alwasys full configurations
//    int lastIndex = masterPoints.size() - 1;
//    dwVector masterVec(masterPoints[0], masterPoints[lastIndex]);
//    double masterDist = coordDistance(masterPoints[0], masterPoints[lastIndex]);

    WingInfo theInfo;
    theInfo.fromImg(inImg);
    thePoints = theInfo.getLandmarks();
//    dwVector theVec(thePoints[0], thePoints[lastIndex]);
//    double theDist = coordDistance(thePoints[0], thePoints[lastIndex]);
//    angle = angle360(masterVec, theVec); //change to superposition
//    scale = theDist/masterDist;

    dwRCoordList masterList(masterPoints);
    dwRCoordList theList(thePoints);

    double mcs = masterList.centroidSize();
    double tcs = theList.centroidSize();
    scale = tcs/mcs;

    masterList.scale(1.0/mcs);
    masterList.center();
    theList.scale(1.0/tcs);
    theList.center();
    angle = theList.rotationAngle(masterList);

    scaled = master.scale(scale);
    scaled.mean(4);

    blured = inImg;
    blured.mean(4); //2

    cornerOfset.setXY(-size, -size);
    cornerOfset.rotate(angle);

#ifdef QT_DEBUG
    master.save("D:/wings/_test/debug/master.png");
    scaled.save("D:/wings/_test/debug/scaled.png");
    blured.save("D:/wings/_test/debug/blured.png");
#endif

}

// in order to use inPolint there is need of rescaling
//std::vector< Coord >  dwAdjust::fitLandmarks( std::vector< Coord > & inPoints)

std::vector< Coord >  dwAdjust::adjusted()
{
    std::vector< Coord > fittedPoints = thePoints;
    for(unsigned i = 0; i < masterPoints.size(); ++i)
    {
        //        realCoord corner(masterPoints[i]);
        //        corner *= scale;
        //        corner += cornerOfset;
        //        masterCut = scaled.crop(corner, 2*size, 2*size, -angle);
        //        meanMaster = masterCut.meanImg();
        //        Coord point = thePoints[i];
        //        //        Coord better = analysePntStep(point);
        //        Coord better = analysePnt(point, 20);
        //        fittedPoints[i] = better;

       fittedPoints[i] = adjustedOne(i);
        // fittedPoints[i] = adjustedFastOne(i);
    }
    return fittedPoints;
}

Coord dwAdjust::adjustedOne(unsigned nr)
{
    realCoord corner(masterPoints[nr]);
    corner *= scale;
    corner += cornerOfset;
    masterCut = scaled.crop(corner, 2*size, 2*size, -angle);
    meanMaster = masterCut.meanImg();
#ifdef QT_DEBUG
    masterCut.save("D:/wings/_test/debug/masterCut.png");
#endif

    Coord point = thePoints[nr];
    Coord better = analysePnt(point, 20); // difference with adjustedFastOne
    return better;
}

Coord dwAdjust::adjustedFastOne(unsigned nr)
{
    realCoord corner(masterPoints[nr]);
    corner *= scale;
    corner += cornerOfset;
    masterCut = scaled.crop(corner, 2*size, 2*size, -angle);
    meanMaster = masterCut.meanImg();
#ifdef QT_DEBUG
    masterCut.save("D:/wings/_test/debug/masterCut.png");
#endif

    Coord point = thePoints[nr];
    Coord better = analysePntStep(point); // difference with adjustedOne
    return better;
}

// finds points with minimum difference
// does not check all pixels in radius (see analysePnt)
Coord dwAdjust::analysePntStep(Coord & thePxl){
    minDiff = 99999999;
    Coord minPxl = thePxl;
    Coord pxl;

    do{
        pxl = minPxl;
        minPxl = findNext(minPxl);
    }while(pxl != minPxl);
    return minPxl;
}

//find if at radius 1 better fit to master image
Coord dwAdjust::findNext(Coord & thePxl){
    Coord minPxl = thePxl;

    for(int i = -1; i < 2; ++i){
        int row = thePxl.dy() + i;
        if(row < 0 || row > blured.height() - 1) continue;
        for(int j = -1; j < 2; ++j){
            int col = thePxl.dx() + j;
            if(col < 0 || col > blured.width() - 1) continue;

            Coord pxl(col, row);
            if(pxl == thePxl) continue;
            int diff = difference(pxl);
            if(diff < minDiff){
                minDiff = diff;
                minPxl = pxl;
            }
        }
    }
    return minPxl;
}

//find if at radius there is better fit to master image
// checks all points in radius
Coord dwAdjust::analysePnt(Coord & thePxl, int radius)
{
    minDiff = 999999999;
    Coord minPxl;

    for(int i = -radius; i < radius + 1; i++){
        int row = thePxl.dy() + i;
        if(row < 0 || row > blured.height() - 1) continue;
        for(int j = -radius; j < radius + 1; j++){
            int col = thePxl.dx() + j;
            if(col < 0 || col > blured.width() - 1) continue;

            Coord pxl(col, row);
            int diff = difference(pxl);
            if(diff < minDiff){
                minDiff = diff;
                minPxl = pxl;
            }
        }
    }
    return minPxl;
}

int dwAdjust::difference(Coord & point)
{
    int x = point.dx() - size;
    int y = point.dy() - size;
//    QImage cropped = blured.copy(x, y, 2*size, 2*size);
//    dwImage test;
//    test = cropped;
    dwImage test = blured.copy(x, y, 2*size, 2*size);

    double testMean = test.meanImg(); //mean image color
    int diff = meanMaster - testMean;
    test.add(diff); //now the two compared images have the same mean
    int imgDiff = test.difference(masterCut);
    return imgDiff;
}

//#ifdef QT_DEBUG
//                tested.save("E:/wings/test/debug/tested.png");
//#endif
