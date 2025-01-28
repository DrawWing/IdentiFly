//bitmap coordinate system, outlines shifted by 0.5 pixel towards top and right

#include "outline.h"

Outline::Outline()
{
}

Outline::Outline(dwImage *img)
{
    fromImg(img);
}

//based on find_outline_pixels() from autotrace pxl-outline.c
//based on find_one_outline() from autotrace pxl-outline.c
void Outline::fromImg(dwImage *img)
{
    maxOtl = 0; //Longest outline is not known.
    maxCCOtl = 0; //Longest counter clockwise outline is not known.

    std::list< pxlList >::iterator iter;
    for( iter = otls.begin(); iter != otls.end(); iter++){
        iter->clear();
    }
    otls.clear();

    image = img;
    shadow = *img; //tmpImg;
    shadow.detach();
    shadow.fill(0);
    shadow.setColor( 0, 0);

    //white margin around image is required
    unsigned border = 1;
    img->addMargin(border);

    for (unsigned row = border; row < img->height()-border; row++)
        for (unsigned col = border; col < img->width()-border; col++)
        {
            unsigned theColor = *(img->scanLine(row) + col);
            if(img->color(theColor) == black)
            {
                if((is_outline_edge(TOP, row, col))&&(!is_marked_edge(TOP, row, col)))
                {
                    findOutline(row, col, false);
                }
                if((is_outline_edge(BOTTOM, row, col))&&(!is_marked_edge(BOTTOM, row, col)))
                {
                    findOutline(row, col, true);
                }
            }
        }
}

//based on find_one_outline() from autotrace pxl-outline.c
void Outline::findOutline(unsigned row, unsigned col, bool clockwise)
{
    pxlList otl;
    if(clockwise){
        otl.clockwise = true;
        edge = BOTTOM;
        pos.setXY(col+1,row);
    }else{
        otl.clockwise = false;
        edge = TOP;
        pos.setXY(col,row-1); //outline jest przesuniety wzgledem czarnej plamy
    }
    do{
        otl.push_back(pos);
        unsigned marked = *(shadow.scanLine(row) + col);
        marked|= 1 << edge;
        *(shadow.scanLine(row) + col)=marked;

        if(clockwise)
            findNextCW(&row,&col);
        else
            findNext(&row,&col);

    }while (edge != NO_EDGE);
    otls.push_back(otl);
}

//based on is_outline_edge() from autotrace pxl-outline.c
bool Outline::is_outline_edge (edge_type e, unsigned row, unsigned col)
{
    unsigned color = *(image->scanLine(row) + col);
    if (image->color(color) != black)
        return false;

    switch (e){
    case LEFT:
        color = *(image->scanLine(row) + col-1);
        return (image->color(color) != black);

    case TOP:
        color = *(image->scanLine(row-1) + col);
        return (image->color(color) != black);

    case RIGHT:
        color = *(image->scanLine(row) + col+1);
        return (image->color(color) != black);

    case BOTTOM:
        color = *(image->scanLine(row+1) + col);
        return (image->color(color) != black);

    case NO_EDGE:
    default:
        ;
    }
    return false; //* NOT REACHED
}

//based on is_marked_edge() from autotrace pxl-outline.c
bool Outline::is_marked_edge (edge_type e, unsigned row, unsigned col)
{
    if (e == NO_EDGE)
        return false;
    unsigned mark = *(shadow.scanLine(row) + col);
    return((mark&(1 << e))!=0);
}

//based on NextPoint() from autotrace pxl-outline.c
//direction CoutnerClockwise - inside outline - white surrounded by black
void Outline::findNext(unsigned *row, unsigned* col)
{
    if(edge == TOP)
    {
        // WEST offset (-1,0)
        if((!is_marked_edge(TOP,*row,*col-1)
             && is_outline_edge(TOP,*row,*col-1)))
        {
            (*col)--;
            pos.setX(*col);
            pos.setY(*row-1); //offset (-1,-1)
            return;
        }
        // NORTHWEST offset (-1,-1)
        if((!is_marked_edge(RIGHT,*row-1,*col-1)
             && is_outline_edge(RIGHT,*row-1,*col-1)) &&
            !(is_marked_edge(LEFT,*row-1,*col) && is_marked_edge(TOP, *row,*col-1)) &&
            !(is_marked_edge(BOTTOM,*row-1,*col) && is_marked_edge(RIGHT, *row,*col-1)))
        {
            edge = RIGHT;
            pos.setX(*col);
            (*col)--;
            (*row)--;
            //pos.setX(*col+1);
            pos.setY(*row-1); //offset (0,-2)
            return;
        }
        //offset (0,0)
        if ((!is_marked_edge(LEFT,*row,*col)
             && is_outline_edge(LEFT,*row,*col)))
        {
            edge = LEFT;
            pos.setX(*col);
            pos.setY(*row); //offset (0,0)
            return;
        }
        edge = NO_EDGE;
        return;
    }
    else if(edge == RIGHT)
    {
        // NORTH offset(0,-1)
        if((!is_marked_edge(RIGHT,*row-1,*col)
             && is_outline_edge(RIGHT,*row-1,*col)))
        {
            (*row)--;
            pos.setX(*col+1);
            pos.setY(*row-1); //offset (1,-2)
            return;
        }
        // NORTHEAST offset(1,-1)
        if((!is_marked_edge(BOTTOM,*row-1,*col+1)
             && is_outline_edge(BOTTOM,*row-1,*col+1)) &&
            !(is_marked_edge(LEFT,*row,*col+1) && is_marked_edge(BOTTOM, *row-1,*col)) &&
            !(is_marked_edge(TOP,*row,*col+1) && is_marked_edge(RIGHT, *row-1,*col)))
        {
            edge = BOTTOM;
            (*col)++;
            (*row)--;
            pos.setX(*col+1);
            pos.setY(*row); //offset (2,-1)
            return;
        }
        //offset (0,0)
        if ((!is_marked_edge(TOP,*row,*col)
             && is_outline_edge(TOP,*row,*col)))
        {
            edge = TOP;
            pos.setX(*col);
            pos.setY(*row-1); //offset (0,-1)
            return;
        }
        edge = NO_EDGE;
        return;
    }
    else if(edge == BOTTOM)
    {
        // case BOTTOM:
        // EAST offset (1,0)
        if((!is_marked_edge(BOTTOM,*row,*col+1)
             && is_outline_edge(BOTTOM,*row,*col+1)))
        {
            (*col)++;
            pos.setX(*col+1);
            pos.setY(*row); //offset (2,0)
            return;
        }
        // SOUTHEAST offset (1,1)
        if((!is_marked_edge(LEFT,*row+1,*col+1)
             && is_outline_edge(LEFT,*row+1,*col+1)) &&
            !(is_marked_edge(TOP,*row+1,*col) && is_marked_edge(LEFT, *row,*col+1)) &&
            !(is_marked_edge(RIGHT,*row+1,*col) && is_marked_edge(BOTTOM,*row,*col+1)))
        {
            edge = LEFT;
            (*col)++;
            (*row)++;
            pos.setX(*col);
            pos.setY(*row); //offset (1,1)
            return;
        }
        //offset (0,0)
        if ((!is_marked_edge(RIGHT,*row,*col)
             && is_outline_edge(RIGHT,*row,*col)))
        {
            edge = RIGHT;
            pos.setX(*col+1);
            pos.setY(*row-1); //offset (1,-1)
            return;
        }
        edge = NO_EDGE;
        return;
    }
    else if(edge == LEFT)
    {
        // case LEFT:
        // SOUTH offset (0,1)
        if((!is_marked_edge(LEFT,*row+1,*col)
             && is_outline_edge(LEFT,*row+1,*col)))
        {
            (*row)++;
            pos.setX(*col);
            pos.setY(*row); //offset (0,1)
            return;
        }
        // SOUTHWEST offset (-1,1)
        if((!is_marked_edge(TOP,*row+1,*col-1)
             && is_outline_edge(TOP,*row+1,*col-1)) &&
            !(is_marked_edge(RIGHT,*row,*col-1) && is_marked_edge(TOP, *row+1,*col)) &&
            !(is_marked_edge(BOTTOM, *row,*col-1) && is_marked_edge(LEFT, *row+1,*col)))
        {
            edge = TOP;
            pos.setY(*row); //offset (-1,0)
            (*col)--;
            (*row)++;
            pos.setX(*col);
            //pos.setY(*row-1);
            return;
        }
        //offset (0,0)
        if ((!is_marked_edge(BOTTOM,*row,*col)
             && is_outline_edge(BOTTOM,*row,*col)))
        {
            edge = BOTTOM;
            pos.setX(*col+1);
            pos.setY(*row); //offset (1,0)
            return;
        }
        edge = NO_EDGE;
    }
    edge = NO_EDGE;
}

//based on NextPoint() from autotrace pxl-outline.c
//direction ClockWise - inside outline - white surrounded by black
void Outline::findNextCW(unsigned *row, unsigned* col)
{
    if(edge == TOP)
    {
        // case TOP:
        //offset (0,0)
        if ((!is_marked_edge(LEFT,*row,*col)
             && is_outline_edge(LEFT,*row,*col)))
        {
            edge = LEFT;
            pos.setX(*col);
            pos.setY(*row); //offset (0,0)
            return;
        }
        // WEST offset (-1,0)
        if((!is_marked_edge(TOP,*row,*col-1)
             && is_outline_edge(TOP,*row,*col-1)))
        {
            (*col)--;
            pos.setX(*col);
            pos.setY(*row-1); //offset (-1,-1)
            return;
        }
        // NORTHWEST offset (-1,-1)
        if((!is_marked_edge(RIGHT,*row-1,*col-1)
             && is_outline_edge(RIGHT,*row-1,*col-1)))
        {
            edge = RIGHT;
            pos.setX(*col);
            (*col)--;
            (*row)--;
            //pos.setX(*col+1);
            pos.setY(*row-1); //offset (0,-2)
            return;
        }
        edge = NO_EDGE;
        return;
    }
    else if(edge == RIGHT)
    {
        // case RIGHT:
        //offset (0,0)
        if ((!is_marked_edge(TOP,*row,*col)
             && is_outline_edge(TOP,*row,*col)))
        {
            edge = TOP;
            pos.setX(*col);
            pos.setY(*row-1); //offset (0,-1)
            return;
        }
        // NORTH offset(0,-1)
        if((!is_marked_edge(RIGHT,*row-1,*col)
             && is_outline_edge(RIGHT,*row-1,*col)))
        {
            (*row)--;
            pos.setX(*col+1);
            pos.setY(*row-1); //offset (1,-2)
            return;
        }
        // NORTHEAST offset(1,-1)
        if((!is_marked_edge(BOTTOM,*row-1,*col+1)
             && is_outline_edge(BOTTOM,*row-1,*col+1)))
        {
            edge = BOTTOM;
            (*col)++;
            (*row)--;
            pos.setX(*col+1);
            pos.setY(*row); //offset (2,-1)
            return;
        }
        edge = NO_EDGE;
        return;
    }
    else if(edge == BOTTOM)
    {
        // case BOTTOM:
        //offset (0,0)
        if ((!is_marked_edge(RIGHT,*row,*col)
             && is_outline_edge(RIGHT,*row,*col)))
        {
            edge = RIGHT;
            pos.setX(*col+1);
            pos.setY(*row-1); //offset (1,-1)
            return;
        }
        // EAST offset (1,0)
        if((!is_marked_edge(BOTTOM,*row,*col+1)
             && is_outline_edge(BOTTOM,*row,*col+1)))
        {
            (*col)++;
            pos.setX(*col+1);
            pos.setY(*row); //offset (2,0)
            return;
        }
        // SOUTHEAST offset (1,1)
        if((!is_marked_edge(LEFT,*row+1,*col+1)
             && is_outline_edge(LEFT,*row+1,*col+1)))
        {
            edge = LEFT;
            (*col)++;
            (*row)++;
            pos.setX(*col);
            pos.setY(*row); //offset (1,1)
            return;
        }
        edge = NO_EDGE;
        return;
    }
    else if(edge == LEFT)
    {
        // case LEFT:
        //offset (0,0)
        if ((!is_marked_edge(BOTTOM,*row,*col)
             && is_outline_edge(BOTTOM,*row,*col)))
        {
            edge = BOTTOM;
            pos.setX(*col+1);
            pos.setY(*row); //offset (1,0)
            return;
        }
        // SOUTH offset (0,1)
        if((!is_marked_edge(LEFT,*row+1,*col)
             && is_outline_edge(LEFT,*row+1,*col)))
        {
            (*row)++;
            pos.setX(*col);
            pos.setY(*row); //offset (0,1)
            return;
        }
        // SOUTHWEST offset (-1,1)
        if((!is_marked_edge(TOP,*row+1,*col-1)
             && is_outline_edge(TOP,*row+1,*col-1)))
        {
            edge = TOP;
            pos.setY(*row); //offset (-1,0)
            (*col)--;
            (*row)++;
            pos.setX(*col);
            //pos.setY(*row-1);
            return;
        }
        edge = NO_EDGE;
    }
    edge = NO_EDGE;
}

std::list< pxlList > * Outline::pixels()
{
    return &otls;
}

// Find the longes outline.
// It cen be done in constructor, probably faster.
void Outline::findMaxOtl()
{
    unsigned maxCWLgh=0;
    unsigned maxCCLgh=0;
    std::list< pxlList >::iterator iter = otls.begin();

    for( ; iter != otls.end(); iter++ ){
        pxlList o = * iter;
        if(o.clockwise){
            if(o.size > maxCWLgh){
                maxCWLgh=o.size;
                maxCWOtl=&(*iter);
            }
        }else{
            if(o.size > maxCCLgh){
                maxCCLgh=o.size;
                maxCCOtl=&(*iter);
            }
        }
    }
    if(maxCWLgh > maxCCLgh)
        maxOtl = maxCWOtl;
    else
        maxOtl = maxCCOtl;
}

//Retrun the longest outline.
pxlList * Outline::maxOutline()
{
    if(maxOtl)
        return maxOtl;
    else{
        findMaxOtl();
        return maxOtl;
    }
}

//Return the longes counter closckwise outline. (black object on white bacground)
pxlList * Outline::maxCCOutline()
{
    if(maxCCOtl)
        return maxCCOtl;
    else{
        findMaxOtl();
        return maxCCOtl;
    }
}

void 
Outline::delShort(unsigned thd)
{
    std::list< pxlList >::iterator iter;
    for( iter = otls.begin(); iter != otls.end(); ){
        pxlList o = * iter;
        if(o.size < thd){
            iter = otls.erase(iter);
        }else{
            iter++;
        }
    }
}

// Destructor 
// Dealocate the pxlLists.
Outline::~Outline()
{
    std::list< pxlList >::iterator iter;
    for( iter = otls.begin(); iter != otls.end(); iter++){
        iter->clear();
    }
}
