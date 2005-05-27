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

RoomAdmin::RoomAdmin() : IntermediateNode(), mapLock(true)
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

void RoomAdmin::lookingForRooms(QObject * recipient, Coordinate * pos)
{
  mapLock.lock();
  Room * r = map.get(pos);
  if (r)
  {
    locks[r->getId()].insert(recipient);
    connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(foundRoom(QObject *, Room *)));
    emit foundRoom(this, r);
    disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(foundRoom(QObject *, Room *)));
  }
  mapLock.unlock();
}

void RoomAdmin::lookingForRooms(QObject * recipient, int id)
{
  mapLock.lock();
  if (greatestUsedId >= id)
  {
    Room * r = roomIndex[id];
    connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(foundRoom(QObject *, Room *)));
    locks[id].insert(recipient);

    emit foundRoom(this, r);
    disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(foundRoom(QObject *, Room *)));
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
  AbstractRoomContainer * ret;
  Room * r;
  mapLock.lock();
  ret = IntermediateNode::getRooms(event);
  if (ret->numRooms() >= 0)
  {

    connect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(foundRoom(QObject *, Room *)));
    for(set<Room *>::iterator i = ((RoomCollection*)ret)->begin(); i != ((RoomCollection*)ret)->end(); ++i)
    {
      r = *i;
      locks[r->getId()].insert(recipient);
      emit foundRoom(this, r);
    }
    disconnect(this, SIGNAL(foundRoom(QObject *, Room *)), recipient, SLOT(foundRoom(QObject *, Room *)));
    delete ret;
  }
  mapLock.unlock();

  // in the future the cycling will be done inside the map ONLY,
  // so this won't be needed anymore and rooms can be searched in several maps in parallel
  event->reset();

}

void RoomAdmin::addExit(int f, int t, int d) {
	Room * from = roomIndex[f];
	Room * to = roomIndex[t];
	from->addExit(d, t);
	to->addReverseExit(d, f);
}
