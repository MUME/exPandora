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
		virtual ~RoomCollection();
		virtual int numRooms() {return rooms.size();} //let's hope this is implemented in some efficient way ...
		Room * insertRoom(ParseEvent * event);
		RoomCollection * filterByOptionals(ParseEvent * event);
		void addRoom(Room * room);
		RoomCollection * merge(RoomSearchNode * other);
		void clear();
		set<Room *> & getRooms() {return rooms;}
		void removeRoom(Room * room); 
	
		Room * matchOne(ParseEvent * ev);	// narrows the Collection by the event and returns a Room if exactly one is left otherwise returns 0

		
		virtual RoomSearchNode * getRooms(ParseEvent * event){return this;}
		virtual RoomCollection * skipDown(ParseEvent * event){return this;}
	private:
		set<Room *> rooms;
		
};

extern ObjectRecycler<RoomCollection> rcmm; // room collection memory management ... :(
#endif

