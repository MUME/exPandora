#ifndef ROOMCOLLECTION
#define ROOMCOLLECTION
#include <set>
#include "ParseEvent.h"
#include "RoomSearchNode.h"
#include "Room.h"
#include "ObjectRecycler.h"

using namespace std;

class RoomCollection : public RoomSearchNode {
	public:
		RoomCollection() {};
		virtual ~RoomCollection() {} // implement it !
		virtual int numRooms() {return rooms.size();} //let's hope this is implemented in some efficient way ...
		Room * insertRoom(ParseEvent * event);
		RoomCollection * filterByOptionals(ParseEvent * event);
		void addRoom(Room * room);
		RoomCollection * merge(RoomSearchNode * other);
		void clear();
		void removeRoom(Room * room); 
		Room * matchOne(ParseEvent * ev);	// narrows the Collection by the event and returns a Room if exactly one is left otherwise returns 0

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
