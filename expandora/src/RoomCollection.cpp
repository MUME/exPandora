#include "RoomCollection.h"
#include "RoomAdmin.h"
#include <stdexcept>
using namespace std;

Room * RoomCollection::insertRoom(char ** properties, int numProperties) {
	Room * room = new Room(properties, numProperties);
	
	rooms->push_back(*room);
	return room;
}


RoomCollection * RoomCollection::filterByOptionals(char ** optionalProperties, int num) {
	RoomCollection * filtered = new RoomCollection();
	Room * room;
	int i;
	for (i = rooms->size(); i > 0; --i) {
		try {
			room = &rooms->at(i);	
			if (room->containsOptionals(optionalProperties, num)) filtered->addRoom(room);
		} catch (out_of_range e) {} //don't care for the exception - just try the next room
	}
	return filtered;
}


void RoomCollection::addRoom(Room * room) {
	rooms->push_back(*room);
}

