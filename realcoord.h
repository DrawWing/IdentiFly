#ifndef REALCOORD_H
#define REALCOORD_H

#include "coord.h"

#include <fstream> 
#include <iostream>
#include <QString>

class realCoord
{
public:
    realCoord(double, double);
    realCoord(Coord);
    realCoord();
    void fromCoord(Coord);
    void setXY(double, double);
    void setX(double);
    void setY(double);
    double dx() const;
    double dy() const;
    QString toTxt(void) const;
    bool isNull() const;
    void rotate(double);
    double distanceTo(const realCoord & p2) const;

    std::ostream& operator<<(std::ostream&);
    realCoord & operator*=( const double p );
    realCoord & operator+=( const realCoord & p );
    realCoord & operator-=( const realCoord & p );
    friend inline realCoord operator+( const realCoord &, const realCoord & );
    friend inline realCoord operator-( const realCoord &, const realCoord & );
    friend double distance(const realCoord &, const realCoord & );

private:
    double x;
    double y;
};

inline bool realCoord::isNull() const
{
    return ((x==0.0)&&(y==0.0));
}

inline bool operator==(realCoord a, realCoord b)
{
    return a.dx()==b.dx() && a.dy()==b.dy();
}

inline realCoord & realCoord::operator*=( const double p )
{
    x*=p; y*=p; return *this;
}

inline realCoord & realCoord::operator+=( const realCoord & p )
{
    x+=p.dx(); y+=p.dy(); return *this;
}

inline realCoord & realCoord::operator-=( const realCoord & p )
{
    x-=p.dx(); y-=p.dy(); return *this;
}

inline realCoord operator+( const realCoord &p1, const realCoord &p2 )
{
    return realCoord(p1.dx()+p2.dx(), p1.dy()+p2.dy());
}

inline realCoord operator-( const realCoord &p1, const realCoord &p2 )
{
    return realCoord(p1.dx()-p2.dx(), p1.dy()-p2.dy());
}

#endif // REALCOORD_H

