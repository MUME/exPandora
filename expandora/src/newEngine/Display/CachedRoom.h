#ifndef CACHEDROOM
#define CACHEDROOM
#include "RoomSignalHandler.h"
#include "Coordinate.h"
#include "Room.h"


class RendererWidget;

/**
   we need to remember which exits we have already drawn and
   decide if we still need the room. That's why we cache
   it's exits and remove them from the cached room as we draw them.*/
class CachedRoom;
class CachedRoom {
 private:

  map<int, set<int> > exits; //other's id->dirs
  map<int, set<int> > reverseExits;
  Room * base;
  RoomAdmin * owner;
  RoomRecipient * locker;
  map<int, CachedRoom *> & cache;
  void removeReverse(int id);

 public:
  CachedRoom(Room * base, RoomAdmin * owner, RoomRecipient * locker, map<int, CachedRoom *> & cache);
  ~CachedRoom();
  void drawExits(CachedRoom * other, RendererWidget * renderer);
  void drawExits(RendererWidget * renderer);
  int getId();
  Coordinate * getCoordinate();

};
#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
