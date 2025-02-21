#ifndef OUTLINE_H
#define OUTLINE_H

#include <list> 
#include "coord.h" 
#include "dwImg.h"

typedef enum
{
	TOP = 1, LEFT = 2, BOTTOM = 3, RIGHT = 0, NO_EDGE = 4
} edge_type;

class Outline  
{
public:
	Outline();
	Outline(dwImage*);
	~Outline();
	void fromImg(dwImage*);
	std::list< pxlList > * pixels();
	pxlList * maxOutline() ;
	pxlList * maxCCOutline() ;
	void delShort(unsigned thd);

private:
    static const QRgb black = 0xFF000000;
	std::list< pxlList > otls;
	pxlList*  maxOtl; //longest outline
	pxlList*  maxCWOtl; //longest clockwise outline
	pxlList*  maxCCOtl; //longest counter clockwise outline
	Coord pos;
	edge_type edge;
	dwImage* image;
	dwImage shadow;

	void findOutline(unsigned row, unsigned col, bool clockwise);
	void findMaxOtl() ;
    void findNextCW(unsigned *, unsigned*);
    void findNext(unsigned *, unsigned*);
    bool is_outline_edge (edge_type, unsigned, unsigned);
    bool is_marked_edge (edge_type, unsigned, unsigned);
};

#endif // !defined(OUTLINE_H)
