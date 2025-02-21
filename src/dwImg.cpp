#include <QVector>
#include <qimage.h>
#include <QDebug>

#include "dwImg.h"
#include <math.h>
#include "histogram.h"
#include "winginfo.h" // for superimpose
#include <QPainter>

dwImage::dwImage()
    :QImage()
{
}

dwImage::dwImage(const QImage & img)
    :QImage(img)
{
}

dwImage::dwImage(const dwImage &img)
    :QImage(img)
{
}

//wiesza sie kiedy fileName nie jest dw.png
dwImage::dwImage( const QString & fileName, const char * format )
    :QImage(fileName, format)
{
}

dwImage::dwImage( int w, int h, Format format ):
    QImage(w, h, format)
{
    if( format == QImage::Format_Indexed8 )
        setGrayTable();
}

dwImage dwImage::dwCopy(){
    //    qimageInfo(); //debug
    dwImage newImg(width(), height(), format());
    if(hasAlphaChannel())
        setAlphaChannel(newImg);
    newImg.setColorTable(colorTable());
    // newImg.margin = margin;
    unsigned char * pd = newImg.bits();
    unsigned char * ps = bits();
    for(int i=0; i < sizeInBytes(); i++) //(width()*height() < numBytes()
        pd[i]=ps[i];
    return newImg;
}

void dwImage::copyText(dwImage & toImg) const
{
    // copy only the new text format
    QString inTxt = text("IdentiFly");
    if(inTxt.isEmpty())
    {
        QStringList txtList = textKeys();
        for(int i = 0; i < txtList.size(); ++i)
        {
            QString key = txtList.at(i);
            QString value = text(key);
            toImg.setText(key, value);
        }
    }
    else
    {
        toImg.setText("IdentiFly", inTxt);
    }
}

//There is no method in QImage for changing width and height of null image.
void dwImage::from(QImage * img){
	if(width()!=img->width() || height()!=img->height())
		return;
//	if(img->hasAlphaChannel())
//		setAlphaChannel(img->alphaChannel());
	setDotsPerMeterX( img->dotsPerMeterX() );
	setDotsPerMeterY( img->dotsPerMeterY() );

	//copy color table
	setColorTable(img->colorTable());
    // margin = 0;
	//copy image pixels
	unsigned char * pd = bits();
	unsigned char * ps = img->bits();
    for(int i=0; i < img->sizeInBytes(); i++) //czy to jest bezpieczne czy rozmiar determinuje numBytes
//        for(int i=0; i < img->numBytes(); i++) //czy to jest bezpieczne czy rozmiar determinuje numBytes
        pd[i]=ps[i];
}

//*oo
//*@o
//***
//            x - non white, o - white, * - andy color
bool dwImage::isCorner(unsigned x, unsigned y) const
{
    if( *(scanLine(y-1)+x) == 255 && *(scanLine(y-1)+x+1) == 255 && *(scanLine(y)+x+1) == 255)
        return true;
    if( *(scanLine(y)+x+1) == 255 && *(scanLine(y+1)+x+1) == 255 && *(scanLine(y+1)+x) == 255)
        return true;
    if( *(scanLine(y+1)+x) == 255 && *(scanLine(y+1)+x-1) == 255 && *(scanLine(y)+x-1) == 255)
        return true;
    if( *(scanLine(y)+x-1) == 255 && *(scanLine(y-1)+x-1) == 255 && *(scanLine(y-1)+x) == 255)
        return true;
    return false;
}

//powinna byc inline
void 
dwImage::markPxl(Coord pxl, unsigned char color){
  *(scanLine(pxl.dy())+pxl.dx()) = color;
}

void dwImage::markList(const pxlList& lst, unsigned char color){
    nodePxl* node = lst.front;
    do{
      markPxl(node->pxl, color);
      node = node->next;
    }while(node != lst.front);
}

void dwImage::markList(const pxlList * outline, unsigned char color)
{
    nodePxl* node = outline->front;
    do{
      markPxl(node->pxl, color);
      node = node->next;
    }while(node != outline->front);
}

dwImage dwImage::cropOutline(const pxlList * outline, double angle)
{
    pxlList rotOtl = outline->copy();
    rotOtl.rotate(angle);
    Coord topPxl = (rotOtl.top())->pxl;
    Coord bottomPxl = (rotOtl.bottom())->pxl;
    Coord rightPxl = (rotOtl.right())->pxl;
    Coord leftPxl = (rotOtl.left())->pxl;
    int wdh = bottomPxl.dy() - topPxl.dy();
    int lgh = rightPxl.dx() - leftPxl.dx();

    const int margin = 10; //magic
    //find top left corner
    realCoord corner(leftPxl.dx(), topPxl.dy());
    realCoord offset( -margin, -margin);
    corner+=offset;
    corner.rotate(-angle);

    dwImage outImg = crop(corner, lgh+2*margin, wdh+2*margin, angle);
    return outImg;
}

void dwImage::addMargin(int bdr, unsigned char color)
{

    // if(margin < bdr){
        int row, col;

        for (row = 0; row < bdr; row++)
            fillRow(row, color);
        // for (row = (unsigned)height()-bdr; row < (unsigned)height(); row++)
        for (row = height()-bdr; row < height(); row++)
            fillRow(row, color);
        for (col = 0; col < bdr; col++)
            fillColumn(col, color);
        for (col = width()-bdr; col < width(); col++)
            fillColumn(col, color);

        // margin = bdr;
    // }
}

void dwImage::fillRow(int row, unsigned char fillColor)
{
  for (int col = 0; col < width(); ++col)
    *(scanLine(row) + col) = fillColor;
}

void dwImage::fillColumn(int col, unsigned char fillColor)
{
  for (int row = 0; row < height(); ++row)
    *(scanLine(row) + col) = fillColor;
}

// only for grayscale
void dwImage::clearRightMargin(int margin)
{
    for (int col = width() - margin; col < width(); ++col)
        fillColumn(col, 0xFF);
}

// Remove black margin produced by some ccd cameras and photographic scanners when scanned automatically 
//only for grayscale images
// The smaller thdFraction the more border is remobed.
void dwImage::clearBorder(double thdFraction, unsigned char colorThd)
{
	if((thdFraction < 0.0) || (thdFraction > 1.0))
		return;

	unsigned char fillColor = 0xFF;
//	unsigned char colorThd = 150

	int left = 0;
	int right = 0;
	int top = 0;
	int bottom =0;

	int thd = width()*thdFraction;
	int row = 0;
	bool change;
	do{
		change = false;
		int cnt = 0;
		for (int col = 0; col < width(); col++){
			unsigned char color = *(scanLine(row) + col);
			if(color < colorThd){
				cnt++;
				if(cnt > thd){
					//fillRow(row, fillColor);
					top++;
					change = true;
					break;
				}
			}else{
				cnt = 0;
			}
		}
		row++;
		if(row == height())
			return; //error
	}while( change );

	row = height()-1; // powinno być -1
	do{
		change = false;
		int cnt = 0;
		for (int col = 0; col < width(); col++){
			unsigned char color = *(scanLine(row) + col);
			if(color < colorThd){
				cnt++;
				if(cnt > thd){
					//fillRow(row, fillColor);
					bottom++;
					change = true;
					break;
				}
			}else{
				cnt = 0;
			}
		}
		row--;
		if(row == 0)
			return; //error
	}while(change);  

	thd = height()*thdFraction;
	int col = 0;
	do{
		change = false;
		int cnt = 0;
		for (row = 0; row < height(); row++){
			unsigned char color = *(scanLine(row) + col);
			if(color < colorThd){
				cnt++;
				if(cnt > thd){
					//fillColumn(col, fillColor);
					left++;
					change = true;
					break;
				}
			}else{
				cnt = 0;
			}
		}
		col++;
                if(col == width())
			return; //error
	}while( change );

	col = width()-1;
	do{
		change = false;
		int cnt = 0;
		for (row = 0; row < height(); row++){
			unsigned char color = *(scanLine(row) + col);
			if(color < colorThd){
				cnt++;
				if(cnt > thd){
					//fillColumn(col, fillColor);
					right++;
					change = true;
					break;
				}
			}else{
				cnt = 0;
			}
		}
		col--;
                if(row == 0)
			return; //error
	}while( change );
	//Filling collumnt after detecting them
	for (row = 0; row < top; row++){
		fillRow(row, fillColor);
	}
	for (row = height()-1; row > height()-bottom; row--){
		fillRow(row, fillColor);
	}
	for (int col = 0; col < left; col++){
		fillColumn(col, fillColor);
	}
	for (int col = width()-1; col > width()-right; col--){
		fillColumn(col, fillColor);
	}
}

//average pixels from radius
void dwImage::mean(int radius){

  dwImage org = dwCopy();

  for (int row=radius; row < height()-radius; row++){
    for (int col=radius; col < width()-radius; col++){
      int suma=0;
      //mozna przyspieszyc
      for(int drow = -radius; drow < radius+1; drow++){
        for(int dcol = -radius; dcol < radius+1; dcol++){
          unsigned char val = *(org.scanLine(row+drow) + col+dcol);
          suma+= val;
        }
      }
      double average = suma/((2*radius+1) * (2*radius+1));
      *(scanLine(row) + col) = average;
    }
  }
}

//Gaussian blur
void dwImage::blur(int times){
    int gaus[] = {
        1, 4, 7, 4,1,
        4,16,26,16,4,
        7,26,41,26,7,
        4,16,26,16,4,
        1, 4, 7, 4,1};
    const int radius = 2;

    for(int i=0; i<times; i++){
        //  dwImage org(this, true);
        dwImage org = dwCopy();
        //zamiast powtorzen lepiej uzyc wiekszy kernel
        for (int row=radius; row < height()-radius; row++){
            for (int col=radius; col < width()-radius; col++){
                int suma = 0;
                int cnt = 0;
                for(int drow = -radius; drow < radius+1; drow++){
                    for(int dcol = -radius; dcol < radius+1; dcol++){
                        unsigned char val = *(org.scanLine(row+drow) + col+dcol);
                        suma+= val*gaus[cnt];
                        cnt++;
                    }
                }
                double average = suma/273; //from Gaussian kernel
                *(scanLine(row) + col) = average;
            }
        }
    }
}

//Find thereshold for extraction of wing outline
int dwImage::outlineThd() const {
    Histogram hist(this);
    const int range = 4;           //range over which histogram is averaged bylo 3
    hist.smooth(range);
    int minCol;
    minCol = hist.maxMinMax(); //minimum between maxima
    return minCol;
}

// Threshold the grayscal image by modyfying color table. 
// was threshold(int) it was discirminated from threshold(unisgned)
void dwImage::thresholdFast(int thd)
{
  QRgb black = 0xFF000000;
  QRgb white = 0xFFFFFFFF;

  if(thd > 255)
      thd = 255; //it does not make sens to make it all black
  if(thd < 0)
      thd = 0;
  for(int i = 0; i < thd; i++)
    setColor ( i, black );
  for(int j = thd ; j < 256; j++)
    setColor ( j, white );
}

// Threshold the grayscal image by modyfying all pixels.
void dwImage::threshold(unsigned thd)
{
    int margin = 1;
  for ( int y = margin; y < height()-margin; y++ ) {
    for ( int x = margin; x < width()-margin; x++ ){
      if(*(scanLine(y)+x) < thd)
        *(scanLine(y)+x) = 0;
    }
  }
}

// Modify color table to show grayscale image.
void dwImage::setGrayTable()
{
    setColorCount(256);
    for(int i = 0; i < colorCount(); i++)
		setColor ( i, qRgba(i,i,i,0xFF) );
}

// Is the color table gray and ordered.
bool dwImage::isGrayTable() const
{
    if(colorCount() == 0)
        return false;
    for(int i = 0; i < colorCount(); i++){
		QRgb value = color(i);
		if( qRed(value) != i )
			return false;
		if( qGreen(value) != i )
			return false;
		if( qBlue(value) != i )
			return false;
	}
	return true;
}

//// Graustufen Bild erstellen, mit selben Dimensionen aber Format Indexed8
//QImage img_gray(img.width(), img.height(), QImage::Format_Indexed8);           
//
//// Colortable erstellen für Graustufen
//QVector<QRgb> grayscales;
//for (int i=0; i<256; ++i)
//    grayscales.push_back(qRgb(i,i,i));
///
/// colortable dem graustufen bild zuweisen
//img_gray.setColorTable(grayscales);
//
//// farben übertragen
//for (int y=0; y<img.height(); ++y)
//    for (int x=0; x<img.width(); ++x)
//    {
//           // farbwert holen
//           QRgb rgb = img.pixel(x,y);
//           // umrechnen in graustufe
//           unsigned char gray = 0.299*qRed(rgb) + 0.587*qGreen(rgb) + 0.114*qBlue(rgb);
//           // dem graustufen bild den wert zuweisen
//           img_gray.setPixel(x,y, gray);
//     }

// Convert image to 8 bit per pixel grayscale format. 
// Returns grayscale image
dwImage dwImage::convertToGray8() const
{
    dwImage outImg( width(), height(), QImage::Format_Indexed8 );
    outImg.setGrayTable();
    if(depth() != 32)
    {
        dwImage tmpImg = convertToFormat(QImage::Format_RGB32);
        for (int row = 0; row < height(); row++){
            for (int col = 0; col < width(); col++){
                QRgb theColor = tmpImg.pixel(col, row);
                unsigned luminance = (qRed(theColor)+qGreen(theColor)+qBlue(theColor))/3;
                *(outImg.scanLine(row) + col) = luminance;
            }
        }
    }else{
        for (int row = 0; row < height(); row++){
            for (int col = 0; col < width(); col++){
                QRgb theColor = pixel(col, row);
                int alpha = qAlpha(theColor);
                if(alpha == 255)
                {
                    unsigned luminance = (qRed(theColor)+qGreen(theColor)+qBlue(theColor))/3;
                    *(outImg.scanLine(row) + col) = luminance;
                }
                else
                {
                    double luminance = 255.0 - (alpha/255.0)*(255.0-(qRed(theColor)+qGreen(theColor)+qBlue(theColor))/3.0);
                    *(outImg.scanLine(row) + col) = luminance;
                }
            }
        }
    }
    outImg.setDotsPerMeterX(dotsPerMeterX());
    outImg.setDotsPerMeterY(dotsPerMeterY());
    copyText(outImg);
    return(outImg);
}

//probably for debuging only
void dwImage::qimageInfo()
{
    int dth = depth();
    qDebug() << "depth " << dth;
    int w = width();
    qDebug() << w;
    int h = height();
    qDebug() << h;
    int n = colorCount();
    qDebug() << "colorCount" << n;
    bool null = isNull();
    qDebug() << "is null " << null;
    null = isGrayTable();
    qDebug() << "isGrayTable " << null;
    null = allGray();
    qDebug() << "allGray " << null;
    null = isGrayscale();
    qDebug() << "isGrayscale " << null;
    Format f = format();
    qDebug() << f;
    QVector<QRgb> ct = colorTable();
    qDebug() << ct;
}

// int dwImage::getMargin() const
// {
// 	return margin;
// }

//use the plane equation to interpolate pixel vaues 
//it does not work for last row and column !!!
unsigned dwImage::interpolate(const realCoord pxl) const
{
    int prevX = floor(pxl.dx());
    int nextX = prevX+1;
    int prevY = floor(pxl.dy());
    int nextY = prevY+1;
    double diffX = nextX - pxl.dx();
    double diffY = nextY - pxl.dy();
    //this is important for the last row and column
    if(prevX == width()-1)
        nextX = prevX;
    if(prevY == height()-1)
        nextY = prevY;

    Coord p1,p2,p0;
    if(diffX < 0.5){
        if(diffY < 0.5){
            p2.setXY(nextX, nextY);
            p1.setXY(nextX, prevY);
            p0.setXY(prevX, nextY);
        }else{
            p2.setXY(nextX, prevY);
            p1.setXY(prevX, prevY);
            p0.setXY(nextX, nextY);
        }
    }else{
        if(diffY < 0.5){
            p2.setXY(prevX, nextY);
            p1.setXY(nextX, nextY);
            p0.setXY(prevX, prevY);
        }else{
            p2.setXY(prevX, prevY);
            p1.setXY(prevX, nextY);
            p0.setXY(nextX, prevY);
        }
    }
    int v1x = p0.dx() - p2.dx();
    int v1y = p0.dy() - p2.dy();
    int v1z = *(scanLine(p0.dy()) + p0.dx()) - *(scanLine(p2.dy()) + p2.dx());
    int v2x = p2.dx() - p1.dx();
    int v2y = p2.dy() - p1.dy();
    int v2z = *(scanLine(p2.dy()) + p2.dx()) - *(scanLine(p1.dy()) + p1.dx());

    int a = v1y*v2z - v2y*v1z;
    int b = v1z*v2x - v2z*v1x;
    int c = v1x*v2y - v2x*v1y;
    int d = -a*p2.dx() - b*p2.dy() - c * (*(scanLine(p2.dy()) + p2.dx()));

    double out = -(a*pxl.dx() + b*pxl.dy() + d)/c;
    return out;
}

dwImage dwImage::simpleCrop(pxlList * outline, const int margin) const
{
//    const int margin = 10;

    //find extrema of the rotated outline
    Coord topPxl = (outline->top())->pxl;
    if(topPxl.dy() < margin)
        topPxl.setY(margin);

    Coord bottomPxl = (outline->bottom())->pxl;
    if(bottomPxl.dy() > height()-margin)
        bottomPxl.setY(height()-margin);

    Coord rightPxl = (outline->right())->pxl;
    if(rightPxl.dx() > width()-margin)
        rightPxl.setX(width()-margin);
    Coord leftPxl = (outline->left())->pxl;
    if(leftPxl.dx() < margin)
        leftPxl.setX(margin);

    int wdh = bottomPxl.dy() - topPxl.dy();
    int lgh = rightPxl.dx() - leftPxl.dx();


    dwImage outImg;
    outImg = copy(leftPxl.dx()-margin, topPxl.dy()-margin, lgh+2*margin, wdh+2*margin);
    outImg.setDotsPerMeterX(dotsPerMeterX());
    outImg.setDotsPerMeterY(dotsPerMeterY());
    return outImg;
}

//crop from this image another image of size wdh x hgt, with upper top corner in corner, and rotated by angle.
//Angle positive = rotate clockwise???
//for grayscale only
dwImage dwImage::crop(const realCoord corner, const unsigned xSize, const unsigned ySize, const double angle) const
{
    double thetaHor = -angle;
    double thetaVer  = thetaHor + M_PI_2; //perpendicular to thetaHor (counterclockwise)
    dwImage outImg(xSize, ySize);

    for(unsigned row = 0; row < ySize; row++){
        realCoord offsetVer(row * cos(thetaVer), row * sin(thetaVer));
        realCoord leftPxl = corner + offsetVer;
        for(unsigned col = 0; col < xSize; col++){
            realCoord offsetHor(col * cos(thetaHor), col * sin(thetaHor));
            realCoord currPxl = leftPxl + offsetHor;
            unsigned char color;
            if((currPxl.dx() > width()-1)||(currPxl.dy() > height()-1)
                ||(currPxl.dx() < 0)||(currPxl.dy() < 0))
                color = 255;
            else
                color = interpolate(currPxl);
//				color = bicubic(currPxl);
            *(outImg.scanLine(row) + col) = color;
        }
    }

    outImg.setDotsPerMeterX(dotsPerMeterX());
    outImg.setDotsPerMeterY(dotsPerMeterY());

    return outImg;
}

dwImage dwImage::scale(const double ratio) const
{
    const unsigned wdh = width();
    const unsigned hgt = height();
    const unsigned newWdh = wdh*ratio;
    const unsigned newHgt = hgt*ratio;
    //make sure that: ratio*(newWdh-1) == wdh-1
    // const double wdhRatio = (double)(wdh-1)/(newWdh-1);
    // const double hgtRatio = (double)(hgt-1)/(newHgt-1);
    const double wdhRatio = (double)(wdh)/(newWdh);
    const double hgtRatio = (double)(hgt)/(newHgt);

    dwImage outImg(newWdh, newHgt);
    for(unsigned row = 0; row < newHgt; ++row){
        for(unsigned col = 0; col < newWdh; ++col){
            realCoord currPxl(col*wdhRatio, row*hgtRatio);
            //unsigned char color = bicubic(currPxl);
            unsigned char color = interpolate(currPxl);
            *(outImg.scanLine(row) + col) = color;
        }
    }
    outImg.setDotsPerMeterX(ratio * dotsPerMeterX());
    outImg.setDotsPerMeterY(ratio * dotsPerMeterY());
    copyText(outImg);
    WingInfo info;
    info.fromImg(outImg);
    info.scale(ratio);
    info.toImg(outImg);

    return outImg;
}

// rotate 90 degrees clockwise
dwImage dwImage::getRotated() const
{
//    QMatrix matrix;
//    matrix.rotate(90);

    QTransform transform;
    transform.rotate(90); // 90-degree clockwise rotation
    dwImage outImg;
    outImg = transformed(transform);

    outImg.setDotsPerMeterX(dotsPerMeterX());
    outImg.setDotsPerMeterY(dotsPerMeterY());
    copyText(outImg);

    return outImg;
}

//calculate avarage color around thePnt in square of size = size
double dwImage::meanImg(Coord thePnt, int size) const
{
    int sum = 0;
    int count = 0;
    for(int i = -size; i < size + 1; ++i){
        int row = thePnt.dy() + i;
        if(row < 0 || row > this->height() - 1) continue;
        for(int j = -size; j < size + 1; ++j){
            int col = thePnt.dx() + j;
            if(col < 0 || col > this->width() - 1) continue;
            unsigned color = *(scanLine(row) + col);
            sum += color;
            ++count;
        }
    }
    double outMean = (double)sum / (double)count;
    return outMean;
}

//calculate avarage color of whole image
double dwImage::meanImg(){
    if( height() == 0 or width() == 0)
        return -1.0;

    unsigned sum = 0;
    for(int row = 0; row < this->height(); row++){
        for(int col = 0; col < this->width(); col++){
            sum += *(scanLine(row) + col);
        }
    }
    double outMean = sum/(this->height()*this->width());
    return outMean;
}

//add the same value to all pixels
void dwImage::add(int toAdd){
    if(toAdd == 0) return;
    for(int row = 0; row < this->height(); row++){
        for(int col = 0; col < this->width(); col++){
            int prevColor = *(scanLine(row) + col);
            if(prevColor + toAdd > 255)
                *(scanLine(row) + col) = 255;
            else if(prevColor + toAdd < 0)
                *(scanLine(row) + col) = 0;
            else *(scanLine(row) + col) += toAdd;
        }
    }
}

//find difference between images of the same size
int dwImage::difference(dwImage toCompare){
    if(this->height() != toCompare.height()) return -1;
    if(this->width() != toCompare.width()) return -1;
    int diff = 0;
    for(int row = 0; row < this->height(); ++row){
        for(int col = 0; col < this->width(); ++col){
            diff += abs(*(scanLine(row) + col) - *(toCompare.scanLine(row) + col) );
        }
    }
    return diff;
}

// distance to neighbour in radius arond inPxl
// neighbour cannot be white
double dwImage::toNeighbour(realCoord inPxl, int radius)
{
    int inRow = floor( inPxl.dy() );
    int inCol = floor( inPxl.dx() );
    Coord thePxl(inCol, inRow);
    return toNeighbour(thePxl, radius);

}

// distance to neighbour in radius arond inPxl
// neighbour cannot be white
double dwImage::toNeighbour(Coord inPxl, int radius)
{
    int inRow = inPxl.dy();
    int inCol = inPxl.dx();
    double minDist = radius + radius; //double radius

    //only radius -1 are checked
    for(int x = -radius + 1; x < radius; ++x)
    {
        for(int y = -radius + 1; y < radius; ++y)
        {
            int row = inRow + y;
            int col = inCol + x;
            if(!valid(col, row))
                continue;
            QRgb theColor = color(*(scanLine(row) + col));
            if(theColor != 0xFFFFFFFF)
//                unsigned theColor = *(scanLine(row) + col);
//                if(theColor < 255)
            {
                realCoord thePxl(col, row);
                double dist = distance(inPxl, thePxl);
                if(dist < minDist)
                    minDist = dist;
            }
        }
    }
    return minDist;
}

dwImage & dwImage::operator=( const dwImage & img)
{
  this->QImage::operator=(img);
  return *this;
}

dwImage & dwImage::operator=( const QImage & img)
{
  this->QImage::operator=(img);
  return *this;
}

