#include "Path.h"
#include "LexDefs.h"

ObjectRecycler<Path> pamm;

void Path::init(Room * _room, RoomAdmin * _admin) {
  admin = _admin;
  room = _room;
  room->hold();
  children.clear();
  parent = 0;
  probability = 1;
}


/** 
 * new Path is fetched from pamm, 
 * distance between rooms is calculated 
 * and probability is updated accordingly
 */
Path * Path::fork(Room * _room, Coordinate * expectedCoordinate) {
  Path * ret = pamm.activate();
  ret->init(_room, admin);
  ret->setParent(this);
  children.insert(ret);
  double dist = expectedCoordinate->distance(_room->getCoordinate());
  if (dist < 1) dist = 1.0/PATH_ACCEPT;
  ret->setProb(probability / dist);
  return ret;
}

void Path::setParent(Path * p) {
  parent = p;
}

void Path::approve() {
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); i++) {
    (*i)->cutDeadBranch();
  }
  children.clear();
  
  if (parent != 0) {
    parent->removeChild(this);
    parent->approve();
  }
  pamm.deactivate(this);
}



/** removes this path and all parents up to the next branch
 * and gives them back to the pamm 
 * and removes the respective rooms if experimental
 */
void Path::deny() {
  if (!children.empty()) return;
  if (room != 0) room->release(admin);
  room = 0;
  if (parent != 0) {
    parent->removeChild(this);
    parent->deny();
  }
  pamm.deactivate(this);
}
	


void Path::clear() {
  if (room != 0) room->release(admin);
  room = 0;
  children.clear();
  parent = 0;
  probability = 1;
}
		
/** 
 * removes this path and recursively all children 
 * and gives them back to the pamm
 * and removes all respective experimental rooms
 */	
void Path::cutDeadBranch() {
  if (room != 0) room->release(admin);
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); i++) {
    (*i)->cutDeadBranch();
  }
  children.clear();
  pamm.deactivate(this);
}
	
void Path::removeChild(Path * p) {
  children.erase(p);
}
