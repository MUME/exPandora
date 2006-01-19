#include "Display.h"
#include "CachedRoom.h"




/**
  The new CachedRoom will at least exist until drawExits(RednererWidget *) is called the 
  next time on any CachedRoom. After that it may disappear any time and should only be 
  referenced through the cache where it will delete itself in that case.
*/
CachedRoom::CachedRoom(Room * in_base, RoomAdmin * in_owner, RoomRecipient * in_locker, map<int, CachedRoom *> & in_cache) : base(in_base), owner(in_owner), locker(in_locker), cache(in_cache) {
  set<int> rawExits;
  for (unsigned int i = 0; i < base->numExits(); ++i) {
    rawExits = base->getNeighbours(i);
    
    for (set<int>::iterator iter = rawExits.begin(); iter != rawExits.end(); ++iter) {
      exits[*iter].insert(i);
    
    }
  }

  for (unsigned int i = 0; i < base->numReverseExits(); ++i) {
    rawExits = base->getReverseNeighbours(i);
    
    for (set<int>::iterator iter = rawExits.begin(); iter != rawExits.end(); ++iter) {
      reverseExits[*iter].insert(i);
    
    }
  }
  int id = base->getId();
  
  cache[id] = this;  
}

CachedRoom::~CachedRoom() {
  int id = base->getId();
  cache[id] = 0;
  owner->releaseRoom(locker, id);
}

int CachedRoom::getId() {
  return base->getId();
}

Coordinate CachedRoom::getCoordinate() {
  return base->getCoordinate();
}

void CachedRoom::removeReverse(int id) {
  reverseExits.erase(id);
  if (exits.empty() && reverseExits.empty()) {
    delete this;
  }
}

void CachedRoom::drawExits(CachedRoom * other, RendererWidget * renderer) {

  Coordinate mc = base->getCoordinate();
  int oId = other->getId();

  set<int> & dirs = exits[oId];

  for (set<int>::iterator i = dirs.begin(); i != dirs.end(); ++i) {
    Coordinate oc = other->getCoordinate();
    renderer->drawExit(mc, oc, *i);
  }
  exits.erase(oId);

}


void CachedRoom::drawExits(RendererWidget * renderer) {
  if (exits.empty() && reverseExits.empty()) {
   delete this;
   return;
  }
  int mId = base->getId();
  for (map<int, set<int> >::iterator i = exits.begin(); i != exits.end(); ++i) {
    int oid = i->first;
    if (cache.find(oid) != cache.end()) {
      CachedRoom * other = cache[oid];
      if(other) {
	other->drawExits(this, renderer);
	drawExits(other, renderer);
	other->removeReverse(mId);
	removeReverse(oid);
      }
    }
  }
  
}
