#include "RoomAdmin.h"
#include "IntermediateNode.h"

using namespace Qt;

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
#ifndef MONOLITHIC
extern "C" MY_EXPORT RoomAdmin * createComponent()
{
  return new RoomAdmin;
}
#else
Initializer<RoomAdmin> roomAdmin("Map");
#endif

RoomAdmin::RoomAdmin() : IntermediateNode(), mapLock(QMutex::Recursive)
{
  mapLock.lock();
  greatestUsedId = UINT_MAX;
  mapLock.unlock();
}

void RoomAdmin::removeRoom(int id)
{
  mapLock.lock();
  map.remove(roomIndex[id]->getCoordinate());
  roomIndex[id] = 0;
  unusedIds.push(id);
  emit deletedRoom(this, id);
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, Frustum * frustum)
{
  Room * r = 0;
  mapLock.lock();
  if (!connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)), DirectConnection))
    throw "can't connect to specified recipient";
  for(vector<Room *>::iterator i = roomIndex.begin(); i != roomIndex.end(); ++i)
  {
    r = *i;
    if(r && frustum->PointInFrustum(r->getCoordinate()))
    {
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
    
    if (!connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)), DirectConnection))
      throw "can't connect to specified recipient";
    locks[r->getId()].insert(recipient);
    emit foundRoom(this, r);
    disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
  }
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, unsigned int id)
{
  mapLock.lock();
  if (greatestUsedId >= id)
  {
    Room * r = roomIndex[id];
    if (!connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)), DirectConnection))
      throw "can't connect to specified recipient";
    locks[id].insert(recipient);
    emit foundRoom(this, r);
    disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)));
  }
  mapLock.unlock();
}

void RoomAdmin::assignId(Room * room)
{
  unsigned int id;

  mapLock.lock();
  if (unusedIds.empty()) id = ++greatestUsedId;
  else
  {
    id = unusedIds.top();
    unusedIds.pop();
    if (id > greatestUsedId) greatestUsedId = id;
  }


  room->setId(id);

  if (roomIndex.size() <= id)
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
  if (!room) {
    mapLock.unlock();
    return;
  }
  room->setTerrain(t);
  map.setNearest(expectedPosition, room);
  assignId(room);
  event->reset();
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, ParseEvent * event)
{
  mapLock.lock();
  if (greatestUsedId == UINT_MAX) {
    createRoom(event, new Coordinate(0,0,0), 0);
    if (greatestUsedId != UINT_MAX) {
      Room * room = roomIndex[greatestUsedId];
      locks[room->getId()].insert(0);
      room->addReverseExit(0,0);
    }
  }

  AbstractRoomContainer * ret;
  Room * r;
  
  ret = IntermediateNode::getRooms(event);
  if (ret->numRooms() >= 0)
  {

    if (!connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(receiveRoom(QObject *, Room *)), DirectConnection))
      throw "can't connect to specified recipient";
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

void RoomAdmin::addExit(int f, int t, int d)
{
  mapLock.lock();
  Room * from = roomIndex[f];
  Room * to = roomIndex[t];
  from->addExit(d, t);
  to->addReverseExit(d, f);
  mapLock.unlock();
}

// removes the lock on a room
// after the last lock is removed, the room is deleted
void RoomAdmin::releaseRoom(QObject * sender, int id)
{
  if(!sender) 
    throw "anonymous locks are permanent and can't be released";
  mapLock.lock();
  locks[id].erase(sender);
  if(locks[id].empty()) removeRoom(id);
  mapLock.unlock();
}

// makes a lock on a room permanent and anonymous.
// Like that the room can't be deleted via releaseRoom anymore.
void RoomAdmin::keepRoom(QObject * sender, int id)
{
  if(!sender) 
    throw "anonymous locks are permanent and can't be kept twice";
  mapLock.lock();
  locks[id].insert(0);
  locks[id].erase(sender);
  mapLock.unlock();
}
