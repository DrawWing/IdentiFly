/*
    Copyright (C) 2026 Adam Tofilski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

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