#include "Path.h"


ObjectRecycler<Path> pamm;


Path::Path() :
  active(false),
  parent(0),
  probability(0),
  room(0),
  signaler(0),
  dir(0)
{}

void Path::init(Room * in_room, RoomAdmin * owner, RoomRecipient * locker, RoomSignalHandler * in_signaler, char direction) {
  if (active) {
    throw "fatal: path already active";
  }
  else active = true;
  dir = direction;
  
  
  signaler = in_signaler;
  room = in_room;
  signaler->hold(room, owner, locker);
  
  children.clear();
  parent = 0;
  
  probability = 1.0;
}


/** 
 * new Path is fetched from pamm, 
 * distance between rooms is calculated 
 * and probability is updated accordingly
 */
Path * Path::fork(Room * in_room, Coordinate & expectedCoordinate, RoomAdmin * owner, PathParameters p, RoomRecipient * locker, uint direction) {
  if (!active) {
    throw "fatal: path inactive";
  }

  
  Path * ret = pamm.activate();
  if (ret == parent) {
    throw "fatal: building path circle";
  }
  ret->init(in_room, owner, locker, signaler, direction);
  ret->setParent(this);
  children.insert(ret);
  Coordinate roomCoord = in_room->getCoordinate();
  double dist = expectedCoordinate.distance(roomCoord);
  if (dist < 0.5) dist = 1.0/p.correctPositionBonus;
  if (in_room->isNew()) dist *= p.newRoomPenalty; 
  ret->setProb(probability / dist);
  
  return ret;
}

void Path::setParent(Path * p) {
  if (!active) {
    throw "fatal: path inactive";
  }
  parent = p;
}

void Path::approve() {
  if (!active) {
    throw "fatal: path inactive";
  }
  int id = -1;
  if (parent) {
    id = parent->getRoom()->getId();
    parent->removeChild(this);
    parent->approve();
    parent = 0;
  }
  signaler->keep(room, dir, id);
  
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); ++i) {
    (*i)->setParent(0);
  }
  children.clear();
  
  
  pamm.deactivate(this);
}



/** removes this path and all parents up to the next branch
 * and gives them back to the pamm 
 * and removes the respective rooms if experimental
 */
void Path::deny() {
  if (!active) {
    throw "fatal: path inactive";
  }
  if (!children.empty()) return;
  signaler->release(room);
  room = 0;
  if (parent != 0) {
    parent->removeChild(this);
    parent->deny();
    parent = 0;
  }
  pamm.deactivate(this);
}


void Path::clear() {
  if (!active) {
    throw "fatal: path inactive";
  }
  active = false;
  if (room != 0) signaler->release(room);
  room = 0;
  children.clear();
  parent = 0;
  signaler = 0;
  dir = 0;
  probability = 1.0;
}
		

	
void Path::removeChild(Path * p) {
  if (!active) {
    throw "fatal: path inactive";
  }
  children.erase(p);
}

