#include "dwSLAO.h"

dwSLAO::dwSLAO(dwRclList &inLmkList, dwRclList &inOtlList)
{
    if(inLmkList.size() != inOtlList.size())
        return;
    lmkList = inLmkList;
    otlList = inOtlList;
}

dwRclList dwSLAO::align()
{
    std::vector< dwRCoordList > lmkVec = lmkList.list();
    std::vector< dwRCoordList > otlVec = otlList.list();

    // aling all configurations to the first one
    for(unsigned i = 1; i < lmkVec.size(); ++i)
        align2(lmkVec[0], lmkVec[i], otlVec[i]);

    dwRclList meanList;
    meanList.fromVector(lmkVec);
    dwRCoordList meanOld = meanList.superimposeGPA();

    const double thd = 0.000001;
    double procDist = 1.0;
    while(procDist > thd)
    {
        // aling all configurations to mean configuration
        for(unsigned i = 0; i < lmkVec.size(); ++i)
            align2(meanOld, lmkVec[i], otlVec[i]);

        meanList.fromVector(lmkVec);
        dwRCoordList meanNew = meanList.superimposeGPA();
        procDist = meanOld.partialProcrustesDistance(meanNew);
        // QString csv = meanNew.toCsv();
        // qDebug() << procDist << ":" << csv;
        meanOld = meanNew;
    }

    dwRclList outRcl;
    outRcl.fromVector(lmkVec);
    return outRcl;
}

void dwSLAO::align2(const dwRCoordList &inRef, dwRCoordList &inSem, dwRCoordList &inOtl)
{
    dwRCoordList ref = inRef; // create copy of reference
    // center reference
    realCoord centroidRef = ref.centroid();
    ref.translate(centroidRef);
    // scale reference
    double csRef = ref.centroidSize();
    ref.scale(1.0/csRef);

    dwRCoordList scaledRef = ref;
    scaledRef.alignRawRef(inSem);
    double dist0 = inSem.squaredDist(scaledRef);
    for(unsigned i = 0; i < inSem.size(); ++i)
    {
        realCoord nearest = inOtl.findNearest(scaledRef.list()[i]);
        inSem.setCoord(i, nearest);
    }
    double dist = inSem.squaredDist(scaledRef);

    while(dist0 > dist)
    {
        dwRCoordList scaledRef = ref;
        scaledRef.alignRawRef(inSem);
        dist0 = dist;
        for(unsigned i = 0; i < inSem.size(); ++i)
        {
            realCoord nearest = inOtl.findNearest(scaledRef.list()[i]);
            inSem.setCoord(i, nearest);
        }
        dist = inSem.squaredDist(scaledRef);
    }
}

