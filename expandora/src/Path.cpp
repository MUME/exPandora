#include "Path.h"

ObjectRecycler<Path> pamm;

void Path::init(Room * _room) {
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
	ret->init(_room);
	ret->setParent(this);
	children.insert(ret);
	double dist = room->getCoordinate()->distance(_room->getCoordinate());
	ret->setProb(probability / dist);
	return ret;
}

void Path::setParent(Path * p) {
	parent = p;
}

void Path::approve() {
	if (room != 0) room->approve();
	room = 0;
	parent->approve();
	set<Path *>::iterator i = children.begin();
	for(; i != children.end(); i++) {
		(*i)->cutDeadBranch();
	}
	children.clear();
	pamm.deactivate(this);
}



/** removes this path and all parents up to the next branch
 * and gives them back to the pamm 
 * and removes the respective rooms if experimental
 */
void Path::deny() {
	if (room != 0) room->clear();
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
	probability = 0;
}
		
/** 
 * removes this path and recursively all chldren 
 * and gives them back to the pamm
 * and removes all respective experimental rooms
 */	
void Path::cutDeadBranch() {
	room->clear();
	set<Path *>::iterator i = children.begin();
	for(; i != children.end(); i++) {
		(*i)->cutDeadBranch();
	}
	pamm.deactivate(this);
}
	
void Path::removeChild(Path * p) {
	children.erase(p);
}
