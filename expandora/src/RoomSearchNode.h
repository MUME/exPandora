#ifndef ROOMSEARCHNODE
#define ROOMSEARCHNODE
#include "Room.h"
#include <vector>
using namespace std;

class RoomCollection;  //RoomCollection is a derived class of this one, so we can't include it here

class RoomSearchNode {
	public:
		virtual RoomCollection * getRooms(vector<char *> & properties, int pos);
		virtual Room * insertRoom(vector<char *> & properties, int pos);
		virtual void setChild(char position, RoomSearchNode * child);
};

#endif
