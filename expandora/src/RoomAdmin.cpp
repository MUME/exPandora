#include "RoomAdmin.h"
#include "IntermediateNode.h"

RoomAdmin::RoomAdmin() : SearchTreeNode("", this, 0) {
	greatestUsedId = -1;
	rooms = 0;
}

Room * RoomAdmin::insertRoom(vector<char *> & properties) {
	Room * newRoom; 
	if (properties.size() == 0) {
		if(rooms == 0) rooms = new RoomCollection();
		newRoom = rooms->insertRoom(properties, 0);
	}	
	else {	
		char next = properties[0][0];
		selectedChild = get(next);
		if (selectedChild == 0) {
			if (next == 0) { // first property is empty
				selectedChild = new IntermediateNode(properties[1], this);		
				put(0, selectedChild);
			}
		
			else {
				selectedChild = new SearchTreeNode(properties[0], this, 0);
				put(next, selectedChild);
			}
		}
		newRoom = selectedChild->insertRoom(properties, 0);
	}
	int id;
	if (unusedIds.empty()) id = ++greatestUsedId;
	else {
		id = unusedIds.top();
		unusedIds.pop();
	}
	
	newRoom->setId(id);
	roomIndex[id] = newRoom;
	return newRoom;	
}

Room * RoomAdmin::insertRoom(vector<char *> & properties, int id) {
	unusedIds.push(id);
	return insertRoom(properties);
}

RoomCollection * RoomAdmin::getRooms(vector<char *> & properties) {
	if (properties.size() == 0) return rooms;
	char next = properties[0][0];
	selectedChild = get(next);
	if (selectedChild == 0) return 0;
	else return selectedChild->getRooms(properties, 0);
}


