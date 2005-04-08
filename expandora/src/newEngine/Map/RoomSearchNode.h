#ifndef ROOMSEARCHNODE
#define ROOMSEARCHNODE
#include "ParseEvent.h"
#include "Room.h"
#include <AbstractRoomContainer.h>


class RoomSearchNode : AbstractRoomContainer {
	public:
		
		virtual RoomSearchNode * getRooms(ParseEvent *) {return 0;}
		virtual void setChild(char, RoomSearchNode *) {;}
		virtual RoomSearchNode * skipDown(ParseEvent *) {return 0;}
};

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
