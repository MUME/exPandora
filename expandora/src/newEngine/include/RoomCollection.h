#ifndef ROOMCOLLECTION
#define ROOMCOLLECTION
#include <set>
#include "ParseEvent.h"
#include "AbstractRoomContainer.h"
#include "Room.h"
#include "ObjectRecycler.h"

using namespace std;

class RoomCollection : public AbstractRoomContainer {
	public:
		RoomCollection() {};
		virtual ~RoomCollection() {} // implement it !
		virtual int numRooms() {return rooms.size();} //let's hope this is implemented in some efficient way ...
		virtual Room * insertRoom(ParseEvent * event);
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
#endif


#ifdef DMALLOC
#include <dmalloc.h>
#endif
