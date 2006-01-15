#include "Path.h"


ObjectRecycler<Path> pamm;
RoomSignalHandler *  Path::signaler = 0;

Path::Path() :
  active(false),
  parent(0),
  probability(0),
  room(0) 
{
if (!signaler) signaler = new RoomSignalHandler;
}

void Path::init(Room * in_room, RoomAdmin * owner, RoomRecipient * locker) {
  if (active) {
    throw "fatal: path already active";
  }
  else active = true;

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
Path * Path::fork(Room * in_room, Coordinate * expectedCoordinate, RoomAdmin * owner, double pathAcceptance, RoomRecipient * locker) {
  if (!active) {
    throw "fatal: path inactive";
  }

  
  Path * ret = pamm.activate();
  if (ret == parent) {
    throw "fatal: building path circle";
  }
  ret->init(in_room, owner, locker);
  ret->setParent(this);
  children.insert(ret);
  double dist = expectedCoordinate->distance(in_room->getCoordinate());
  if (dist < 1) dist = 1.0/pathAcceptance;
  if (in_room->isNew()) dist *= pathAcceptance); 
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
  signaler->keep(room);
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); ++i) {
    (*i)->setParent(0);
  }
  children.clear();
  
  if (parent != 0) {
    parent->removeChild(this);
    parent->approve();
    parent = 0;
  }
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
  probability = 1.0;
}
		

	
void Path::removeChild(Path * p) {
  if (!active) {
    throw "fatal: path inactive";
  }
  children.erase(p);
}

