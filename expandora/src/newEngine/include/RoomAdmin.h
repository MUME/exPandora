#ifndef ROOMADMIN
#define ROOMADMIN

class RoomRecipient;
class RoomAdmin {
  public:
    // removes the lock on a room
    // after the last lock is removed, the room is deleted
    virtual void releaseRoom(RoomRecipient *, int) = 0;

    // makes a lock on a room permanent and anonymous.
    // Like that the room can't be deleted via releaseRoom anymore.
    virtual void keepRoom(RoomRecipient *, int) = 0;
    
    virtual ~RoomAdmin() {};
};

#endif
