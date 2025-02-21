#ifndef DWCHISQR_H
#define DWCHISQR_H

class dwChisqr
{
public:
    dwChisqr();
    double probability(int Dof, double Cv);
    double incompletegammac(double a, double x);

private:
    static const int A = 15;

    long double igf(long double S, long double Z);
    long double gamma(long double N);
};

#endif // DWCHISQR_H
