#include "RoomCollection.h"
#include "RoomAdmin.h"
#include <stdexcept>
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

Room * RoomCollection::insertRoom(ParseEvent * event) {
  //Room * room = new Room();
  Room * room = rmm.activate();
  room->init(event, this);
	
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
  if (other != 0 && other->numRooms() > 0) {
    rooms.insert(((RoomCollection *)other)->begin(), ((RoomCollection *)other)->end());
  }
  return this;
}

void RoomCollection::checkConsistency() {
  stack<Room *> deleteQueue;
  for(set<Room *>::iterator i = rooms.begin(); i != rooms.end(); i++) 
    if ((*i)->getCoordinate() == 0) deleteQueue.push(*i);
  while (!deleteQueue.empty()) {
    rooms.erase(deleteQueue.top());
    deleteQueue.pop();
  }
}
