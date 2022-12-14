#ifndef MAPFRONTEND
#define MAPFRONTEND

#include <qmutex.h>
#include <vector>
#include <stack>
#include <set>

#include "RoomSearchNode.h"
#include "IntermediateNode.h"
#include "Component.h"
#include "Frustum.h"
#include "Room.h"
#include "Map.h"
#include "RoomRecipient.h"
#include "RoomAdmin.h"



/**
 * The MapFrontend organizes rooms and their relations to each other.
 */
class MapFrontend : public Component, public IntermediateNode, public RoomAdmin
{
private:
  Q_OBJECT

  Map map;
  std::vector<Room *> roomIndex;
  std::vector<std::set<RoomRecipient *> > locks;
  std::vector<RoomCollection *> roomHomes;
  std::stack<int>  unusedIds;
  uint greatestUsedId;
  QMutex mapLock;


  void removeRoom(int id);
  void assignId(Room * room, RoomCollection * roomHome);

public:
  MapFrontend();
  virtual Qt::ConnectionType requiredConnectionType(const QString &) {return Qt::DirectConnection;}
  // removes the lock on a room
  // after the last lock is removed, the room is deleted
  void releaseRoom(RoomRecipient *, int);

  // makes a lock on a room permanent and anonymous.
  // Like that the room can't be deleted via releaseRoom anymore.
  void keepRoom(RoomRecipient *, int);



public slots:
  // looking for rooms leads to a bunch of foundRoom() signals
  void lookingForRooms(RoomRecipient *,ParseEvent *);
  void lookingForRooms(RoomRecipient *,unsigned int); // by id
  void lookingForRooms(RoomRecipient *,Coordinate);
  void lookingForRooms(RoomRecipient *,Frustum *);

  // createRoom creates a room without a lock
  // it will get deleted if no one looks for it for a certain time
  void createRoom(ParseEvent *, Coordinate, char);

  // same as above, but creates an anonymous lock on the room
  // and tries to assign a special id
  void createPredefinedRoom(ParseEvent *, Coordinate, char, int);

  // addExit doesn't lock a room either
  void addExit(int, int, uint);



  // other slots like moveRoom and forceDeleteRoom need to be
  // implemented later

signals:

  // this signal is sent out if a room is deleted. So any clients still
  // working on this room can start some emergency action.
  void deletedRoom(RoomAdmin *, int);
};





#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
