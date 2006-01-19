#ifndef COORDINATE
#define COORDINATE
#include <vector>
#include <map>
#include <qstring.h>
#include "BaseEvent.h"
#include "ObjectRecycler.h"




class Coordinate
  {
  public:

    static std::vector<Coordinate> stdMoves;
    static std::map<QString, uint> moveCodes;
    static void insertMoves(std::map<QString, Coordinate> &);
    
    bool operator== (Coordinate & other);
    bool operator!= (Coordinate & other);
    void operator+= (Coordinate & other); 
    void operator-= (Coordinate & other);
    Coordinate operator+ (Coordinate & other);
    Coordinate operator- (Coordinate & other);
    
    int distance(Coordinate & other);
    void clear();
    Coordinate(int in_x = 0, int in_y = 0, int in_z = 0) : x(in_x), y(in_y), z(in_z)
    {}
    Coordinate go (BaseEvent * ev);
    

    int x;
    int y;
    int z;
  };


#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif


