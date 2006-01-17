#ifndef ROOMCOLLECTION
#define ROOMCOLLECTION
#include <set>
#include "Room.h"
#include "ParseEvent.h"
#include "AbstractRoomContainer.h"
#include "ObjectRecycler.h"

using namespace std;

class RoomCollection : public AbstractRoomContainer {
	public:
		RoomCollection() {};
		virtual ~RoomCollection() {} // implement it !
		virtual int numRooms() {return rooms.size();} //let's hope this is implemented in some efficient way ...
		Room * createRoom(ParseEvent * event);
		RoomCollection * insertRoom(ParseEvent *) {return this;}
		RoomCollection * filterByOptionals(ParseEvent * event);
		void addRoom(Room * room);
		virtual RoomCollection * merge(AbstractRoomContainer * other);
		void clear();
		void removeRoom(Room * room); 
		Room * matchOne(ParseEvent * ev, int tolerance);	// narrows the Collection by the event and returns a Room if exactly one is left otherwise returns 0

		set<Room *>::iterator begin() {return rooms.begin();};
		set<Room *>::iterator end() {return rooms.end();};

		virtual RoomCollection * getRooms(ParseEvent *);
		virtual RoomCollection * skipDown(ParseEvent *);
	private:
		set<Room *> rooms;
		
};

extern ObjectRecycler<RoomCollection> rcmm; // room collection memory management ... :(



#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif