#ifndef ROOMSEARCHNODE
#define ROOMSEARCHNODE
#include "ParseEvent.h"
#include "Room.h"


class RoomSearchNode {
	public:
		virtual int numRooms() {return -1;}
		virtual RoomSearchNode * merge(RoomSearchNode * other) {return other;}
		virtual RoomSearchNode * getRooms(ParseEvent * event) {return 0;}
		virtual Room * insertRoom(ParseEvent * event) {return 0;}
		
		virtual void setChild(char position, RoomSearchNode * child) {;}
		virtual RoomSearchNode * skipDown(ParseEvent * event) {return 0;}
};

#endif
