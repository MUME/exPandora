#include "RoomAdmin.h"
#include "IntermediateNode.h"

RoomAdmin roomAdmin;

RoomAdmin::RoomAdmin() : SearchTreeNode("") {
	greatestUsedId = -1;
	rooms = 0;
}

Room * RoomAdmin::insertRoom(ParseEvent * event, Terrain * t) {
	Room * room;
	
	if (event->current() == 0) {
		rooms = rcmm.activate();
		return rooms->insertRoom(event);
	}
	else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return 0;
	else room = SearchTreeNode::insertRoom(event);
	assignId(room);
	room->setTerrain(t);
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
	if (roomIndex.size() <= id) roomIndex.resize(id + 1, 0);
	roomIndex[id] = room;
}

Room * RoomAdmin::insertRoom(ParseEvent * event, int id, Coordinate * c, Terrain * t) {
	unusedIds.push(id);
	Room * room = insertRoom(event, t);
	map.setNearest(c, room);
	room->setCoordinate(c);
	return room;
}


Room * RoomAdmin::quickInsert(ParseEvent * knownEvent, Coordinate * expectedPosition, Terrain * t) {
	Room * room = deepestMatch->insertRoom(knownEvent);
	assignId(room);
	if (expectedPosition != 0) {
		Coordinate * c = map.setNearest(expectedPosition, room);
		room->setCoordinate(c);
	}
	room->setTerrain(t);	
	return room;
}

Room * RoomAdmin::insertRoom(ParseEvent * event, Coordinate * expectedPosition, Terrain * t) {
	RoomSearchNode * temp = deepestMatch;
	deepestMatch = this;
	Room * ret = quickInsert(event, expectedPosition, t);
	deepestMatch = temp;
	return ret;
}

RoomSearchNode * RoomAdmin::getRooms(ParseEvent * event) {
	if (event->current() == 0) return rooms;
	else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) deepestMatch = skipDown(event);
	else deepestMatch = SearchTreeNode::getRooms(event);
	return deepestMatch;
}


