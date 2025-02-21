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
        align(lmkVec[0], lmkVec[i], otlVec[i]);

    dwRclList meanList;
    meanList.fromVector(lmkVec);
    dwRCoordList meanOld = meanList.superimposeGPA();

    const double thd = 0.00001;
    double procDist = 1.0;
    while(procDist > thd)
    {
        // aling all configurations to mean configuration
        for(unsigned i = 0; i < lmkVec.size(); ++i)
            align(meanOld, lmkVec[i], otlVec[i]);

        meanList.fromVector(lmkVec);
        dwRCoordList meanNew = meanList.superimposeGPA();
        procDist = meanOld.partialProcrustesDistance(meanNew);
        QString csv = meanNew.toCsv();
        qDebug() << procDist << ":" << csv;
        meanOld = meanNew;
    }

    dwRclList outRcl;
    outRcl.fromVector(lmkVec);
    return outRcl;
}

void dwSLAO::align(const dwRCoordList &inRef, dwRCoordList &inLmk, dwRCoordList &inOtl)
{
    dwRCoordList ref = inRef;
    std::vector< realCoord > landmarks2 = inLmk.list();
    const double thd = 0.000001;
    double dif = 1;
    while(dif>thd)
    {
        realCoord centroid1 = ref.centroid();
        ref.translate(centroid1);

        double cs1 = ref.centroidSize();
        double cs2 = inLmk.centroidSize();
        ref.scale(cs2/cs1);

        double angle = ref.rotationAngleRaw(inLmk);
        ref.rotate(angle);

        realCoord centroid2 = inLmk.centroid();
        realCoord centroid2neg = centroid2;
        centroid2neg*=-1;
        ref.translate(centroid2neg);

        dwRCoordList refScaled = ref;
        refScaled.translate(centroid1);
        refScaled.scale(1.0/cs1);

        dwRCoordList inLmksuper = inLmk;
        double procDist0 = inLmksuper.superimposePart(refScaled);

        for(unsigned i = 0; i < landmarks2.size(); ++i)
        {
            // QString old2 = inLmk.list()[i].toTxt();
            realCoord nearest = inOtl.findNearest(ref.list()[i]);
            inLmk.setCoord(i, nearest);
            // qDebug() << ref.list()[i].toTxt() << old2 << nearest.toTxt();
        }
        inLmksuper = inLmk;
        double procDist = inLmksuper.superimposePart(refScaled);
        dif = procDist0 - procDist;
        // qDebug() << procDist << dif;
    }
}

// dwRclList dwSLAO::getLandmarks() const
// {
//     return lmkList;
// }
