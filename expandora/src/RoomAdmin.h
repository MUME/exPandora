#ifndef ROOMADMIN
#define ROOMADMIN
#include "RoomSearchNode.h"
#include "SearchTreeNode.h"
#include <vector>
#include <stack>
#include "Room.h"
#include "Map.h"
#include "Terrain.h"

using namespace std;

class RoomAdmin : public SearchTreeNode {
	public:
		RoomAdmin();
		~RoomAdmin() {} // implement it ...
		
		int lastId() {return greatestUsedId;}
		RoomSearchNode * getRooms(ParseEvent * event);
		Room * quickInsert(ParseEvent * knownEvent); 	// inserts a room based on an event we already getRoom()'d on
								// like this we don't traverse the upper part of the tree once again
		Room * quickInsert(ParseEvent * knownEvent, Coordinate * expectedPosition, Terrain * t = 0);
		Room * getRoom(int id) {if (greatestUsedId >= id) return roomIndex[id]; else return 0;};
		Room * getRoom(Coordinate * pos) {return map.get(pos);};
		
		Room * insertRoom(ParseEvent * event, Terrain * t = 0); // pos defaults to 0, returns the id of the new room
		Room * insertRoom(ParseEvent * event, int id, Coordinate * c, Terrain * t = 0); 	// inserts the room with predefined id, 
								// only use when building the tree from the database 
		Room * insertRoom(ParseEvent * event, Coordinate * expectedPosition, Terrain * t = 0);

	private:
		//Coordinate * findNearestFree(Coordinate * pos);
		Map map;
		void assignId(Room * room); 
		RoomSearchNode * deepestMatch;
		vector<Room *> roomIndex;
		stack<int>  unusedIds;
		int greatestUsedId;
		RoomCollection * rooms; // for the occasional room without properties ;)		
};

extern RoomAdmin roomAdmin; //globally visible roomAdmin ...
#endif
	
