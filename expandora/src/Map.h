#ifndef MAP
#define MAP

#include "Coordinate.h"
#include "Room.h"
#include <vector>
using namespace std;

class Map {
	public:
		bool defined(Coordinate *c);
		Coordinate * setNearest(Coordinate *c, Room *room);
		Room * get(Coordinate *c);
		void remove(Coordinate * c) {set(c, 0);}
	private:
		
		vector<vector<vector<Room *> > > & getSegment(Coordinate * c);
		void set(Coordinate *c, Room *room);
		Coordinate * getNearestFree(Coordinate * c);
		vector<vector<vector<Room *> > > map [2][2][2]; 
};

#endif
