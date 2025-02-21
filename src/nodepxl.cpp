#include "nodepxl.h"

nodePxl::nodePxl(Coord inPixel, nodePxl* inPrev, nodePxl* inNext)
{
    pxl = inPixel;
    prev = inPrev;
    next = inNext;
}

nodePxl* nodePxl::nextAt(int index)
{
    nodePxl* currNode = this;
    for(int i = 0; i < index; ++i)
        currNode = currNode->next;
    return currNode;
}

nodePxl* nodePxl::prevAt(int index)
{
    nodePxl* currNode = this;
    for(int i = 0; i < index; ++i)
        currNode = currNode->prev;
    return currNode;
}

////find a tangent to any point of closed outline,
////consider points in tangentSurround on both sides of a startPxl.
////Arrow of the vector points in the direction from begin to end.
dwVector nodePxl::tangent(int tangentSurround) const
{
    dwVector tangent;
    Coord startCoord = pxl;

    nodePxl* frontPxl = next;
    nodePxl* backPxl = prev;
    for (int pos = 0; pos <= tangentSurround; ++pos)
    {
        Coord front = frontPxl->pxl;
        Coord back = backPxl->pxl;

        dwVector diff1(front, startCoord);
        tangent = tangent + diff1;
        dwVector diff2(startCoord, back);
        tangent = tangent + diff2;

        frontPxl = frontPxl->next;
        backPxl = backPxl->prev;
    }

    return tangent;
}

