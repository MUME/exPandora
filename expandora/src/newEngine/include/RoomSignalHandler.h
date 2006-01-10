#ifndef ROOMSIGNALHANDLER
#define ROOMSIGNALHANDLER

#include <map>
#include <qobject.h>
#include <qmutex.h>
#include "Room.h"

class RoomSignalHandler : public QObject {
 private:
  Q_OBJECT
  map<Room *, QObject *> owners;
  map<Room *, int> holdCount;
  QMutex releaseMutex;

 public:
  /* receiving from our clients: */
  // hold the room, we don't know yet what to do, overrides release, re-caches if room was un-cached
  void hold(Room * room, QObject * owner);
  // room isn't needed anymore and can be deleted if no one else is holding it and no one else uncached it
  void release(Room * room);
  // keep the room but un-cache it - overrides both hold and release
  void keep(Room * room);

 signals:
  void keepRoom(QObject *, int);
  void releaseRoom(QObject *, int);
  /* Sending to the rooms' owners:
     keepRoom: keep the room, but we don't need it anymore for now
     releaseRoom: delete the room, if you like */
};

#endif
