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

// if master is constant : inImg is scaled and rotated
// than fitted points need to be inversly tranformed

#ifndef DWADJUST_H
#define DWADJUST_H

#include "coord.h"
#include "dwImg.h"

#include <vector>

class dwAdjust
{
public:
    dwAdjust(dwImage & inImg, const dwImage & master);
    // dwAdjust(dwImage & inImg, const dwImage & master, int inRadius = 60);
    std::vector< Coord > adjusted();
    Coord adjustedOne(unsigned int nr);
    Coord adjustedFastOne(unsigned int nr);

private:
    //    void init(dwImage & inImg, const dwImage & master);
    int difference(Coord &);
    Coord analysePnt(Coord & thePxl, int radius);
    Coord analysePntStep(Coord & thePxl);
    Coord findNext(Coord & thePxl);

    dwImage inImg;
    dwImage masterImg;
    dwImage scaled;
    dwImage blured;
    dwImage masterCut;

    std::vector< Coord > masterPoints;
    std::vector< Coord > thePoints;
//    std::vector< Coord > inPoints;
    std::vector< Coord > inTransPoints;
    std::vector< Coord > outPoints;

    double scale; // inSize / masterSize
    double angle; // angle between in and master
    int radius; // radius of circle around points used for adjusting
    int size; // half size of crop image; the crop image size = 2x+1

    realCoord cornerOfset;
    double meanMaster;
    int minDiff;

};

#endif // DWADJUST_H