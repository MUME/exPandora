#ifndef ROOMADMIN
#define ROOMADMIN
#include "RoomSearchNode.h"
#include "SearchTreeNode.h"
#include <vector.h>
#include <stack.h>
#include "Room.h"

class RoomAdmin : public SearchTreeNode {
	public:
		RoomAdmin();
		~RoomAdmin();
		RoomSearchNode * getRooms(ParseEvent * event);
		Room * getRoom(int id) {return roomIndex[id];};
		Room * insertRoom(ParseEvent * event); // pos defaults to 0, returns the id of the new room
		Room * insertRoom(ParseEvent * event, int id); 	// inserts the room with predefined id, 
											// only use when building the tree from the database 
	private:
		vector<Room *> roomIndex;
		stack<int>  unusedIds;
		int greatestUsedId;
		RoomCollection * rooms; // for the occasional room without properties ;)		
};

extern RoomAdmin roomAdmin; //globally visible roomAdmin ...
#endif
	
