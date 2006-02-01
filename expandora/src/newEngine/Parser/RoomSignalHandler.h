#ifndef ROOMSIGNALHANDLER
#define ROOMSIGNALHANDLER

#include <map>
#include <set>
#include <qobject.h>
#include "Room.h"
#include "RoomRecipient.h"
#include "RoomAdmin.h"

class RoomSignalHandler : public QObject {
  Q_OBJECT
 private:

  std::map<Room *, RoomAdmin *> owners;
  std::map<Room *, std::set<RoomRecipient *> > lockers;
  std::map<Room *, int> holdCount;
  

 public:
  RoomSignalHandler(QObject * parent) : QObject(parent) {}
  /* receiving from our clients: */
  // hold the room, we don't know yet what to do, overrides release, re-caches if room was un-cached
  void hold(Room * room, RoomAdmin * owner, RoomRecipient * locker);
  // room isn't needed anymore and can be deleted if no one else is holding it and no one else uncached it
  void release(Room * room);
  // keep the room but un-cache it - overrides both hold and release
  void keep(Room * room, uint dir, int toId);

  /* Sending to the rooms' owners:
     keepRoom: keep the room, but we don't need it anymore for now
     releaseRoom: delete the room, if you like */
     
  signals:
   void addExit(int, int, uint);
};
#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
