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
		RoomCollection * getRooms(vector<char *> & properties);
		Room * getRoom(int id) {return roomIndex[id];};
		Room * insertRoom(vector<char *> & properties); // pos defaults to 0, returns the id of the new room
		Room * insertRoom(vector<char *> & properties, int id); 	// inserts the room with predefined id, 
											// only use when building the tree from the database 
	private:
		vector<Room *> roomIndex;
		stack<int>  unusedIds;
		int greatestUsedId;
		RoomCollection * rooms; // for the occasional room without properties ;)		
};

extern RoomAdmin roomAdmin; //globally visible roomAdmin ...
#endif
	
