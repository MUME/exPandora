#ifndef ROOMRECIPIENT
#define ROOMRECIPIENT

#include "Room.h"
#include "RoomAdmin.h"

class RoomRecipient {
  public:
    virtual void receiveRoom(RoomAdmin * admin, Room * room) = 0;
    virtual ~RoomRecipient() {};
};

#endif

