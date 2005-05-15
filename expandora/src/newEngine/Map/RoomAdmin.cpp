#include "RoomAdmin.h"
#include "IntermediateNode.h"

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
extern "C" MY_EXPORT RoomAdmin * createComponent() {
  return new RoomAdmin;
}

RoomAdmin::RoomAdmin() : IntermediateNode(), mapLock(true) {
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

#ifdef DEBUG
  fprintf(stderr, "assigning id: %i\n", id);
#endif
  room->setId(id);

  if ((int)roomIndex.size() <= id) roomIndex.resize(id + 1, 0);
  roomIndex[id] = room;
  mapLock.unlock();
}

Room * RoomAdmin::insertRoom(ParseEvent * event, int id, Coordinate * c, int t) {
  mapLock.lock();
  unusedIds.push(id);
  Room * room = insertRoom(event, c, t);
  mapLock.unlock();
  return room;
}



Room * RoomAdmin::insertRoom(ParseEvent * event, Coordinate * expectedPosition, int t) {

  mapLock.lock();    
  Room * room = IntermediateNode::insertRoom(event);
  room->setTerrain(t);
  map.setNearest(expectedPosition, room);
  assignId(room);
  mapLock.unlock();
  return room;
}

AbstractRoomContainer * RoomAdmin::getRooms(ParseEvent * event) {
  AbstractRoomContainer * ret;

  mapLock.lock();
  ret = IntermediateNode::getRooms(event);
  if (ret->numRooms() < 0) ret = rcmm.activate();
  deepestMatch = this; 
  mapLock.unlock();

  event->reset(); // we can't tell where that node came from
  return ret;
}


