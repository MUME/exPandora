#include "RoomCollection.h"
#include "RoomAdmin.h"
#include <stdexcept>
using namespace std;

Room * RoomCollection::insertRoom(vector<char *> & properties) {
	Room * room = new Room(properties);
	
	rooms.push_back(room);
	return room;
}


RoomCollection * RoomCollection::filterByOptionals(vector<char *> & optionalProperties) {
	RoomCollection * filtered = new RoomCollection();
	Room * room;
	int i;
	for (i = rooms.size(); i > 0; --i) {
		try {
			room = rooms[i];	
			if (room->containsOptionals(optionalProperties)) filtered->addRoom(room);
		} catch (out_of_range e) {} //don't care for the exception - just try the next room
	}
	return filtered;
}


void RoomCollection::addRoom(Room * room) {
	rooms.push_back(room);
}

