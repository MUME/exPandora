#ifndef ROOMADMIN
#define ROOMADMIN
#include "RoomSearchNode.h"
#include "IntermediateNode.h"
#include <qmutex.h>
#include <vector>
#include <stack>
#include "Room.h"
#include "Map.h"
#include "Terrain.h"


using namespace std;



class RoomAdmin : public IntermediateNode {
 public:
  RoomAdmin();
		
  int lastId() {return greatestUsedId;}
  RoomSearchNode * getRooms(ParseEvent * event);

  // inserts a room based on an event we already getRoom()'d on
  // like this we don't traverse the upper part of the tree once again
  Room * quickInsert(ParseEvent * knownEvent, Coordinate * expectedPosition, Terrain * t = 0);
  Room * getRoom(int id);
  Room * getRoom(Coordinate * pos);
		

  Room * insertRoom(ParseEvent * event, int id, Coordinate * c, Terrain * t = 0); 	// inserts the room with predefined id, 
  // only use when building the tree from the database 
  Room * insertRoom(ParseEvent * event, Coordinate * expectedPosition, Terrain * t = 0);
  void removeRoom(int id); 
 private:
  Map map;
  void assignId(Room * room); 
  RoomSearchNode * deepestMatch;
  vector<Room *> roomIndex;
  stack<int>  unusedIds;
  int greatestUsedId;
  QMutex mapLock;
};

//extern RoomAdmin roomAdmin; //globally visible roomAdmin ...
#endif
	

#ifdef DMALLOC
#include <dmalloc.h>
#endif
