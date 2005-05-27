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

void RoomAdmin::lookingForRooms(QObject * recipient, Coordinate * pos) {
  mapLock.lock();
  connect(this, SIGNAL(foundRoom(foundRoom(QObject *, Room *))), recipient, SLOT(foundRoom(QObject *, Room *)));
  emit foundRoom(this,  map.get(pos));
  disconnect(this, SIGNAL(foundRoom(foundRoom(QObject *, Room *))), recipient, SLOT(foundRoom(QObject *, Room *)));
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, int id) {
  mapLock.lock();
  if (greatestUsedId >= id) {
    connect(this, SIGNAL(foundRoom(foundRoom(QObject *, Room *))), recipient, SLOT(foundRoom(QObject *, Room *)));
    emit foundRoom(this, roomIndex[id]);
    disconnect(this, SIGNAL(foundRoom(foundRoom(QObject *, Room *))), recipient, SLOT(foundRoom(QObject *, Room *)));
  }
  mapLock.unlock();
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

void RoomAdmin::createPredefinedRoom(ParseEvent * event, int id, Coordinate * c, int t) {
  mapLock.lock();
  unusedIds.push(id);
  Room * room = createRoom(event, c, t);
  mapLock.unlock();
}



void RoomAdmin::createRoom(ParseEvent * event, Coordinate * expectedPosition, int t) {
  mapLock.lock();    
  Room * room = IntermediateNode::insertRoom(event);
  room->setTerrain(t);
  map.setNearest(expectedPosition, room);
  assignId(room);
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, ParseEvent * event) {
  AbstractRoomContainer * ret;
  mapLock.lock();
  ret = IntermediateNode::getRooms(event);
  if (ret->numRooms() < 0) ret = rcmm.activate();
  connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(foundRoom(QObject *, Room *)));
  for(set<Room *>::iterator i = ret.begin(); i != ret.end(); ++i) {
    emit foundRoom(this, *i);
  }
  disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(foundRoom(QObject *, Room *)));    
  mapLock.unlock();

  event->reset(); // we can't tell where that node came from

}


