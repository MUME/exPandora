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
	assignId(room);
	return room;
}

void RoomAdmin::assignId(Room * room) {
	int id;
	if (unusedIds.empty()) id = ++greatestUsedId;
	else {
		id = unusedIds.top();
		unusedIds.pop();
	}
	
	room->setId(id);
	roomIndex[id] = room;
}

Room * RoomAdmin::insertRoom(ParseEvent * event, int id) {
	unusedIds.push(id);
	Room * room = insertRoom(event);
	assignId(room);
	return room;
}


Room * RoomAdmin::quickInsert(ParseEvent * knownEvent, Coordinate * expectedPosition) {
	Room * room = deepestMatch->insertRoom(knownEvent);
	assignId(room);
	if (expectedPosition != 0) {
		Coordinate * c = map.setNearest(expectedPosition, room);
		room->setCoordinate(c);
	}	
	return room;
}

RoomSearchNode * RoomAdmin::getRooms(ParseEvent * event) {
	if (event->current() == 0) return rooms;
	else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) deepestMatch = skipDown(event);
	else deepestMatch = SearchTreeNode::getRooms(event);
	return deepestMatch;
}


