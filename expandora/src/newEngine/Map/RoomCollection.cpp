#include "RoomCollection.h"
#include "RoomFactory.h"
#include <stack>

using namespace std;

ObjectRecycler<RoomCollection> rcmm;

RoomCollection * RoomCollection::getRooms(ParseEvent *) {
  RoomCollection * ret = rcmm.activate();
  ret->merge(this);
  return ret;
}

RoomCollection * RoomCollection::skipDown(ParseEvent *) {
  RoomCollection * ret = rcmm.activate();
  ret->merge(this);
  return ret;
}

Room * RoomCollection::createRoom(ParseEvent * event) {
  Room * room = RoomFactory::createRoom(event);
	
  rooms.insert(room);
  return room;
}




void RoomCollection::addRoom(Room * room) {
  rooms.insert(room);
}


void RoomCollection::removeRoom(Room * room) {
  rooms.erase(room);
}

/**
 * don't erase the rooms as they are still contained in other RoomCollections
 */
void RoomCollection::clear() {
  rooms.clear();
} 


RoomCollection * RoomCollection::merge(AbstractRoomContainer * other) {
  if (other != 0 && other->numRooms() > 0) {
    rooms.insert(((RoomCollection *)other)->begin(), ((RoomCollection *)other)->end());
  }
  return this;
}


