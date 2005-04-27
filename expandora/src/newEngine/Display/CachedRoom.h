#ifndef CACHEDROOM
#define CACHEDROOM
#include "Coordinate.h"
#include "Room.h"
#include "RoomSignalHandler.h"

class RendererWidget;

/**
   we need to remember which exits we have already draw and
   decide if we still need the room. That's why we cache
   it's exits and remove them from the cached room as we draw them.*/
class CachedRoom;
class CachedRoom {
 private:
  static RoomSignalHandler signaler;
  static map<int, CachedRoom *> cache;
  map<int, set<int> > exits; //other's id->dirs
  map<int, set<int> > reverseExits;
  Room * base;
  void removeReverse(int id);
  
 public:
  CachedRoom(Room * base, QObject * owner);
  ~CachedRoom();
  void drawExits(CachedRoom * other, RendererWidget * renderer);
  void drawExits(RendererWidget * renderer);
  int getId();
  Coordinate * getCoordinate();

};

#endif
