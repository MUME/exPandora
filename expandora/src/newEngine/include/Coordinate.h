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

    static std::vector<Coordinate *> stdMoves;
    static std::map<QString, char> moveCodes;
    static void insertMoves(std::map<QString, Coordinate *> &);
    

    int distance(Coordinate * other);
    void clear();
    Coordinate(int in_x = 0, int in_y = 0, int in_z = 0) : x(in_x), y(in_y), z(in_z)
    {}
    Coordinate * go (BaseEvent * ev);
    void add
      (Coordinate * other);

    int x;
    int y;
    int z;
  };


extern ObjectRecycler<Coordinate> cmm;


#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif


