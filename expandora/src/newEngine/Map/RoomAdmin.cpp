#include "RoomAdmin.h"
#include "IntermediateNode.h"

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
extern "C" MY_EXPORT RoomAdmin * createComponent()
{
  return new RoomAdmin;
}

RoomAdmin::RoomAdmin() : IntermediateNode(), mapLock(QMutex::Recursive)
{
  mapLock.lock();
  greatestUsedId = -1;
  mapLock.unlock();
}

void RoomAdmin::removeRoom(int id)
{
  mapLock.lock();
  map.remove(roomIndex[id]->getCoordinate());
  roomIndex[id] = 0;
  unusedIds.push(id);
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, Frustum * frustum) {
  Room * r = 0;
  mapLock.lock();
  connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
  for(vector<Room *>::iterator i = roomIndex.begin(); i != roomIndex.end(); ++i) {
    r = *i;
    if(r && frustum->PointInFrustum(r->getCoordinate())) {
      locks[r->getId()].insert(recipient);
      emit foundRoom(this, r);
    }
  }
  disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, Coordinate * pos)
{
  mapLock.lock();
  Room * r = map.get(pos);
  if (r)
  {
    locks[r->getId()].insert(recipient);
    connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
    emit foundRoom(this, r);
    disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
  }
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, int id)
{
  mapLock.lock();
  if (greatestUsedId >= id)
  {
    Room * r = roomIndex[id];
    connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
    locks[id].insert(recipient);

    emit foundRoom(this, r);
    disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
  }
  mapLock.unlock();
}

void RoomAdmin::assignId(Room * room)
{
  int id;

  mapLock.lock();
  if (unusedIds.empty()) id = ++greatestUsedId;
  else
  {
    id = unusedIds.top();
    unusedIds.pop();
    if (id > greatestUsedId) greatestUsedId = id;
  }

#ifdef DEBUG
  fprintf(stderr, "assigning id: %i\n", id);
#endif
  room->setId(id);

  if ((int)roomIndex.size() <= id)
  {
    roomIndex.resize(id + 1, 0);
    locks.resize(id + 1);
  }
  roomIndex[id] = room;
  mapLock.unlock();
}

void RoomAdmin::createPredefinedRoom(ParseEvent * event, Coordinate * c, char t, int id)
{
  mapLock.lock();
  unusedIds.push(id);
  createRoom(event, c, t);
  locks[id].insert(0);
  mapLock.unlock();
}



void RoomAdmin::createRoom(ParseEvent * event, Coordinate * expectedPosition, char t)
{
  mapLock.lock();
  Room * room = IntermediateNode::insertRoom(event);
  room->setTerrain(t);
  map.setNearest(expectedPosition, room);
  assignId(room);
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, ParseEvent * event)
{
  if (greatestUsedId == -1) createRoom(event, new Coordinate(0,0,0), 0);
  
  AbstractRoomContainer * ret;
  Room * r;
  mapLock.lock();
  ret = IntermediateNode::getRooms(event);
  if (ret->numRooms() >= 0)
  {

    connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
    for(set<Room *>::iterator i = ((RoomCollection*)ret)->begin(); i != ((RoomCollection*)ret)->end(); ++i)
    {
      r = *i;
      locks[r->getId()].insert(recipient);
      emit foundRoom(this, r);
    }
    disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
    delete ret;
  }
  mapLock.unlock();
  event->reset();

}

void RoomAdmin::addExit(int f, int t, int d) {
	Room * from = roomIndex[f];
	Room * to = roomIndex[t];
	from->addExit(d, t);
	to->addReverseExit(d, f);
}

// removes the lock on a room
// after the last lock is removed, the room is deleted
void RoomAdmin::releaseRoom(QObject * sender, int id) {
  locks[id].erase(sender);
  if(locks[id].empty()) removeRoom(id);
}

// makes a lock on a room permanent and anonymous.
// Like that the room can't be deleted via releaseRoom anymore.
void RoomAdmin::keepRoom(QObject * sender, int id) {
  locks[id].insert(0);
  locks[id].erase(sender);
}
