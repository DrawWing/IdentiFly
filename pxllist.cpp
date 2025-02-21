// mozliwe problemy 
// - next or prev is the same as the node

#include "pxllist.h"
//#include "compasscc.h"
#include <math.h> //"nodepxl.h"
#include <QStringList>

pxlList::pxlList()
{
  front = 0;
  back = 0;
  size = 0;
  clockwise = false;
  cyclic = true;
  back_pxl=0;
  front_pxl=0;
  lgh = 0.0;
  lghFrom = 0;
  lghTo = 0;
  wdhFrom = 0;
  wdhTo = 0;
}

//convert from list to pxlList
pxlList::pxlList(std::list<Coord> * l)
{
  std::list<Coord>::iterator iter;
  for(iter = l->begin(); iter != l->end(); iter++ )
    append(* iter);
  cyclic = true; //nie potrzebne bo i tak jest cyclic
}

//destruktor jest w klaskie Outline
//pxlList::~pxlList()
//{
//	clear();
//}

void
pxlList::clear()
{
	while(size){
		erase(back);
		//if(size<1210){
		//	int i = 0;
		//}
	}
}

void
pxlList::fromList(std::list<Coord> * l)
{
  std::list<Coord>::iterator iter;
  for(iter = l->begin(); iter != l->end(); iter++ )
    append(* iter);
}

//wczytaj pxlList z pliku tekstowego
//format pliku
void
pxlList::fromFile(std::ifstream  & inFile)
{
	//sprawdzic czy lista jest pusta
	int x;
	int y;
	while(inFile >> x >> y) {
		Coord pxl(x,y);
		push_back(pxl);
	}
}

QString pxlList::toTxtChain() const
{
    QString outString;

    nodePxl* currNode = front;
    nodePxl* nextNode = currNode->next;

    Coord pxl = currNode->pxl;
    QString startString = pxl.toTxt();
    outString.append(startString);
    outString.append(" ");


// below codes are in Cartesian coordinates
// 0 = step to the right, 2 = up, 4 = left, 6 = down,
// and the other digits are intermediate directions.
    do{
        int nextDx=nextNode->pxl.dx()-currNode->pxl.dx();
        int nextDy=nextNode->pxl.dy()-currNode->pxl.dy();

        if(nextDx == 0)
        {
            if(nextDy == -1) //dy in bitmap coordinates
                outString+="2"; //up
            else if(nextDy == 1)
                outString+="6"; //down
        }else if(nextDx == 1)
        {
            if(nextDy == -1)
                outString+="1"; //up right
            else if(nextDy == 0)
                outString+="0"; //right
            else if(nextDy == 1)
                outString+="7"; //down right
        }else if(nextDx == -1)
        {
            if(nextDy == -1)
                outString+="3"; //up left
            else if(nextDy == 0)
                outString+="4"; //left
            else if(nextDy == 1)
                outString+="5"; //down left
        }
        currNode=currNode->next;
        nextNode=nextNode->next;
    }while(currNode != front);
    return outString;
}

std::vector<Coord> pxlList::toVec() const
{
    std::vector<Coord> outVec;

    nodePxl* currNode = front;
    nodePxl* nextNode = currNode->next;

    do{
        outVec.push_back(currNode->pxl);

        currNode=currNode->next;
        nextNode=nextNode->next;
    }while(currNode != front);

    return outVec;
}

void pxlList::fromTxt(QString inString)
{
    clear();

    QStringList strLst = inString.split(" ");
    if(strLst.size() != 2)
        return;

    QString coordString = strLst.at(0);
    Coord pxl;
    pxl.fromTxt(coordString);
    append(pxl);

    QString chainString = strLst.at(1);
    for(int i = 0; i < chainString.size(); ++i)
    {
        Coord shiftPxl;
        QChar shiftChar= chainString.at(i);
        if(shiftChar == '0')
            shiftPxl.setXY(1,0);
        else if(shiftChar == '1')
            shiftPxl.setXY(1,-1);
        else if(shiftChar == '2')
            shiftPxl.setXY(0,-1);
        else if(shiftChar == '3')
            shiftPxl.setXY(-1,-1);
        else if(shiftChar == '4')
            shiftPxl.setXY(-1,0);
        else if(shiftChar == '5')
            shiftPxl.setXY(-1,1);
        else if(shiftChar == '6')
            shiftPxl.setXY(0,1);
        else if(shiftChar == '7')
            shiftPxl.setXY(1,1);
//        else
//        {
//            qDebug()<<"Ilegal chain code.";
//            continue;
//        }


        pxl += shiftPxl;
        append(pxl);
    }
}

pxlList pxlList::copy() const{
    pxlList newList;
    nodePxl * iter = back;
    do{
        Coord pxl = iter->pxl;
        newList.push_back(pxl);
        iter=iter->next;
    }while(iter != back);
    return newList;
}

bool
pxlList::isOK() const{
	if(size<3)
		return false;
	nodePxl * curr = front;
	nodePxl * prev = front->prev;
	do{
		Coord currPxl = curr->pxl;
		Coord prevPxl = prev->pxl;
		int dx = currPxl.dx() - prevPxl.dx();
		int dy = currPxl.dy() - prevPxl.dy();
		if(dx>1 || dx<-1 || dy>1 ||dy<-1)
			return false;
		prev=curr;
		curr=curr->next;
	}while(curr!=front);
	return true;
}

void 
pxlList::append(Coord pxl){
  nodePxl* newNode = new nodePxl(pxl, back, front);
  if(size){
    front->prev=newNode;
    back->next=newNode;
    back = newNode;
  }
  else
  {
    front = newNode;
    back = newNode;
    newNode->prev=newNode;
    newNode->next=newNode;
  }
  size++;
}

//powinien zwracac poprzedni nodePxl
//ale wtedy nie moze usunac ostanitego pixela
void
pxlList::erase(nodePxl* node){
	if(size==0)
		return;

	if(size>1){
		node->prev->next=node->next;
		node->next->prev=node->prev;
		if(node==front)
			front=node->next;
		if(node==back)
			back=node->prev;
	}else{
		front = 0;
		back = 0;   
	}
	size--;
	delete node;
}

//insert newPxl after the node
void 
pxlList::insert(nodePxl* node, Coord pxl){
  if(size){
    nodePxl* newNode = new nodePxl(pxl,node, node->next);
    node->next->prev=newNode;
    node->next=newNode;
  }
  else
  {
    nodePxl*	newNode = new nodePxl(pxl,back, front);
    front = newNode;
    back = newNode;
  }
  size++;

}

void 
pxlList::push_back(Coord pxl){
  nodePxl*	newNode = new nodePxl(pxl,back, front);
  if(size){
    front->prev=newNode;
    back->next=newNode;
    back = newNode;
  }
  else
  {
    front = newNode;
    back = newNode;
    newNode->prev=newNode;
    newNode->next=newNode;//jesli lista zerowa tworzenie tutaj poprawic
  }
  size++;
}

void 
pxlList::push_back(nodePxl* newNode){
  if(size){
    newNode->prev=back;
    newNode->next=front;
    front->prev=newNode;
    back->next=newNode;
    back = newNode;
  }
  else
  {
    front = newNode;
    back = newNode;
    newNode->prev=newNode;
    newNode->next=newNode;
  }
  size++;
}

void 
pxlList::preAppend(nodePxl* newNode){
  if(size){
    newNode->prev=back;
    newNode->next=front;
    front->prev=newNode;
    back->next=newNode;
    front = newNode; //!!!
  }
  else
  {
    front = newNode;
    back = newNode;
    newNode->prev=newNode;
    newNode->next=newNode;
  }
  size++;
}

void 
pxlList::swap(pxlList* newList){
  pxlList temp;

  temp.front=newList->front;
  temp.back=newList->back;
  temp.size=newList->size;
  temp.clockwise = newList->clockwise;
  temp.cyclic = newList->cyclic;
  temp.back_pxl = newList->back_pxl;
  temp.front_pxl = newList->front_pxl;

  newList->front=front;
  newList->back=back;
  newList->size=size;
  newList->clockwise = clockwise;
  newList->cyclic = cyclic;
  newList->back_pxl = back_pxl;
  newList->front_pxl = front_pxl;

  front=temp.front;
  back=temp.back;
  size=temp.size;
  clockwise = temp.clockwise;
  cyclic = temp.cyclic;
  back_pxl = temp.back_pxl;
  front_pxl = temp.front_pxl;
}

//rotate the list by angle radians
//returned list should be in real coordinates
void pxlList::rotate(double angle){
  double sin_angle = sin(angle);
  double cos_angle = cos(angle);
  nodePxl* p = back;
  do{
    double x = p->pxl.dx();
    double y = p->pxl.dy();
 //   double nx = x*cos_angle-y*sin_angle;
	//double ny = y*cos_angle+x*sin_angle;
    p->pxl.setXY( (int)(x*cos_angle-y*sin_angle), (int)(y*cos_angle+x*sin_angle) );
    p = p->next;
  }while(p != back);
}

//find top pixel
nodePxl* pxlList::top(){
  nodePxl* p=front;
  Coord pxl = p->pxl;
  int val=pxl.dy();
  nodePxl* topPxl = p;
  p = p->next;
  while(p!=front){
    pxl= p->pxl;
    if(pxl.dy() < val){
      val = pxl.dy();
      topPxl = p;
    }
    p=p->next;
  }
  return topPxl;
}

//find bottom pixel
nodePxl* pxlList::bottom(){
  nodePxl* p = front;
  Coord pxl = p->pxl;
  int val=pxl.dy();
  nodePxl* bottomPxl = p;
  p=p->next;
  while(p != front){
    pxl= p->pxl;
    if(pxl.dy()>val){
      val = pxl.dy();
      bottomPxl = p;
    }
    p=p->next;
  }
  return bottomPxl;
}

//find right pixel
nodePxl* pxlList::right(){
  nodePxl* p = front;
  Coord pxl = p->pxl;
  int val=pxl.dx();
  nodePxl* rightPxl = p;
  p=p->next;
  while(p != front){
    pxl= p->pxl;
    if(pxl.dx() > val){
      val = pxl.dx();
      rightPxl = p;
    }
    p=p->next;
  }
  return rightPxl;
}

//find right pixel look clockwise
// top ritht for counterclockwise contours
nodePxl* pxlList::rightCW(){
  nodePxl* p = front;
  Coord pxl = p->pxl;
  int val=pxl.dx();
  nodePxl* rightPxl = p;
  p=p->prev;
  while(p != front){
    pxl= p->pxl;
    if(pxl.dx() > val){
      val = pxl.dx();
      rightPxl = p;
    }
    p=p->prev;
  }
  return rightPxl;
}

////find far right pxl; top right in CCW
//nodePxl* pxlList::farRight(nodePxl *rightNode)
//{
//    nodePxl* p = rightNode;
//    Coord pxl = p->pxl;
//    int rightVal=pxl.dx();
//    int val=rightVal;
//    nodePxl* farRightPxl = p;
//    p=p->next;
//    pxl= p->pxl;
//    val = pxl.dx();
//    while(val == rightVal){
//      farRightPxl = p;
//      p=p->next;
//      pxl= p->pxl;
//      val = pxl.dx();
//    }
//    return farRightPxl;
//}

//find left pixel
nodePxl* pxlList::left(){
  nodePxl* p = front;
  Coord pxl = p->pxl;
  int val=pxl.dx();
  nodePxl* leftPxl = p;
  p=p->next;
  while(p != front){
    pxl= p->pxl;
    if(pxl.dx() < val){
      val = pxl.dx();
      leftPxl = p;
    }
    p=p->next;
  }
  return leftPxl;
}

////find top left pixel
//nodePxl* pxlList::topLeft(){
//    nodePxl* leftPxl = left();
//    nodePxl* topLeftPxl = leftPxl;

//    nodePxl* p = leftPxl;
//    Coord pxl = p->pxl;
//    int val = pxl.dy();
//    while(pxl.dy() < val){
//      val = pxl.dy();
//      topLeftPxl = p;

//      p = p->next;
//      pxl = p->pxl;;
//    }

//    p = leftPxl;
//    p = p->prev;
//    pxl = p->pxl;
//    val = pxl.dy();
//    while(pxl.dy() < val){
//      val = pxl.dy();
//      topLeftPxl = p;

//      p = p->prev;
//      pxl = p->pxl;;
//    }

//    return topLeftPxl;
//}

//return index of node in the list
//it there is no node in the list return -1
int pxlList::index(nodePxl* node) const{
  int cnt = 0;
  nodePxl* p = back;
  do{
    if(p == node)
      return cnt;
    p=p->next;
    cnt++;
  }while(p != back);
  return -1;
}

//return pointer to node at index idx
//it list is shorter than idx return 0
nodePxl* pxlList::at(int idx) const
{
  int cnt = 0;
  nodePxl* p = back;
  do{
    if(cnt == idx)
      return p;
    p=p->next;
    cnt++;
  }while(p != back);
  return 0;
}

//Find length of figure enclosed by pxlList
void pxlList::findLgh(){
    lgh = 0;
    nodePxl* from = back;
    do{
        nodePxl* to = from->next;
        do{
            dwVector vec(from->pxl, to->pxl);
            double currLgh = vec.magnitude();
            if(currLgh > lgh){
                lgh = currLgh;
                lghFrom = from;
                lghTo = to;
            }
            to = to->next;
        }while(to != back);
        from = from->next;
    }while(from != back);

    dwVector horVec(1.0, 0.0);
    dwVector lghVec1;
    lghVec1.fromCoord(lghFrom->pxl, lghTo->pxl);
    double angle1 = -angle360(lghVec1, horVec);
    dwVector lghVec2 = lghVec1.multiply(-1.0);
    double angle2 = -angle360(lghVec2, horVec);
    if(angle1*angle1 < angle2*angle2){
        lghAngle = angle1;
    }else{
        lghAngle = angle2;
    }
}

//Find width of figure enclosed by pxlList
void pxlList::findWdh(){
	if(lghFrom == 0)
		findLgh();
	pxlList rotOtl = this->copy();
	rotOtl.rotate(lghAngle);

	wdh = 0;
	nodePxl* from = rotOtl.at(index(lghFrom)); 
	do{
		nodePxl* to = rotOtl.at(index(lghTo))->next;
		do{
			if(from->pxl.dx() == to->pxl.dx()){
				dwVector vec(from->pxl, to->pxl);
				double currWdh = vec.magnitude();
				if(currWdh > wdh){
					wdh = currWdh;
					wdhFrom = from;
					wdhTo = to;
				}
			}
			to = to->next;
		}while(to != rotOtl.at(index(lghFrom)));
		from = from->next;
	}while(from != rotOtl.at(index(lghTo))->next);
	//this looks circular but it is not
	wdhFrom = this->at(rotOtl.index(wdhFrom));
	wdhTo = this->at(rotOtl.index(wdhTo));
}

int 
pxlList::xSize()
{
  nodePxl* r = right();
  nodePxl* l = left();
  return r->pxl.dx() - l->pxl.dx();
}

int 
pxlList::ySize()
{
  nodePxl* t = top();
  nodePxl* b = bottom();
  return b->pxl.dy() - t->pxl.dy();
}

void pxlList::log(std::ofstream& logFile) const
{
  nodePxl * iter = front;
  do{
    Coord pxl = iter->pxl;
    pxl.log(logFile);
    iter=iter->next;
  }while(iter!=front);
  logFile<<std::endl;
}

void pxlList::save(std::ofstream& logFile) const
{
  nodePxl * iter = front;
  do{
    Coord pxl = iter->pxl;
    pxl.save(logFile);
    iter=iter->next;
  }while(iter!=front);
  logFile<<std::endl;
}

double pxlList::length(){
  if(lghFrom == 0)
    findLgh();
  return lgh;
}

nodePxl* pxlList::lengthFrom(){
  if(lghFrom == 0)
    findLgh();
  return lghFrom;
}

nodePxl* pxlList::lengthTo(){
  if(lghTo == 0)
    findLgh();
  return lghTo;
}

double pxlList::lengthAngle(){
  if(lghFrom == 0)
    findLgh();
  return lghAngle;
}

double pxlList::width(){
  if(wdhFrom == 0)
    findWdh();
  return wdh;
}

nodePxl* pxlList::widthFrom(){
  if(wdhFrom == 0)
    findWdh();
  return wdhFrom;
}

nodePxl* pxlList::widthTo(){
  if(wdhTo == 0)
    findWdh();
  return wdhTo;
}

bool pxlList::isSane() const
{
	nodePxl * iter = back;
	do{
		if( (iter == iter->next) || (iter == iter->prev) ){
			return false;
		}
		iter=iter->next;
	}while(iter!=back);
	return true;
}

//find in the pixel list one closes to thePxl
nodePxl* pxlList::findClose(Coord toPxl)
{
    nodePxl* p=front;
    Coord pxl = p->pxl;
    dwVector vec(pxl, toPxl);
    double dist = vec.magnitude();
    nodePxl* outPxl = p;
    p = p->next;
    while(p!=front){
      pxl= p->pxl;
      vec.fromCoord(pxl, toPxl);
      double val = vec.magnitude();
      if( val < dist){
        dist = val;
        outPxl = p;
      }
      p=p->next;
    }
    return outPxl;
}

//find semilendmarks around the whole outline
std::vector<Coord> pxlList::semilandmarks(unsigned length, Coord toPxl)
{
    std::vector<Coord> outVector;
    if(size < length)
        return outVector;
    nodePxl* first = findClose(toPxl);
    Coord pxl = first->pxl;
    outVector.push_back(pxl);
    int prevDist = 0;

    nodePxl* p = first;

    double interval = (double)size/length;
    for(unsigned i = 1; i < length; ++i)
    {
        int theDist = interval*i;
        int theInterval = theDist-prevDist;
        for(int j = 0; j < theInterval; ++j)
        {
            p = p->next;
        }
        pxl = p->pxl;
        outVector.push_back(pxl);
        prevDist+=theInterval;
    }
    return outVector;
}

//find semilendmarks between two points
std::vector<Coord> pxlList::semilandmarks2points(unsigned length, Coord fromPxl, Coord toPxl)
{
    std::vector<Coord> outVector;

    nodePxl* node1 = findClose(fromPxl);
    Coord pxl = node1->pxl;
    outVector.push_back(pxl);
    int prevDist = 0;

    nodePxl* node2 = findClose(toPxl);

    nodePxl* p = node1;

    int dist = distance(node1, node2);
    if(dist < (int)length)
        return outVector;

    double interval = (double)dist/length;
    for(unsigned i = 1; i < length; ++i)
    {
        int theDist = interval*i;
        int theInterval = theDist-prevDist;
        for(int j = 0; j < theInterval; ++j)
        {
            p = p->next;
        }
        pxl = p->pxl;
        outVector.push_back(pxl);
        prevDist+=theInterval;
    }

    pxl = node2->pxl;
    outVector.push_back(pxl);

    return outVector;
}

int pxlList::distance(nodePxl* fromNode, nodePxl* toNode) const
{
    int dist = 0;

    nodePxl* theNode = fromNode;
    while(theNode != toNode)
    {
        theNode = theNode->next;
        ++dist;
        if(dist > (int)size)
            return -1;
    }
    return dist;
}
