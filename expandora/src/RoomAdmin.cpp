#include "RoomAdmin.h"
#include "IntermediateNode.h"

RoomAdmin roomAdmin;

RoomAdmin::RoomAdmin() : SearchTreeNode(""), mapLock(true) {
  mapLock.lock();
  greatestUsedId = -1;
  mapLock.unlock();
}

void RoomAdmin::removeRoom(int id) {
  mapLock.lock();
  map.remove(roomIndex[id]->getCoordinate());
  roomIndex[id] = 0;
  unusedIds.push(id);
  mapLock.unlock();
}

Room * RoomAdmin::getRoom(Coordinate * pos) {
  mapLock.lock();
  Room * ret = map.get(pos);
  mapLock.unlock();
  return ret;
}

Room * RoomAdmin::getRoom(int id) {
  Room * ret = 0;
  mapLock.lock();
  if (greatestUsedId >= id) {
    ret = roomIndex[id];
  }
  mapLock.unlock();
  return ret;
}

void RoomAdmin::assignId(Room * room) {
  int id;

  mapLock.lock();
  if (unusedIds.empty()) id = ++greatestUsedId;
  else {
    id = unusedIds.top();
    unusedIds.pop();
    if (id > greatestUsedId) greatestUsedId = id;
  }


  room->setId(id);

  if ((int)roomIndex.size() <= id) roomIndex.resize(id + 1, 0);
  roomIndex[id] = room;
  mapLock.unlock();
}

Room * RoomAdmin::insertRoom(ParseEvent * event, int id, Coordinate * c, Terrain * t) {
  mapLock.lock();
  unusedIds.push(id);
  Room * room = insertRoom(event, c, t);
  mapLock.unlock();
  return room;
}


Room * RoomAdmin::quickInsert(ParseEvent * knownEvent, Coordinate * expectedPosition, Terrain * t) {
  if (knownEvent->current()->size() == SKIPPED_ONE || knownEvent->current()->size() == SKIPPED_MANY) return 0;

  mapLock.lock();
  if (deepestMatch == 0) return 0;
  Room * room = deepestMatch->insertRoom(knownEvent);
  if (room != 0) deepestMatch = room->getHome();
  else deepestMatch = 0;

  map.setNearest(expectedPosition, room);
  room->setTerrain(t);	
  assignId(room);
  mapLock.unlock();  
  return room;
}

Room * RoomAdmin::insertRoom(ParseEvent * event, Coordinate * expectedPosition, Terrain * t) {
  if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return 0;

  mapLock.lock();    
  Room * room = SearchTreeNode::insertRoom(event);
  room->setTerrain(t);
  map.setNearest(expectedPosition, room);
  assignId(room);
  mapLock.unlock();
  return room;
}

RoomSearchNode * RoomAdmin::getRooms(ParseEvent * event) {
  RoomCollection * ret;
  mapLock.lock();
  if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) deepestMatch = skipDown(event);
  else deepestMatch = SearchTreeNode::getRooms(event);
  if (deepestMatch->numRooms() < 0) ret = rcmm.activate();
  else ret = (RoomCollection *)deepestMatch;
  mapLock.unlock();
  return ret;
}


