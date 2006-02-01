#ifndef MAP
#define MAP

#include "Coordinate.h"
#include "Room.h"
#include <vector>


/**
 * The Map stores the geographic relations of rooms to each other
 * it doesn't store the search tree. The Map class is only used by the 
 * RoomAdmin, which also stores the search tree
 */
class Map {
	public:
		bool defined(Coordinate &c);
		Coordinate setNearest(Coordinate &c, Room *room);
		Room * get(Coordinate &c);
		void remove(Coordinate & c) {set(c, 0);}
	private:
		
		std::vector<std::vector<std::vector<Room *> > > & getSegment(Coordinate & c);
		void set(Coordinate &c, Room *room);
		Coordinate getNearestFree(Coordinate & c);
		std::vector<std::vector<std::vector<Room *> > > map [2][2][2]; 
};
#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif

#ifdef DMALLOC
#include <mpatrol.h>
#endif
