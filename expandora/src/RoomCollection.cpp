#include "RoomCollection.h"
#include "RoomAdmin.h"
#include <stdexcept>
using namespace std;

ObjectRecycler<RoomCollection> rcmm;

Room * RoomCollection::insertRoom(ParseEvent * event) {
	Room * room = new Room(event);
	
	rooms.push_back(room);
	return room;
}


RoomCollection * RoomCollection::filterByOptionals(ParseEvent * event) {
	RoomCollection * filtered = rcmm.activate();
	Room * room;
	for  (list<Room *>::iterator i = rooms.begin(); i != rooms.end(); i++)	
		if ((*i)->containsOptionals(event->getOptionals())) filtered->addRoom(*i);
	
	return filtered;
}


void RoomCollection::addRoom(Room * room) {
	rooms.push_back(room);
}

void RoomCollection::clear() {
	rooms.clear();
} 


void RoomCollection::merge(RoomCollection * other) {
	rooms.splice(rooms.begin(), other->getRooms());
}
