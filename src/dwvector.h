#ifndef DWVECTOR_H
#define DWVECTOR_H

#include "coord.h"
#include "realcoord.h"

# ifndef M_PI
#  define M_PI    3.14159265358979323846
#  define M_PI_2  1.57079632679489661923
#  define M_PI_4  0.78539816339744830962
#  define M_1_PI  0.31830988618379067154
#  define M_2_PI  0.63661977236758134308
#  define M_SQRT2 1.41421356237309504880
#  define M_SQRT1_2 0.70710678118654752440
# endif

class dwVector  
{
public:
    dwVector(double=0.0, double=0.0);
    dwVector(Coord);
    dwVector(Coord, Coord);
    dwVector(realCoord, realCoord);
    void fromCoord(Coord, Coord);
    double x() const;
    double y() const;
    realCoord toRealCoord() const;
    double magnitude();
    double toRadians() const;
    void normalize();
    dwVector perpendicular() const;
    double theta() const;
    dwVector multiply(double) const;
    friend inline dwVector operator+( const dwVector &, const dwVector & );
    friend inline dwVector operator-( const dwVector &, const dwVector & );
    friend inline double dot( const dwVector &, const dwVector & );
    friend double angle(const dwVector &, const dwVector & );
    friend double smallestAngle(const dwVector &, const dwVector & );
    friend double angle360(const dwVector &, const dwVector & );
    friend double angleDif(const double, const double);
    double angleDif(const double, const double);
private:
    double dx;
    double dy;
};

inline dwVector operator+( const dwVector &v1, const dwVector &v2 )
{ return dwVector(v1.dx+v2.dx, v1.dy+v2.dy); }

inline dwVector operator-( const dwVector &v1, const dwVector &v2 )
{ return dwVector(v1.dx-v2.dx, v1.dy-v2.dy); }

inline double dot(const dwVector &v1, const dwVector &v2)
{ return v1.dx * v2.dx + v1.dy * v2.dy; }


#endif // !defined(DWVECTOR_H)
