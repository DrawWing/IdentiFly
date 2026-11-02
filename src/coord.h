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

#ifndef COORD_H
#define COORD_H

#include <fstream> 
#include <iostream> 
#include <QString>

class Coord
{
public:
	Coord(int, int);
	Coord();
	void setXY(int, int);
	void setX(int);
	void setY(int);
	int dx() const;
	int dy() const;
	bool isNull() const;
    QString toTxt() const;
    void fromTxt(QString inString);
    void flip(int imageHeight = 0);
	void log(std::ofstream&) const;
	void save(std::ofstream&) const;
	Coord & operator+=( const Coord & p );
	Coord & operator-=( const Coord & p );
	Coord & operator*=(double c);
	friend inline bool operator==(const Coord & a, const Coord & b);
	friend inline bool operator!=(const Coord & a, const Coord & b);
	friend inline Coord operator+( const Coord &, const Coord & );
	friend inline Coord operator-( const Coord &, const Coord & );
	std::ostream& operator<<(std::ostream&);

    double distanceTo(const Coord & p2) const;
	friend double coordDistance(const Coord &, const Coord & );

private:
	int x;
	int y;
};

inline bool Coord::isNull() const{
	return ((x==0)&&(y==0));
}

inline bool operator==(const Coord & a, const Coord & b)
{
	return a.dx()==b.dx() && a.dy()==b.dy();
}

inline bool operator!=(const Coord & a, const Coord & b)
{
	return a.dx()!=b.dx() || a.dy()!=b.dy();
}

inline Coord & Coord::operator+=( const Coord & p )
{ x+=p.dx(); y+=p.dy(); return *this; }

inline Coord & Coord::operator-=( const Coord & p )
{ x-=p.dx(); y-=p.dy(); return *this; }

inline Coord & Coord::operator*=(double c)
{ x=(int)(x*c); y=(int)(y*c); return *this; } //results are rounded but it works

inline Coord operator+( const Coord &p1, const Coord &p2 )
{ return Coord(p1.dx()+p2.dx(), p1.dy()+p2.dy()); }

inline Coord operator-( const Coord &p1, const Coord &p2 )
{ return Coord(p1.dx()-p2.dx(), p1.dy()-p2.dy()); }

#endif /* not COORD_H */
