#include "Experimenting.h"
#include "Parser.h"

Experimenting::Experimenting(Parser * par, list<Path *> * pat, double pa) {
  parent = par;
  pathAcceptance = pa;
  shortPaths = pat;
  paths = new list<Path *>;
  best = 0;
  second = 0;
  prevBest = shortPaths->front()->getProb();
}


  

void Experimenting::receiveRoom(QObject * map, Room * room) {
  for (list<Path *>::iterator i = shortPaths->begin(); i != shortPaths->end(); ++i) {
    Coordinate * c = parent->getExpectedCoordinate((*i)->getRoom());
    Path * working = (*i)->fork(room, c, map, pathAcceptance, this);
    if (working->getProb() < prevBest/pathAcceptance) {
      (*i)->removeChild(working);
      pamm.deactivate(working);
    }
    else {
      if (best == 0) best = working;
      else if (working->getProb() > best->getProb()) {
	paths->push_back(best);
	second = best;
	best = working;
      }
      else {
	if (second == 0) second = working;
	paths->push_back(working);
      }
    }
    cmm.deactivate(c);
  }
}
  

list<Path *> * Experimenting::evaluate() {
  Path * working = 0;
  while(!shortPaths->empty()) {
    working = shortPaths->front();
    shortPaths->pop_front();
    if (!(working->hasChildren())) working->deny();	
  }
 
  if (best != 0) {
    if (second == 0 || best->getProb() > second->getProb()*pathAcceptance) {
      for (list<Path *>::iterator i = paths->begin(); i != paths->end(); ++i) {
	(*i)->deny();
      }
      paths->clear();
    }
    paths->push_front(best);
  }

  delete(shortPaths);
  shortPaths = 0;
  best = 0;
  second = 0;
  return paths;
}

