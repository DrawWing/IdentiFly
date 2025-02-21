#include "dwvector.h"
#include <math.h>

dwVector::dwVector(double x, double y)
{
    dx=x;
    dy=y;
}

dwVector::dwVector(Coord point)
{
    dx=point.dx();
    dy=point.dy();
}

dwVector::dwVector(Coord p1, Coord p2)
{
    dx=p1.dx()-p2.dx();
    dy=p1.dy()-p2.dy();
}

dwVector::dwVector(realCoord p1, realCoord p2)
{
    dx=p1.dx()-p2.dx();
    dy=p1.dy()-p2.dy();
}

void dwVector::fromCoord(Coord p1, Coord p2)
{
    dx=p1.dx()-p2.dx();
    dy=p1.dy()-p2.dy();
}

double dwVector::x() const
{
    return dx;
}

double dwVector::y() const
{
    return dy;
}

realCoord dwVector::toRealCoord() const
{
    realCoord out(dx, dy);
    return out;
}

double dwVector::magnitude()
{
    return _hypot (dx, dy);
}

//Find angle in radians between vector v and vector {1,0}
double dwVector::toRadians() const
{
    double angle;
    dwVector vCopy(dx, dy); //nie chce zniszczyc vectora dla ktorego wywolywana jest funkcja
    if(dy == 0)
    {
        if(dx > 0)
            return 0.0;
        else
            return M_PI;
    }

    if(dx == 0)
    {
        if(dy > 0)
            return M_PI_2;
        else
            return M_PI+M_PI_2;
    }

    vCopy.normalize();
    dwVector v1(1.0, 0.0);
    angle = acos(dot(v1, vCopy));

    if(dy > 0)
        return angle;
    else
        return 2*M_PI-angle;
}

//mozna przyspieszyc przez sprawdzenie wspolrzednych
void dwVector::normalize ()
{
    double m = magnitude();

    if (m > 0.0)
    {
        dx /= m;
        dy /= m;
    }
}

// Given the inVec and outVec, return the angle between them in range 0-pi radians
double angle (const dwVector &inVec, const dwVector &outVec)
{
    dwVector v1=inVec;
    dwVector v2=outVec;
    v1.normalize();
    v2.normalize();

    return acos(dot (v2, v1));
}

// Given the inVec and outVec, return the angle between them in range 0 - pi/2 radians
double smallestAngle (const dwVector &inVec, const dwVector &outVec)
{
    double a1 = angle(inVec, outVec);
    dwVector minusOutVec(-outVec.x(), -outVec.y());
    double a2 = angle(inVec, minusOutVec);
    if(a1 < a2)
        return a1;
    else
        return a2;
}

// Given the inVec and outVec, return the angle between them in range (-pi) - (pi) radians
double angle360 (const dwVector &inVec, const dwVector &outVec)
{
    return atan2(inVec.y(),inVec.x()) - atan2(outVec.y(),outVec.x());
}

double angleDif (const double ang1, const double ang2)
{
    if(ang1 < ang2){
        double dif1 = ang2 - ang1;
        double dif2 = ang1 + 2*M_PI - ang2;
        if(dif1 > dif2){
            return dif2;
        }else{
            return dif1;
        }
    }else{
        double dif1 = ang1 - ang2;
        double dif2 = ang2 + 2*M_PI - ang1;
        if(dif1 > dif2){
            return dif2;
        }else{
            return dif1;
        }
    }
}

double dwVector::angleDif (const double ang1, const double ang2)
{
    if(ang1 < ang2){
        double dif1 = ang2 - ang1;
        double dif2 = ang1 + 2*M_PI - ang2;
        if(dif1 > dif2){
            return dif2;
        }else{
            return dif1;
        }
    }else{
        double dif1 = ang1 - ang2;
        double dif2 = ang2 + 2*M_PI - ang1;
        if(dif1 > dif2){
            return dif2;
        }else{
            return dif1;
        }
    }
}

//return perpencicular vector
dwVector dwVector::perpendicular() const
{
    dwVector outVec(-dy,dx);
    return outVec;
}

//return angle of pollar coordinates
double dwVector::theta() const
{
    return atan2(dy,dx); //much better
    /*
	if(dx > 0.0){
	return atan(dy/dx);
	}else if(dx < 0.0){
	if(dy > 0)
	return atan(dy/dx)+M_PI;
	else if(dy < 0) 
	return atan(dy/dx)-M_PI;
	else
	return M_PI;    
	}else{ 
	if(dy > 0)
	return M_PI_2;
	else
	return -M_PI_2;
	}
	*/
}

dwVector dwVector::multiply(double c) const
{
    dwVector outVec(dx * c, dy * c);
    return outVec;
}

