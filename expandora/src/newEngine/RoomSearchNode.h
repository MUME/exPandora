#ifndef ROOMSEARCHNODE
#define ROOMSEARCHNODE
#include "ParseEvent.h"
#include "Room.h"


class RoomSearchNode {
	public:
		virtual int numRooms() {return -1;}
		virtual RoomSearchNode * merge(RoomSearchNode * other) {return other;}
		virtual RoomSearchNode * getRooms(ParseEvent *) {return 0;}
		virtual Room * insertRoom(ParseEvent *) {return 0;}
		
		virtual void setChild(char, RoomSearchNode *) {;}
		virtual RoomSearchNode * skipDown(ParseEvent *) {return 0;}
};

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
