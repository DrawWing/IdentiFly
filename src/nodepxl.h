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