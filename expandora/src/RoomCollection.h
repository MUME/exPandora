#ifndef ROOMCOLLECTION
#define ROOMCOLLECTION
#include <vector.h>
#include "RoomSearchNode.h"
#include "Room.h"

class RoomCollection : public RoomSearchNode {
	public:
		RoomCollection() {};
		RoomCollection * getRooms(char ** properties, int property, int numProperties) {return this;}
		Room * insertRoom(char ** properties, int numProperties);
		Room * insertRoom(char ** properties, int property, int numProperties) {return insertRoom(properties, numProperties);};
		RoomCollection * filterByOptionals(char ** optionalProperties, int num);
		void RoomCollection::addRoom(Room * room);
	private:
		vector<Room> * rooms;
		
};
#endif

