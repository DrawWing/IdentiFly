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

#ifndef PXLLIST_H
#define PXLLIST_H

#include "coord.h"
#include "nodepxl.h"
#include <fstream> 
#include <list>
#include <vector>
#include <QString>

class pxlList  
{
public:
	pxlList();
	pxlList(std::list<Coord> *);
	//~pxlList();
	void clear(); //dealocate the pixelNodes
	void fromList(std::list<Coord> *);
	void fromFile(std::ifstream  & inFile);
    QString toTxtChain() const;
    std::vector<Coord> toVec() const;
    void fromTxt(QString inString);

	pxlList copy() const;
	bool isOK() const;
	void log(std::ofstream&) const;
	void save(std::ofstream&) const;
	void append(Coord);//to be removed
	void push_back(Coord);
	void push_back(nodePxl*);
	void preAppend(nodePxl*);
	void insert(nodePxl*, Coord);
	void erase(nodePxl*);
	void swap(pxlList*);
	void rotate(double angle); //angle in radians //should be in realCoord!!!!
	nodePxl* top(); //find top pxl
	nodePxl* bottom(); //find bottom pxl
    nodePxl* right(); //find right pxl
    nodePxl* rightCW(); //find right pxl looking clocwise
//    nodePxl* farRight(nodePxl* rightNode); //find far right pxl; top right in CCW
    nodePxl* left(); //find left pxl
//    nodePxl* topLeft(); //find top left pxl
    int index(nodePxl*) const;
    nodePxl* at(int) const;
	double length(); //maximum lenght of the outline (slow)
	nodePxl* lengthFrom();
	nodePxl* lengthTo();
	double lengthAngle(); //angle between length and horizontal
	double width(); //width of the outline perpendicular to length
	nodePxl* widthFrom();
	nodePxl* widthTo();
	int xSize(); //dlugosc 
	int ySize(); //wysokosc

	nodePxl* front;//may be unnecessary in cyclic lists
	nodePxl* back;
	//private:
	unsigned size;
	bool clockwise;
	bool cyclic;
    nodePxl *front_pxl; //bez sensu powtorzenie front????????
    nodePxl *back_pxl;
	bool isSane() const;
    std::vector<Coord> semilandmarks(unsigned length, Coord toPxl);
    std::vector<Coord> semilandmarks2points(unsigned length, Coord fromPxl, Coord toPxl);
    int distance(nodePxl* fromNode, nodePxl* toNode) const;
private:
	double lgh;
	nodePxl* lghFrom; //wyznaczaja dlugosc
	nodePxl* lghTo;
	double lghAngle;
	void findLgh();
	double wdh;
	nodePxl* wdhFrom; //wyznaczaja szerokosc
	nodePxl* wdhTo;
	void findWdh();
    nodePxl* findClose(Coord toPxl); //find in the pixel list one closes to thePxl
};

#endif // !defined(PXLLIST_H)