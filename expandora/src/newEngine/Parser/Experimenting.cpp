#include "Experimenting.h"
#include "Parser.h"

Experimenting::Experimenting(Parser * par, list<Path *> * pat, double pa) {
  parent = par;
  pathAcceptance = pa;
  shortPaths = pat;
  paths = new list<Path *>();
  best = 0;
  second = 0;
  prevBest = shortPaths.front()->getProb();
}


  

void foundRoom(QObject * map, Room * room) {
  releaseSchedule.insert(make_pair(room, 0));
  roomOwners.insert(make_pair(room, map));
  for (i = shortPaths.begin(); i != shortPaths.end(); ++i) {
    Coordinate * c = parent->getExpectedCoordinate((*i)->getRoom());
    Path * working = (*i)->fork(room, c);
    if (working->getProb() < prevBest/pathAcceptance) {
      (*i)->removeChild(working);
      pamm.deactivate(working);
    }
    else {
      if (best == 0) best = working;
      else if (working->getProb() > best->getProb()) {
	paths.push_back(best);
	second = best;
	best = working;
      }
      else {
	if (second == 0) second = working;
	paths.push_back(working);
      }
      (releaseSchedule.find(room)->second)++;
    }
    cmm.deactivate(c);
  }
}
  

list<Path *> * Experimenting::evaluate() {
  Path * working = 0;
  for (i = shortPaths.begin(); i != shortPaths.end(); ++i) {
    working = paths.front();
    paths.pop_front();
    if (!(working->hasChildren())) working->deny();	
  }
 
  if (best != 0) {
    if (second == 0 || best->getProb() > second->getProb()*pathAcceptance) { // excactly one path left -> go APPROVED
      list<Path *>::iterator i = paths.begin();
      for (; i != paths.end(); i++) {
	(*i)->deny();
      }
      paths.clear();
    }
    paths.push_front(best);
  }

  for (map<Room *, int>::iterator i = releaseSchedule.begin(); i != releaseSchedule.end(); ++i) {
    if (i->second == 0) {
      QObject * recipient = roomOwners.find(i->first);
      QObject::connect(this, SIGNAL(release(int)), recipient, SLOT(release(int))); 
      emit release(i->first->getId());
      QObject::disconnect(this, SIGNAL(release(int)), recipient, SLOT(release(int))); 
    }
    releaseSchedule.clear();
    roomOwners.clear();
  }

  delete(shortPaths);
  shortPaths = 0;
  best = 0;
  second = 0;
  return paths;
}

