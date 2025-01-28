#include "histogram.h"

Histogram::Histogram(const dwImage * const img)
{
    for (int color = 0; color < 256; color++){
        tab[color]=0;
    }
    // int mgr = img->getMargin();
    if(!img->isGrayTable())
    {
        dwImage grayImg = img->convertToGray8();
        for (int row = 0; row < grayImg.height(); row++){
            for (int col = 0; col < grayImg.width(); col++)
                tab[*(grayImg.scanLine(row) + col)]++;
        }
    } else {
        for (int row = 0; row < img->height(); row++){
            for (int col = 0; col < img->width(); col++)
                tab[*(img->scanLine(row) + col)]++;
        }
    }
}

Histogram::Histogram(const dwImage & inImg, int x, int y, int radius)
{
    for (int color = 0; color < 256; color++){
        tab[color]=0;
    }

    for(int i = -radius; i < radius + 1; i++){
        int row = y + i;
        if(row < 0 || row > inImg.height() - 1) continue;
        for(int j = -radius; j < radius + 1; j++){
            int col = x + j;
            if(col < 0 || col > inImg.width() - 1) continue;

            int color = *(inImg.scanLine(row) + col);
            if(color == 255) continue;

            tab[color]++;
        }
    }
}

Histogram::Histogram(unsigned char * inTab, int size)
{
	for (int color = 0; color < 256; color++){
		tab[color]=0;
	}
	//pomin brzeg!
	for (int i = 0; i < size; i++){
		tab[*inTab]++;
		inTab++;
	}
}

// inTab musi miec dlugosc 256
Histogram::Histogram(unsigned inTab[])
{
	for(int i = 0; i < 256; i++){
		tab[i] = inTab[i];
	}
}

// pomija kolor bialy 255
int Histogram::percentile(double proportion){
	if(proportion > 1.0 || proportion < 0.0)
		return -1;
	unsigned total = 0;
	for(int i = 0; i < 255; i++){ //without white 255
		total += tab[i];
	}
	unsigned thd = total*proportion;
	total = 0;
	for(int i = 0; i < 256; i++){
		total += tab[i];
		if(total > thd)
			return i;
	}
	return -1; // should never happen
}

void Histogram::smooth(const unsigned range)
{
    unsigned validMin = 0;
    for (int color = 0; color < 256; color++)
    {
        if(tab[color] == 0)
            validMin = color;
        else
            break;
    }
    if(tab[0] == 0)
        validMin++;

    unsigned validMax = 255;
    for (int color = 255; color > 0; color--) //validMax = 0 is not possible
    {
        if(tab[color] == 0)
            validMax = color;
        else
            break;
    }
    //validMax--; //always smaller than 255

    unsigned tmp_tab[256];
    for (int color = 0; color < 256; color++)
    {
        tmp_tab[color] = tab[color];
        tab[color] = 0;
    }

    //analyse left end of histogram
    for (unsigned color = validMin; color < validMin + range; ++color){
//        if( (color < 0) || (color > 255) )
//            return; //error
        for(unsigned i = validMin; i < color+range+1; i++){
            tab[color] += tmp_tab[i];
        }
        //		int tmp = color+range+1-validMin;
        tab[color] /= color+range+1-validMin;
    }
    //analyse middle of histogram
    for (unsigned color = validMin+range; color < validMax-range; color++){
//        if( (color < 0) || (color > 255) )
//            return; //error
        int intRange = range;
        for(int i = -intRange; i < intRange+1; i++){
            tab[color] += tmp_tab[color+i];
        }
        tab[color] /= 2*range+1;
    }
    //analyse right end of histogram
    //pomin hist_tab[255]
    for (unsigned color = validMax-range; color < validMax; color++){
        if( color > 255 )
            return; //error
        for(unsigned i = color-range; i < validMax; i++){
            tab[color] += tmp_tab[i];
        }
        //		int tmp = validMax-color+range;
        tab[color] /= validMax-color+range;
    }
}

// find minimum between 2 most distant maxima
// if no maxima was found 0 is returned
// white 255 is ignored
// no maximum is found if there are 2 identical maximum values
int Histogram::maxMinMax(){
    unsigned max1=0;
    int maxCol1=0;
    unsigned max2=0;
    int maxCol2=0;
    unsigned min;
    int minCol;
    //find 2 most distant maxima
    for(int color = 1; color < 254; color++){
        if(tab[color-1] < tab[color] && tab[color] > tab[color+1]){
            if(tab[color] > max1){
                max2 = max1;
                maxCol2 = maxCol1;
                max1 = tab[color];
                maxCol1 = color;
            }else if(tab[color] > max2){
                max2 = tab[color];
                maxCol2 = color;
            }
        }
    }
    if(max1 == 0)
        return 0;            //error
    if(maxCol1 > maxCol2){
        int temp = maxCol1;
        maxCol1 = maxCol2;
        maxCol2 = temp;
    }
    //find minimum between the maxima
    min = max1;
    minCol = maxCol1;
    for(int color = maxCol1+1; color < maxCol2; color++){
        if(tab[color] < min){
            min = tab[color];
            minCol = color;
        }
    }
    return minCol;
}

void Histogram::log(std::ofstream& logFile) const
{
	for(int i = 0; i < 256; i++)
		logFile<<i<<": "<<tab[i]<<std::endl;
}
