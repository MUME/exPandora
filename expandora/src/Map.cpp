#include "Map.h"
#include <stdexcept>
using namespace std;

/**
 * doesn't modify c
 */
bool Map::defined(Coordinate * _c) {
	Coordinate * c = cmm.activate();
	c->x = _c->x, c->y = _c->y, c->z=_c->z;
	vector<vector<vector<Room *> > > & segment = getSegment(c);
	try {
		if (segment.at(c->x).at(c->y).at(c->z) != 0) {
			cmm.deactivate(c);
			return true;
		}
		else {
			cmm.deactivate(c);
			return false;
		}
	} catch (out_of_range e) {
		cmm.deactivate(c);
		return false;
	}
}


Room * Map::get(Coordinate *c) {
	if (!defined(c)) return 0;
	else {
		Coordinate * cm = cmm.activate();
		cm->add(c);
		vector<vector<vector<Room *> > > & segment = getSegment(cm);
		Room * ret = segment[cm->x][cm->y][cm->z];
		cmm.deactivate(cm);
		return ret;
	}
}

/**
 * modifies c so that it fits the found segment
 */
vector<vector<vector<Room *> > > & Map::getSegment(Coordinate * c) {
	int xs, ys, zs;
	if(c->x < 0) xs = 0, c->x*=-1;
	else xs = 1;
	if(c->y < 0) ys = 0, c->y*=-1;
	else ys = 1;
	if(c->z < 0) zs = 0, c->z*=-1;
	else zs = 1;
	return map[xs][ys][zs];
}

	
	
/**
 * doesn't modify c
 */		
void Map::set(Coordinate *cm, Room *room) {
	Coordinate * c = cmm.activate();
	c->add(cm);
	vector<vector<vector<Room *> > > & segment = getSegment(c);
	if (segment.capacity() <= (unsigned int)c->x) segment.resize((unsigned int)c->x + 1);
	if (segment[(unsigned int)c->x].capacity() <= (unsigned int)c->y) segment[(unsigned int)c->x].resize((unsigned int)c->y + 1);
	if (segment[(unsigned int)c->x][(unsigned int)c->y].capacity() <= (unsigned int)c->z) segment[(unsigned int)c->x][(unsigned int)c->y].resize((unsigned int)c->z + 1);
	segment[c->x][c->y][c->z] = room;
	cmm.deactivate(c);
}

/**
 * gets a new coordinate but doesn't return the old one ... should probably be changed ...
 */
Coordinate * Map::setNearest(Coordinate *_c, Room *room) {
	Coordinate * c = getNearestFree(_c);
	set(c, room);
	return c;
}
		
Coordinate * Map::getNearestFree(Coordinate * p) {
	Coordinate * c = cmm.activate();
	int distance = 1;
	int x  = 0, y = 0, z = 0;
	while(1) {
		for (x = 0; x < distance; x++) {
			for (y = 0; x+y < distance; y++) {
				for (z = 0; x+y+z < distance; z++) {
					c->x=p->x+x, c->y=p->y+y, c->z=p->z+z;
					if (!defined(c)) return c;
					c->z=p->z-z;
					if (!defined(c)) return c;
					c->y=p->y-y, c->z=p->z+z;						
					if (!defined(c)) return c;
					c->z=p->z-z;
					if (!defined(c)) return c;
					c->x=p->x-x, c->y=p->y+y;c->z=p->z+z;
					if (!defined(c)) return c;
					c->z=p->z-z;
					if (!defined(c)) return c;
					c->y=p->y-y, c->z=p->z+z;						
					if (!defined(c)) return c;
					c->z=p->z-z;
					if (!defined(c)) return c;
				}
			}
		}
		distance++;
	}
	return 0;
}
