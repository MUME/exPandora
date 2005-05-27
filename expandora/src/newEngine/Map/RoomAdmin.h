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

 private:
  Q_OBJECT

  Map map;
  AbstractRoomContainer * deepestMatch;
  vector<Room *> roomIndex;
  stack<int>  unusedIds;
  int greatestUsedId;
  QMutex mapLock;


  void removeRoom(int id); 
  void assignId(Room * room); 
  int lastId() {return greatestUsedId;}

 slots:
  // looking for rooms leads to a bunch of foundRoom() signals
  void lookingForRooms(QObject *,ParseEvent *);
  void lookingForRooms(QObject *,int); // by id
  void lookingForRooms(QObject *,Coordinate *);
  void lookingForRooms(QObject *,Frustum *);

  // createRoom creates a room without a lock
  // it will get deleted if no one looks for it for a certain time
  void createRoom(ParseEvent *, Coordinate *, char);

  // same as above, but creates an anonymous lock on the room 
  // and tries to assign a special id
  void createPredefinedRoom(ParseEvent *, Coordinate *, char, int);

  // addExit doesn't lock a room either
  void addExit(int, int, char);

  // removes the lock on a room
  // after the last lock is removed, the room is deleted
  void releaseRoom(QObject *, int);

  // makes a lock on a room permanent and anonymous.
  // Like that the room can't be deleted via releaseRoom anymore.
  void keepRoom(QObject *, int);

  // other slots like moveRoom and forceDeleteRoom need to be 
  // implemented later

 signals:
  // if a room is sent out via foundRoom it is locked so that it doesn't
  // get deleted while being processed by another component.
  void foundRoom(QObject *, Room *);

  // this signal is sent out if a room is deleted. So any clients still
  // working on this room can start some emergency action.
  void deletedRoom(QObject *, Room *);
};


#endif
	

#ifdef DMALLOC
#include <dmalloc.h>
#endif
