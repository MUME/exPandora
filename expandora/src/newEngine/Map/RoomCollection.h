#ifndef ROOMCOLLECTION
#define ROOMCOLLECTION
#include <set>
#include "Room.h"
#include "ParseEvent.h"
#include "AbstractRoomContainer.h"
#include "ObjectRecycler.h"


class RoomCollection : public AbstractRoomContainer {
	public:
		RoomCollection() {};
		virtual ~RoomCollection() {} // implement it !
		virtual int numRooms() {return rooms.size();} //let's hope this is implemented in some efficient way ...
		Room * createRoom(ParseEvent * event);
		RoomCollection * insertRoom(ParseEvent *) {return this;}
		
		void addRoom(Room * room);
		virtual RoomCollection * merge(AbstractRoomContainer * other);
		void clear();
		void removeRoom(Room * room); 

		std::set<Room *>::iterator begin() {return rooms.begin();}
		std::set<Room *>::iterator end() {return rooms.end();}

		virtual RoomCollection * getRooms(ParseEvent *);
		virtual RoomCollection * skipDown(ParseEvent *);
	private:
		std::set<Room *> rooms;
		
};

extern ObjectRecycler<RoomCollection> rcmm; // room collection memory management ... :(



#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
