#ifndef COORDINATE
#define COORDINATE

#include "BaseEvent.h"
#include "ObjectRecycler.h"
#include <vector>
using namespace std;

class Coordinate {
	public:
		int distance(Coordinate * other);
		void clear();
		Coordinate(int _x = 0, int _y = 0, int _z = 0);
		Coordinate * go (BaseEvent * ev);
		void add(Coordinate * other);
		
		int x;
		int y;
		int z;
};

extern vector<Coordinate *> stdMoves;
extern ObjectRecycler<Coordinate> cmm;
#endif
