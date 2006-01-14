#ifndef ROOMRECIPIENT
#define ROOMRECIPIENT

#include "Room.h"

class RoomRecipient {
  public:
    void receiveRoom(RoomAdmin const * admin, Room const * room) = 0;

};

#endif