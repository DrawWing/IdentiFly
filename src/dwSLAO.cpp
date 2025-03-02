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

    const double thd = 0.000001;
    double dif = 1;
    while(dif>thd)
    {
        // scale reference to semilandmarks
        dwRCoordList refScaled = ref;
        double csSem = inSem.centroidSize();
        refScaled.scale(csSem);

        // rotate reference
        double angle = refScaled.rotationAngleRaw(inSem);
        refScaled.rotate(angle);

        // translate reference to align with semilandmarks
        realCoord centroidSem = inSem.centroid();
        centroidSem*=-1;
        refScaled.translate(centroidSem);

        dwRCoordList inSemsuper = inSem;
        double procDist0 = inSemsuper.superimposePart(ref);

        for(unsigned i = 0; i < inSem.size(); ++i)
        {
            // QString old2 = inSem.list()[i].toTxt();
            realCoord nearest = inOtl.findNearest(refScaled.list()[i]);
            inSem.setCoord(i, nearest);
            // qDebug() << refScaled.list()[i].toTxt() << old2 << nearest.toTxt();
        }
        inSemsuper = inSem;
        double procDist = inSemsuper.superimposePart(ref);
        dif = procDist0 - procDist;
        // qDebug() << procDist << dif;
    }
}
