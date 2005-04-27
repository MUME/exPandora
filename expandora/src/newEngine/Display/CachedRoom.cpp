#include "CachedRoom.h"
#include "Display.h"

RoomSignalHandler CachedRoom::signaler;
map<int, CachedRoom *> CachedRoom::cache;

CachedRoom::CachedRoom(Room * in_base, QObject * owner) : base(in_base) {
  signaler.hold(base, owner);
  set<int> * rawExits;
  for (int i = base->numExits()-1; i >= 0; --i) {
    rawExits = base->getNeighbours(i);
    for (set<int>::iterator iter = rawExits->begin(); iter != rawExits->end(); ++iter) {
      exits[*iter].insert(i);
    }
  }

  for (int i = base->numReverseExits()-1; i >= 0; --i) {
    rawExits = base->getReverseNeighbours(i);
    for (set<int>::iterator iter = rawExits->begin(); iter != rawExits->end(); ++iter) {
      reverseExits[*iter].insert(i);
    }
  }
  int id = base->getId();
  if (cache.find(id) != cache.end()) delete cache[id];
  cache[id] = this;
}

CachedRoom::~CachedRoom() {
  signaler.release(base);
}

int CachedRoom::getId() {
  return base->getId();
}

Coordinate * CachedRoom::getCoordinate() {
  return base->getCoordinate();
}

void CachedRoom::removeReverse(int id) {
  reverseExits.erase(id);
  if (exits.empty() && reverseExits.empty()) {
    cache.erase(base->getId());
    delete this;
  }
}

void CachedRoom::drawExits(CachedRoom * other, RendererWidget * renderer) {
  Coordinate * oc = other->getCoordinate();
  Coordinate * mc = base->getCoordinate();
  int mId = base->getId();
  int oId = other->getId();


  set<int> & dirs = exits[oId];
  other->removeReverse(mId);
  for (set<int>::iterator i = dirs.begin(); i != dirs.end(); ++i) {
    renderer->drawExit(mc, oc, *i);
  }
  exits.erase(oId);
  
  /*
  if (reverseExits.find(oId) != reverseExits.end()) 
    other->drawExits(this, renderer);
  */

  if (exits.empty() && reverseExits.empty()) {
    cache.erase(base->getId());
    delete this;
  }
}


void CachedRoom::drawExits(RendererWidget * renderer) {
  for (map<int, set<int> >::iterator i = exits.begin(); i != exits.end(); ++i) {
    int oid = i->first;
    if (cache.find(oid) != cache.end()) {
      CachedRoom * other = cache[oid];
      other->drawExits(this, renderer);
      drawExits(other, renderer);
    }
  }
}
