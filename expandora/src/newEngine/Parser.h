#ifndef PARSER
#define PARSER
#include <queue>
#include <list>
#include "Terrain.h"
#include "ParseEvent.h"
#include "Property.h"
#include "Path.h"
#include "Room.h"
#include "Coordinate.h"


#define APPROVED 0
#define EXPERIMENTING 1
#define SYNCING 2

class Display;

class Parser {
 public:
  Parser();
  void attachRoomAdmin(RoomAdmin * _admin) {admin = _admin;}         
  void attachDisplay(Display * _display) {display = _display;}
  void setTerrain(Property * ter);
  void event(ParseEvent * ev);
  Room * getMostLikely() {return mostLikelyRoom;}
 private:
  void mudPop();
  void playerPop();
  void unify();
		
  void dropNote(ParseEvent * ev);
  void checkQueues();
  void experimenting();
  void syncing();
  void approved();
  void enlargePaths(RoomCollection * enlargingRooms);
  void buildPaths(RoomCollection * initialRooms);

  char state;
  queue<ParseEvent *> playerEvents;
  queue<ParseEvent *> mudEvents;
  Terrain * activeTerrain;
  list<Path *> paths;
  Room * mostLikelyRoom;
  RoomAdmin * admin;
  Display * display;
  Coordinate * getExpectedCoordinate(Room * base);
};	

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
