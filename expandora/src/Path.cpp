#include "Path.h"

ObjectRecycler<Path> pamm;

void Path::init(Room * _room, RoomAdmin * _admin) {
  admin = _admin;
  room = _room;
  children.clear();
  parent = 0;
  probability = 1;
}


/** 
 * new Path is fetched from pamm, 
 * distance between rooms is calculated 
 * and probability is updated accordingly
 */
Path * Path::fork(Room * _room) {
  Path * ret = pamm.activate();
  ret->init(_room, admin);
  ret->setParent(this);
  children.insert(ret);
  int dist = room->getCoordinate()->distance(_room->getCoordinate());
  if (dist == 0) dist = 1;
  ret->setProb(probability / (double)dist);
  return ret;
}

void Path::setParent(Path * p) {
  parent = p;
}

void Path::approve() {
  if (room != 0) room->hold();
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); i++) {
    (*i)->cutDeadBranch();
  }
  children.clear();
  parent->approve();
  pamm.deactivate(this);
}



/** removes this path and all parents up to the next branch
 * and gives them back to the pamm 
 * and removes the respective rooms if experimental
 */
void Path::deny() {
  room->release(admin); // wrong: the room could still be part of other paths; room needs some kind of "path count"
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); i++) {
    (*i)->cutDeadBranch();
  }
  children.clear();
  parent->removeChild(this);
  pamm.deactivate(this);
}
	


void Path::clear() {
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
  room->release(admin);
  room = 0;
  set<Path *>::iterator i = children.begin();
  for(; i != children.end(); i++) {
    (*i)->cutDeadBranch();
  }
  pamm.deactivate(this);
}
	
void Path::removeChild(Path * p) {
  children.erase(p);
}
