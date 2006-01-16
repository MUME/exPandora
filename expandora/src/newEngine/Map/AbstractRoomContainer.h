#ifndef ABSTRACTROOMCONTAINER
#define ABSTRACTROOMCONTAINER

#include "Room.h"
#include "ParseEvent.h"

class RoomCollection;
class AbstractRoomContainer {

 public:
  virtual int numRooms() {return -1;}
  virtual AbstractRoomContainer * merge(AbstractRoomContainer * other) {return other;}
  virtual RoomCollection * insertRoom(ParseEvent *) {return 0;}
  virtual ~AbstractRoomContainer() {}
};

#ifdef DMALLOC
#include <mpatrol.h>
#endif

#endif
