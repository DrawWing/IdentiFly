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

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "dwImg.h"

class Histogram
{
public:
    Histogram(const dwImage *const);
    Histogram(const dwImage & inImg, int x, int y, int radius);
    Histogram(unsigned char * inTab, int size);
    Histogram(unsigned inTtab[]);
    int percentile(double proportion); //returns color value at which cumulative histogram exeeds proportion
    void smooth(unsigned range); //smooth the histogram proportionaly to range
    int maxMinMax(); // find minimum between 2 most distant maxima
    void log(std::ofstream& logFile) const;

private:
    unsigned tab[256]; // the histogram values
};

#endif // HISTOGRAM_H