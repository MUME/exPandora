#include "RoomCollection.h"
#include "RoomAdmin.h"
#include <stdexcept>
#include <stack>
using namespace std;

ObjectRecycler<RoomCollection> rcmm;

Room * RoomCollection::insertRoom(ParseEvent * event) {
  Room * room = rmm.activate();
  room->init(event);
	
  rooms.insert(room);
  return room;
}


RoomCollection * RoomCollection::filterByOptionals(ParseEvent * event) {
  RoomCollection * filtered = rcmm.activate();
  for  (set<Room *>::iterator i = rooms.begin(); i != rooms.end(); i++)	
    if ((*i)->containsOptionals(event->getOptionals())) filtered->addRoom(*i);
	
  return filtered;
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

Room * RoomCollection::matchOne(ParseEvent * target) {
  set<Room *>::iterator i = rooms.begin();
  stack<Room *> deleteSchedule;
  for (; i != rooms.end(); i++) {
    if (!(*i)->fastCompare(target, defaultTolerance)) deleteSchedule.push(*i);
  }
  while (!deleteSchedule.empty()) {
    rooms.erase(deleteSchedule.top());
    deleteSchedule.pop();
  }
  if (rooms.begin() == rooms.end()) return 0;
  else if ((++(rooms.begin())) == rooms.end()) return *(rooms.begin());
  else return 0;
}

RoomCollection * RoomCollection::merge(RoomSearchNode * other) {
  if (other->numRooms() > 0) {
    set<Room *> otherSet = ((RoomCollection *)other)->getRooms();
    rooms.insert(otherSet.begin(), otherSet.end());
  }
  return this;
}
