#include "Path.h"


ObjectRecycler<Path> pamm;
PathSignalHandler Path::signaler;

void Path::init(Room * in_room, QObject * owner) {
  if (active) {
    throw "fatal: path already active";
  }
  else active = true;

  signaler.hold(room, owner);
  room = in_room;

  children.clear();
  parent = 0;
  probability = 1;
}


/** 
 * new Path is fetched from pamm, 
 * distance between rooms is calculated 
 * and probability is updated accordingly
 */
Path * Path::fork(Room * in_room, Coordinate * expectedCoordinate, QObject * owner, double pathAcceptance) {
  if (!active) {
    throw "fatal: path inactive";
  }

  
  Path * ret = pamm.activate();
  if (ret == parent) {
    throw "fatal: building path circle";
  }
  ret->init(in_room, owner);
  ret->setParent(this);
  children.insert(ret);
  double dist = expectedCoordinate->distance(in_room->getCoordinate());
  if (dist < 1) dist = 1.0/pathAcceptance;
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
  signaler.forget(room);
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
  if (room != 0) signaler.release(room);
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
  if (room != 0) signaler.release(room);
  room = 0;
  children.clear();
  parent = 0;
  probability = 1;
  active = false;
}
		

	
void Path::removeChild(Path * p) {
  if (!active) {
    throw "fatal: path inactive";
  }
  children.erase(p);
}


void PathSignalHandler::hold(Room * room, QObject * owner) {
  owners[room] == owner;
  ++holdCount[room];
}

void PathSignalHandler::release(Room * room) {
  if (holdCount.find(room) == holdCount.end()) return;
  if (--holdCount[room] == 0) {
    QObject * rcv = owners[room];
    if (rcv != 0) {
      releaseMutex.lock();
      QObject::connect(this, SIGNAL(releaseRoom(int)), rcv, SLOT(releaseRoom(int)));
      emit(releaseRoom(room->getId()));
      QObject::disconnect(this, SIGNAL(releaseRoom(int)), 0, 0);
      releaseMutex.unlock();
    }
    owners.erase(room);
    holdCount.erase(room);
  }
}

void PathSignalHandler::forget(Room * room) {
  owners.erase(room);
  holdCount.erase(room);
}
