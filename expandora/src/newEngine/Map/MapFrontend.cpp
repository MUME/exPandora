#include "MapFrontend.h"

using namespace Qt;
using namespace std;

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
#ifndef MONOLITHIC
extern "C" MY_EXPORT MapFrontend * createComponent()
{
  return new MapFrontend;
}
#else
Initializer<MapFrontend> roomAdmin("Map");
#endif

MapFrontend::MapFrontend() : mapLock(QMutex::Recursive)
{
  mapLock.lock();
  greatestUsedId = UINT_MAX;
  mapLock.unlock();
}

void MapFrontend::removeRoom(int id)
{
  mapLock.lock();
  Room * room = roomIndex[id];
  roomIndex[id] = 0;
  roomHomes[id]->removeRoom(room);
  roomHomes[id] = 0;
  Coordinate rc = room->getCoordinate();
  map.remove(rc);
  delete room;
  unusedIds.push(id);
  emit deletedRoom(this, id);
  mapLock.unlock();
}

void MapFrontend::lookingForRooms(RoomRecipient * recipient, Frustum * frustum)
{
  Room * r = 0;
  mapLock.lock();

  for(vector<Room *>::iterator i = roomIndex.begin(); i != roomIndex.end(); ++i)
  {
    r = *i;
    
    if(r)
    {
      Coordinate rc = r->getCoordinate();
      if(frustum->PointInFrustum(rc))
      {
        locks[r->getId()].insert(recipient);
        recipient->receiveRoom(this, r);
      }
    }
  }

  mapLock.unlock();
}

void MapFrontend::lookingForRooms(RoomRecipient * recipient, Coordinate pos)
{
  mapLock.lock();
  Room * r = map.get(pos);
  if (r)
  {


    locks[r->getId()].insert(recipient);
    recipient->receiveRoom(this, r);

  }
  mapLock.unlock();
}

void MapFrontend::lookingForRooms(RoomRecipient * recipient, uint id)
{
  mapLock.lock();
  if (greatestUsedId >= id)
  {
    Room * r = roomIndex[id];

    locks[id].insert(recipient);
    recipient->receiveRoom(this, r);

  }
  mapLock.unlock();
}

void MapFrontend::assignId(Room * room, RoomCollection * roomHome)
{
  uint id;

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
    roomHomes.resize(id + 1);
  }
  roomIndex[id] = room;
  roomHomes[room->getId()] = roomHome;
  mapLock.unlock();
}

void MapFrontend::createPredefinedRoom(ParseEvent * event, Coordinate c, char t, int id)
{
  mapLock.lock();
  unusedIds.push(id);
  createRoom(event, c, t);
  locks[id].insert(0);
  mapLock.unlock();
}



void MapFrontend::createRoom(ParseEvent * event, Coordinate expectedPosition, char t)
{
  mapLock.lock();
  RoomCollection * roomHome = IntermediateNode::insertRoom(event);

  if (!roomHome)
  {
    mapLock.unlock();
    return;
  }
  Room * room = roomHome->createRoom(event);
  map.setNearest(expectedPosition, room);
  assignId(room, roomHome);

  event->reset();
  mapLock.unlock();
}

void MapFrontend::lookingForRooms(RoomRecipient * recipient, ParseEvent * event)
{
  mapLock.lock();
  if (greatestUsedId == UINT_MAX)
  {
    createRoom(event, Coordinate(0,0,0), 0);
    if (greatestUsedId != UINT_MAX)
    {
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


    for(set<Room *>::iterator i = ((RoomCollection*)ret)->begin(); i != ((RoomCollection*)ret)->end(); ++i)
    {
      r = *i;
      locks[r->getId()].insert(recipient);
      recipient->receiveRoom(this, r);
    }

    delete ret;
  }
  mapLock.unlock();
  event->reset();

}

void MapFrontend::addExit(int f, int t, uint d)
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
void MapFrontend::releaseRoom(RoomRecipient * sender, int id)
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
void MapFrontend::keepRoom(RoomRecipient * sender, int id)
{
  if(!sender)
    throw "anonymous locks are permanent and can't be kept twice";
  mapLock.lock();
  locks[id].insert(0);
  locks[id].erase(sender);
  mapLock.unlock();
}
