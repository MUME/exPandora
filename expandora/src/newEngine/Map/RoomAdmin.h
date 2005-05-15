#ifndef ROOMADMIN
#define ROOMADMIN
#include "RoomSearchNode.h"
#include "IntermediateNode.h"
#include "Component.h"
#include <qthread.h>
#include <qmutex.h>
#include <vector>
#include <stack>
#include "Room.h"
#include "Map.h"



using namespace std;


/**
 * The RoomAdmin organizes rooms and their relations to each other.
 */
class RoomAdmin : public IntermediateNode, public Component {
 public:
  RoomAdmin();

  void start(QThread::Priority) {}	
  int lastId() {return greatestUsedId;}
  AbstractRoomContainer * getRooms(ParseEvent * event);

  Room * getRoom(int id);
  Room * getRoom(Coordinate * pos);
		

  Room * insertRoom(ParseEvent * event, int id, Coordinate * c, int t = 0); 	// inserts the room with predefined id, 
  // only use when building the tree from the database 
  Room * insertRoom(ParseEvent * event, Coordinate * expectedPosition, int t = 0);
  void removeRoom(int id); 
 private:
  Q_OBJECT
  Map map;
  void assignId(Room * room); 
  AbstractRoomContainer * deepestMatch;
  vector<Room *> roomIndex;
  stack<int>  unusedIds;
  int greatestUsedId;
  QMutex mapLock;
};


#endif
	

#ifdef DMALLOC
#include <dmalloc.h>
#endif
