#ifndef ROOMSEARCHNODE
#define ROOMSEARCHNODE
#include "Room.h"
class RoomCollection;  //RoomCollection is a derived class of this one, so we can't include it here

class RoomSearchNode {
	public:
		virtual RoomCollection * getRooms(char ** properties, int pos, int numProperties);
		virtual Room * insertRoom(char ** properties, int pos, int numProperties);
		virtual void setChild(char position, RoomSearchNode * child);
};

#endif
