/*
    Copyright (C) 2026 Adam Tofilski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "dwSLAO.h"

dwSLAO::dwSLAO(dwRclList &inLmkList, dwRclList &inOtlList)
{
    if(inLmkList.size() != inOtlList.size())
        return;
    lmkList = inLmkList;
    otlList = inOtlList;
}

// Align each configuration once to the current mean shape without further
// iterative refinement. This is the single-pass baseline version.
dwRclList dwSLAO::alignOnce()
{
    std::vector< dwRCoordList > lmkVec = lmkList.list();
    std::vector< dwRCoordList > otlVec = otlList.list();

    dwRclList alignedRcl;
    alignedRcl.fromVector(lmkVec);
    dwRCoordList meanOld = alignedRcl.superimposeGPA();

    for(unsigned i = 0; i < lmkVec.size(); ++i)
        alignToOutline(meanOld, lmkVec[i], otlVec[i]);

    dwRclList outRcl;
    outRcl.fromVector(lmkVec);
    return outRcl;
}

// Repeatedly align each configuration to the current mean shape and stop when
// the Procrustes distance between successive means no longer improves.
dwRclList dwSLAO::alignIterate()
{
    std::vector< dwRCoordList > lmkVec = lmkList.list();
    std::vector< dwRCoordList > otlVec = otlList.list();

    dwRclList alignedRcl;
    alignedRcl.fromVector(lmkVec);
    dwRCoordList meanOld = alignedRcl.superimposeGPA();

    const double thd = 0.0001;
    const int maxIter = 100;
    double PDold = 1e308; // max double
    int iter = 0;
    while(iter < maxIter)
    {
        // aling all configurations to mean configuration
        for(unsigned i = 0; i < lmkVec.size(); ++i)
            alignToOutline(meanOld, lmkVec[i], otlVec[i]);

        alignedRcl.fromVector(lmkVec);
        dwRCoordList meanNew = alignedRcl.superimposeGPA();
        double PDnew = meanOld.superimposePart(meanNew);
        if(PDnew >= PDold - thd)
            break;

        PDold = PDnew;
        meanOld = meanNew;
        ++iter;
        qDebug() << PDnew;
    }

    dwRclList outRcl;
    outRcl.fromVector(lmkVec);
    return outRcl;
}

// Move along outline each semilandmark in a configuration in order to minimize distance to reference
void dwSLAO::alignToOutline(const dwRCoordList &inRef, dwRCoordList &inCon, dwRCoordList &inOtl)
{
    dwRCoordList ref = inRef; // create copy of reference
    // center reference
    realCoord centroidRef = ref.centroid();
    ref.translate(centroidRef);
    // scale reference
    double csRef = ref.centroidSize();
    ref.scale(1.0/csRef);

    dwRCoordList scaledRef = ref;
    scaledRef.alignRawRef(inCon);
    double dist0 = inCon.squaredDist(scaledRef);
    for(unsigned i = 0; i < inCon.size(); ++i)
    {
        realCoord nearest = inOtl.findNearest(scaledRef.list()[i]);
        inCon.setCoord(i, nearest);
    }
    double dist = inCon.squaredDist(scaledRef);

    while(dist0 > dist)
    {
        dwRCoordList scaledRef = ref;
        scaledRef.alignRawRef(inCon);
        dist0 = dist;
        for(unsigned i = 0; i < inCon.size(); ++i)
        {
            realCoord nearest = inOtl.findNearest(scaledRef.list()[i]);
            inCon.setCoord(i, nearest);
        }
        dist = inCon.squaredDist(scaledRef);
    }
}
