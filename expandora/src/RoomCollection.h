#ifndef ROOMCOLLECTION
#define ROOMCOLLECTION
#include <list>
#include "ParseEvent.h"
#include "RoomSearchNode.h"
#include "Room.h"
#include "ObjectRecycler.h"

using namespace std;

class RoomCollection {
	public:
		RoomCollection() {};
		Room * insertRoom(ParseEvent * event);
		RoomCollection * filterByOptionals(ParseEvent * event);
		void addRoom(Room * room);
		void merge(RoomCollection * other);
		void clear();
		list<Room *> & getRooms() {return rooms;}
	private:
		list<Room *> rooms;
		
};

extern ObjectRecycler<RoomCollection> rcmm; // room collection memory management ... :(
#endif

