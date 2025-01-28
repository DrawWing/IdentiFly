#ifndef NODEPXL_H
#define NODEPXL_H

#include "coord.h"
#include "dwvector.h"

class nodePxl  
{
public:
    nodePxl(Coord inPixel, nodePxl* inPrev, nodePxl* inNext);
    Coord pxl;
    nodePxl* prev;
    nodePxl* next;
    nodePxl* nextAt(int);
    nodePxl* prevAt(int);

    dwVector tangent(int tangentSurround) const; // used only by wing.cpp
};

inline bool operator==(const nodePxl &p1, const nodePxl &p2)
{
    return p1.pxl == p2.pxl && p1.prev == p2.prev && p1.next == p2.next;
}

inline bool operator!=(const nodePxl &p1, const nodePxl &p2)
{
    return p1.pxl != p2.pxl || p1.prev != p2.prev || p1.next != p2.next;
}

#endif // NODEPXL_H
