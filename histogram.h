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
