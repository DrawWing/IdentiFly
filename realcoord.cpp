#include "realcoord.h"
#include <math.h> //rotate

realCoord::realCoord(double dx, double dy)
{
  x=dx;
  y=dy;
}

realCoord::realCoord(Coord pxl)
{
  x=pxl.dx();
  y=pxl.dy();
}

realCoord::realCoord()
{
  x=0.0;
  y=0.0;
}

void realCoord::fromCoord(Coord pxl)
{
  x=pxl.dx();
  y=pxl.dy();
}

void realCoord::setX(double dx)
{
  x=dx;
}

void realCoord::setY(double dy)
{
  y=dy;
}

void realCoord::setXY(double dx, double dy)
{
  x=dx;
  y=dy;
}

double realCoord::dy() const
{
  return y;
}

double realCoord::dx() const
{
  return x;
}

QString realCoord::toTxt(void) const
{
    QString out =  QString("%1 %2\n").arg(x, 10).arg(y, 10);
    return out;
}

void realCoord::rotate(double angle)
{
	double sin_angle = sin(angle);
	double cos_angle = cos(angle);
	double tmpX = x;
	double tmpY = y;
	x = tmpX*cos_angle-tmpY*sin_angle;
	y = tmpY*cos_angle+tmpX*sin_angle;
}

double realCoord::distanceTo(const realCoord & p2) const
{
    return _hypot (x-p2.dx(), y-p2.dy());
}

std::ostream& operator<<(std::ostream& s, realCoord c)
{
  return s<<'('<<c.dx()<<','<<c.dy()<<')';
}

//  distance between 2 points.
double distance (const realCoord & p1, const realCoord & p2)
{
	return _hypot (p1.dx()-p2.dx(), p1.dy()-p2.dy());
}

