#ifndef COORDINATE
#define COORDINATE
#include <vector>
#include <map>
#include "BaseEvent.h"
#include "ObjectRecycler.h"
#include <qstring.h>

using namespace std;

class Coordinate;
class Coordinate {
 public:
  
  static vector<Coordinate *> stdMoves;
  static map<QString, char> moveCodes;
  static void insertMoves(map<QString, Coordinate *> &);

  int distance(Coordinate * other);
  void clear();
  Coordinate(int _x = 0, int _y = 0, int _z = 0) : x(_x), y(_y), z(_z) {}
    Coordinate * go (BaseEvent * ev);
    void add(Coordinate * other);
		
    int x;
    int y;
    int z;
};


extern ObjectRecycler<Coordinate> cmm;
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
