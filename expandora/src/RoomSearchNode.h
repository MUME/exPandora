#ifndef ROOMSEARCHNODE
#define ROOMSEARCHNODE
#include "ParseEvent.h"
#include "Room.h"

class RoomCollection;  //RoomCollection is a derived class of this one, so we can't include it here

class RoomSearchNode {
	public:
		virtual RoomCollection * getRooms(ParseEvent * event);
		virtual Room * insertRoom(ParseEvent * event);
		virtual void setChild(char position, RoomSearchNode * child);
		virtual RoomCollection * skipDown(ParseEvent * event);
};

#endif
