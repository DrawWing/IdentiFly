// based on:
// http://www.codeproject.com/Articles/432194/How-to-Calculate-the-Chi-Squared-P-Value

/*
    Implementation of the Chi-Square Distribution &
    Incomplete Gamma Function in C

    Written By Jacob Wells
    July 31, 2012
    Based on the formulas found here:

    Wikipedia - Incomplete Gamma Function -> Evaluation formulae -> Connection with Kummer's confluent hypergeometric function
    http://en.wikipedia.org/wiki/Regularized_Gamma_function#Connection_with_Kummer.27s_confluent_hypergeometric_function

    Wikipedia - Chi-squared Distribution -> Cumulative distribution function
    http://en.wikipedia.org/wiki/Chi-squared_distribution#Cumulative_distribution_function

    These functions are placed in the Public Domain, and may be used by anyone, anywhere, for any reason, absolutely free of charge.

*/

#include <math.h>
#include "dwChisqr.h"

dwChisqr::dwChisqr()
{

}

// Dof degrees of freedom
// Cv critical value
double dwChisqr::probability(int Dof, double Cv)
{
    if(Cv < 0 || Dof < 1)
     {
         return 0.0;
     }
     long double K = ((double)Dof) * 0.5;
     long double X = Cv * 0.5;
     if(Dof == 2)
     {
     return exp(-1.0 * X);
     }

     long double PValue = igf(K, X);
//     if(isnan(PValue) || isinf(PValue) || PValue <= 1e-8)
     if(isnan(PValue) || isinf(PValue))
     {
         return 0;
     }

     long double gammaK = gamma(K);
     // long double gammaK = tgamma(K); // use of tgamma from <cmath>
     long double p = PValue / gammaK;
     // does not work if PValue /= gamma(K);
     //PValue /= tgamma(K);

     return (1.0 - p);
}

/*
    Incomplete Gamma Function

    No longer need as I'm now using the log_igf(), but I'll leave this here anyway.
*/

long double dwChisqr::igf(long double S, long double Z)
{
    if(Z < 0.0)
    {
        return 0.0;
    }
    long double Sc = (1.0 / S);
    Sc *= powl(Z, S);
    Sc *= expl(-Z);

    long double Sum = 1.0;
    long double Nom = 1.0;
    long double Denom = 1.0;

    int I;
    for( I = 0; I < 2000; I++) // 200
    {
        Nom *= Z;
        S++;
        Denom *= S;
        Sum += (Nom / Denom);
    }

    return Sum * Sc;
}

/*
    Implementation of the Gamma function using Spouge's Approximation in C.

    Written By Jacob
    7/31/2012
    Public Domain

    This code may be used by anyone for any reason
    with no restrictions absolutely free of cost.
*/

//#include <math.h>
//#include "gamma.h"


//#define A 15 // 15
/*
    A is the level of accuracy you wish to calculate.
    Spouge's Approximation is slightly tricky, as you
    can only reach the desired level of precision, if
    you have EXTRA precision available so that it can
    build up to the desired level.

    If you're using double (64 bit wide datatype), you
    will need to set A to 11, as well as remember to
    change the math functions to the regular
    (i.e. pow() instead of powl())

    double A = 11
    long double A = 15

   !! IF YOU GO OVER OR UNDER THESE VALUES YOU WILL !!!
              !!! LOSE PRECISION !!!
*/


long double dwChisqr::gamma(long double N)
{
    /*
        The constant SQRT2PI is defined as sqrt(2.0 * PI);
        For speed the constant is already defined in decimal
        form.  However, if you wish to ensure that you achieve
        maximum precision on your own machine, you can calculate
        it yourself using (sqrt(atan(1.0) * 8.0))
    */

    //const long double SQRT2PI = sqrtl(atanl(1.0) * 8.0);
    const long double SQRT2PI = 2.5066282746310005024157652848110452530069867406099383;

    long double Z = (long double)N;
    long double Sc = powl((Z + A), (Z + 0.5));
    Sc *= expl(-1.0 * (Z + A));
    Sc /= Z;

    long double F = 1.0;
    long double Ck;
    long double Sum = SQRT2PI;

    int K;
    for( K = 1; K < A; K++)
    {
        Z++;
        Ck = powl(A - K, K - 0.5);
        Ck *= expl(A - K);
        Ck /= F;

        Sum += (Ck / Z);

        F *= (-1.0 * K);
    }

    return (long double)(Sum * Sc);
}

/*************************************************************************
Complemented incomplete gamma integral

The function is defined by


 igamc(a,x)   =   1 - igam(a,x)

                           inf.
                             -
                    1       | |  -t  a-1
              =   -----     |   e   t   dt.
                   -      | |
                  | (a)    -
                            x


In this implementation both arguments must be positive.
The integral is evaluated by either a power series or
continued fraction expansion, depending on the relative
values of a and x.

ACCURACY:

Tested at random a, x.
               a         x                      Relative error:
arithmetic   domain   domain     # trials      peak         rms
   IEEE     0.5,100   0,100      200000       1.9e-14     1.7e-15
   IEEE     0.01,0.5  0,100      200000       1.4e-13     1.6e-15

Cephes Math Library Release 2.8:  June, 2000
Copyright 1985, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double dwChisqr::incompletegammac(double a, double x)
{
    double igammaepsilon;
    double igammabignumber;
    double igammabignumberinv;
    double ans;
    double ax;
    double c;
    double yc;
    double r;
    double t;
    double y;
    double z;
    double pk;
    double pkm1;
    double pkm2;
    double qk;
    double qkm1;
    double qkm2;
    // double tmp;
    double result;


    igammaepsilon = 0.000000000000001;
    igammabignumber = 4503599627370496.0;
    igammabignumberinv = 2.22044604925031308085*0.0000000000000001;
    if( (x<=(double)(0))||(a<=(double)(0)) )
        // if( ae_fp_less_eq(x,(double)(0))||ae_fp_less_eq(a,(double)(0)) )
    {
        result = (double)(1);
        return result;
    }
    if( (x<(double)(1))||(x<a) )
        // if( ae_fp_less(x,(double)(1))||ae_fp_less(x,a) )
    {
        result = (double)1-incompletegammac(a, x);
        // result = (double)1-incompletegamma(a, x, _state);
        return result;
    }
    ax = a*log(x)-x-lgamma(a);
    // ax = a*ae_log(x, _state)-x-lngamma(a, &tmp, _state);
    if( ax<-709.78271289338399 )
        // if( ae_fp_less(ax,-709.78271289338399) )
    {
        result = (double)(0);
        return result;
    }
    ax = exp(ax);
    // ax = ae_exp(ax, _state);
    y = (double)1-a;
    z = x+y+(double)1;
    c = (double)(0);
    pkm2 = (double)(1);
    qkm2 = x;
    pkm1 = x+(double)1;
    qkm1 = z*x;
    ans = pkm1/qkm1;
    do
    {
        c = c+(double)1;
        y = y+(double)1;
        z = z+(double)2;
        yc = y*c;
        pk = pkm1*z-pkm2*yc;
        qk = qkm1*z-qkm2*yc;
        if( qk!=(double)(0) )
            // if( ae_fp_neq(qk,(double)(0)) )
        {
            r = pk/qk;
            t = fabs((ans-r)/r);
            // t = ae_fabs((ans-r)/r, _state);
            ans = r;
        }
        else
        {
            t = (double)(1);
        }
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        if( fabs(pk)>igammabignumber )
            // if( ae_fp_greater(ae_fabs(pk, _state),igammabignumber) )
        {
            pkm2 = pkm2*igammabignumberinv;
            pkm1 = pkm1*igammabignumberinv;
            qkm2 = qkm2*igammabignumberinv;
            qkm1 = qkm1*igammabignumberinv;
        }
    }
    while(t>igammaepsilon);
    // while(ae_fp_greater(t,igammaepsilon));
    result = ans*ax;
    return result;
}


