#ifndef ROOMSEARCHNODE
#define ROOMSEARCHNODE
#include "ParseEvent.h"
#include "Room.h"
#include <AbstractRoomContainer.h>


class RoomSearchNode : public AbstractRoomContainer {
	public:
		
		virtual AbstractRoomContainer * getRooms(ParseEvent *) {return 0;}
		virtual void setChild(char, RoomSearchNode *) {;}
		virtual AbstractRoomContainer * skipDown(ParseEvent *) {return 0;}
};

#endif

#ifdef DMALLOC
#include <mpatrol.h>
#endif
