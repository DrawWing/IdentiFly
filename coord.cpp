#include "coord.h"
#include <math.h>
#include <QStringList>

Coord::Coord(int dx, int dy){
  x=dx;
  y=dy;
}

Coord::Coord(){
  x=0;
  y=0;
}

void
Coord::setX(int dx){
  x=dx;
}

void Coord::setY(int dy)
{
  y=dy;
}

void Coord::setXY(int dx, int dy)
{
  x=dx;
  y=dy;
}

int Coord::dy() const
{
  return y;
}

int Coord::dx() const
{
  return x;
}

QString Coord::toTxt() const
{
  QString outStr = "(";
  QString valStr = QString::number(x);
  outStr += valStr;
  outStr += ",";
  valStr = QString::number(y);
  outStr += valStr;
  outStr += ")";
  return outStr;
}

void Coord::fromTxt(QString inString)
{
    inString = inString.trimmed();
    inString.remove("(");
    inString.remove(")");
    QStringList strLst = inString.split(",");
    if(strLst.size() != 2)
        return;
    QString xString = strLst.at(0);
    x = xString.toUInt();
    QString yString = strLst.at(1);
    y = yString.toUInt();
}

// convert between bitamap and cartesian coordinates
void Coord::flip(int imageHeight)
{
    y = imageHeight - y;
}

void Coord::log(std::ofstream& logFile) const
{
  logFile<<"("<<x<<","<<y<<")";
}

void Coord::save(std::ofstream& logFile) const
{
	logFile<<x<<" "<<y<<std::endl; 
}

std::ostream& operator<<(std::ostream& s, Coord c){
  return s<<'('<<c.dx()<<','<<c.dy()<<')';
}

double Coord::distanceTo(const Coord & p2) const
{
    return _hypot (x-p2.dx(), y-p2.dy());
}

// Calculate distance between 2 points.
double
coordDistance (const Coord & p1, const Coord & p2)
{
	return _hypot (p1.dx()-p2.dx(), p1.dy()-p2.dy());
}
