#include "Room.h"

ObjectRecycler<Room> rmm;


Room::Room() {
  id = 0;
  terrain = 0;
  unique = false;
}

set<int> Room::getNeighbours(uint k) {
QMutexLocker locker(&roomMutex);
if (exits.size() > k)
  return exits[k];
else {
  set<int> ret;
  return ret;
}
}

set<int> Room::getReverseNeighbours(uint k) {
QMutexLocker locker(&roomMutex);
if (reverseExits.size() > k)
  return reverseExits[k];
else {
  set<int> ret;
  return ret;
}
}

void Room::init(ParseEvent * event) {

  TinyList<Property *> * x_properties = event->getProperties();



  for (char i = 0; x_properties->get(i); ++i)
    properties.put(i, x_properties->get(i)->copyString());

  TinyList<Property *> * x_optionalProperties = event->getOptionals();
  for (char i = 0; x_optionalProperties->get(i) != 0; i++)
    optionalProperties.put(i, x_optionalProperties->get(i)->copyString());
}


void Room::addExit(uint direc, int target) {
  QMutexLocker locker(&roomMutex);
  if (exits.size() <= direc) exits.resize(direc+1); 
  set<int> & roomsInDir = exits[direc];
  roomsInDir.insert(target);
}

void Room::addReverseExit(uint direc, int from) {
  QMutexLocker locker(&roomMutex);
  if (reverseExits.size() <= direc) reverseExits.resize(direc+1); 
  set<int> & roomsInDir = reverseExits[direc];
  roomsInDir.insert(from);
}


set<int> * Room::go(BaseEvent * dir) {
  QMutexLocker locker(&roomMutex);
  set<int> * ret  = new set<int>();;
  Coordinate & move = Coordinate::stdMoves[dir->subType];
  if ((move.x == 0) && (move.y == 0) && (move.z == 0)) {
    ret->insert(id);
  }
  else if (dir->subType == UNKNOWN) {
    for (unsigned int i = 0; i < exits.size(); ++i) {
      ret->insert(exits[i].begin(), exits[i].end());
    }
  }
  else if (exits.size() > dir->subType) {
    set<int> & dirExits = exits[dir->subType];
    ret->insert(dirExits.begin(), dirExits.end());
  }
  return ret;
}

/*
 * return all properties to pmm and remove this room from the room collection
 */
void Room::clear() {

  for (char i = 0; properties.get(i) != 0; i++) {
    ssmm.deactivate(properties.get(i));
    properties.remove(i);
  }
  for (char i = 0; optionalProperties.get(i) != 0; ++i) {
    ssmm.deactivate(optionalProperties.get(i));
    optionalProperties.remove(i);
  }
  for (unsigned int i = 0; i < exits.size(); ++i) {
    exits[i].clear();
  }
  for (unsigned int i = 0; i < reverseExits.size(); ++i) {
    reverseExits[i].clear();
  }
  c.clear();
  terrain = 0;
  unique = false;
  id = 0;
}
	
Room::~Room() {
  clear();
}

/** compare the optional properties that are not present in the search tree
 * perhaps we should allow a tolerance, too?
 */
bool Room::containsOptionals(TinyList<Property *> * optionals) {
  char i;
  char j;
	
  bool matched = false;
  QMutexLocker locker(&roomMutex);
  for (i = 0; optionals->get(i) != 0; i++) {
    for (j = 0; optionalProperties.get(j) != 0; j++) {
      if (optionals->get(i)->equals(optionalProperties.get(j))) {
	matched = true;
	break;
      }
    }
    if (!matched) return false;
    else matched = false;
  }
  return true;
}

/**
 * we allow a tolerance for typoes and such like pandora does
 */
bool Room::fastCompare(ParseEvent * ev, int tolerance) {
  char j = 0;
  TinyList<Property *> * props = ev->getProperties(); 
  char i = 0;
  QMutexLocker locker(&roomMutex);		
  for (; props->get(i) != 0 && properties.get(j) != 0; i++, j++) {
    tolerance -= props->get(i)->compare(properties.get(j));
    if (tolerance < 0) return false;
  }
  return true;
}
				
bool Room::isNew() {
  QMutexLocker locker(&roomMutex);
  for (unsigned int i = 0; i < reverseExits.size(); ++i) {
    if (!reverseExits[i].empty()) return false;
  }
  return true;
}
