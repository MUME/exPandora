#include "RoomAdmin.h"
#include "IntermediateNode.h"

RoomAdmin roomAdmin;

RoomAdmin::RoomAdmin() : SearchTreeNode("") {
	greatestUsedId = -1;
	rooms = rcmm.activate();
}

void RoomAdmin::removeRoom(int id) {
  map.remove(roomIndex[id]->getCoordinate());
  roomIndex[id] = 0;
  unusedIds.push(id);
}

Room * RoomAdmin::insertRoom(ParseEvent * event, Terrain * t) {
	Room * room = 0;
	
	if (event->current() == 0) {
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
		if (id > greatestUsedId) greatestUsedId = id;
	}
	
	room->setId(id);
	if ((int)roomIndex.size() <= id) roomIndex.resize(id + 1, 0);
	roomIndex[id] = room;
}

Room * RoomAdmin::insertRoom(ParseEvent * event, int id, Coordinate * c, Terrain * t) {
	unusedIds.push(id);

	Room * room = insertRoom(event, c, t);

	
	return room;
}


Room * RoomAdmin::quickInsert(ParseEvent * knownEvent, Coordinate * expectedPosition, Terrain * t) {
	Room * room = deepestMatch->insertRoom(knownEvent);
	deepestMatch = room->getHome();
	assignId(room);
	if (expectedPosition != 0) {
	  map.setNearest(expectedPosition, room);
	}
	room->setTerrain(t);	
	return room;
}

Room * RoomAdmin::insertRoom(ParseEvent * event, Coordinate * expectedPosition, Terrain * t) {


	Room * ret = insertRoom(event, t);
	map.setNearest(expectedPosition, ret);

	return ret;
}

RoomSearchNode * RoomAdmin::getRooms(ParseEvent * event) {
	if (event->current() == 0) return rooms;
	else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) deepestMatch = skipDown(event);
	else deepestMatch = SearchTreeNode::getRooms(event);
	return deepestMatch;
}


