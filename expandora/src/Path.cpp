#include "Path.h"
#include "LexDefs.h"

ObjectRecycler<Path> pamm;

void Path::init(Room * _room, RoomAdmin * _admin) {
  if (active) {
    printf("fatal: path already active");
  }
  else active = true;
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
  if (!active) {
    printf("fatal: path inactive");
  }
  Path * ret = pamm.activate();
  if (ret == parent) {
    printf("fatal: building path circle");
  }
  ret->init(_room, admin);
  ret->setParent(this);
  children.insert(ret);
  double dist = expectedCoordinate->distance(_room->getCoordinate());
  if (dist < 1) dist = 1.0/PATH_ACCEPT;
  ret->setProb(probability / dist);
  return ret;
}

void Path::setParent(Path * p) {
  if (!active) {
    printf("fatal: path inactive");
  }
  parent = p;
}

void Path::approve() {
  if (!active) {
    printf("fatal: path inactive");
  }
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); i++) {
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
    printf("fatal: path inactive");
  }
  if (!children.empty()) return;
  if (room != 0) room->release(admin);
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
    printf("fatal: path inactive");
  }
  if (room != 0) room->release(admin);
  room = 0;
  children.clear();
  parent = 0;
  probability = 1;
  active = false;
}
		
/** 
 * removes this path and recursively all children 
 * and gives them back to the pamm
 * and removes all respective experimental rooms
	
void Path::cutDeadBranch() {
  if (!active) {
    printf("fatal: path inactive");
  }
  if (room != 0) room->release(admin);
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); i++) {
    (*i)->cutDeadBranch();
  }
  parent = 0;
  children.clear();
  pamm.deactivate(this);
}
*/
	
void Path::removeChild(Path * p) {
  if (!active) {
    printf("fatal: path inactive");
  }
  children.erase(p);
}
