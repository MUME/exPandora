#include "RoomAdmin.h"
#include "IntermediateNode.h"

RoomAdmin::RoomAdmin() : SearchTreeNode("") {
	greatestUsedId = -1;
	rooms = 0;
}

Room * RoomAdmin::insertRoom(ParseEvent * event) {
	Room * room;
	
	if (event->current() == 0) {
		rooms = rcmm.activate();
		return rooms->insertRoom(event);
	}
	else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return 0;
	else room = SearchTreeNode::insertRoom(event);
	
	int id;
	if (unusedIds.empty()) id = ++greatestUsedId;
	else {
		id = unusedIds.top();
		unusedIds.pop();
	}
	
	room->setId(id);
	roomIndex[id] = room;
	return room;	
}

Room * RoomAdmin::insertRoom(ParseEvent * event, int id) {
	unusedIds.push(id);
	return insertRoom(event);
}

RoomSearchNode * RoomAdmin::getRooms(ParseEvent * event) {
	if (event->current() == 0) return rooms;
	else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return skipDown(event);
	else return SearchTreeNode::getRooms(event);
}


