#ifndef ROOMCOLLECTION
#define ROOMCOLLECTION
#include <vector>
#include "RoomSearchNode.h"
#include "Room.h"
using namespace std;

class RoomCollection {
	public:
		RoomCollection() {};
		RoomCollection * getRooms(vector<char *> properties, int property) {return this;}
		Room * insertRoom(vector<char *> properties);
		Room * insertRoom(vector<char *> properties, int property) {return insertRoom(properties);};
		RoomCollection * filterByOptionals(vector<char *> optionalProperties);
		void RoomCollection::addRoom(Room * room);
	private:
		vector<Room *> rooms;
		
};
#endif

