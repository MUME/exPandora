#include "RoomSignalHandler.h"

using namespace Qt;

void RoomSignalHandler::hold(Room * room, QObject * owner) {
  owners[room] = owner;
  ++holdCount[room];
}

void RoomSignalHandler::release(Room * room) {
  if (holdCount.find(room) == holdCount.end()) 
    throw "room can't be released because it isn't held";
  if (--holdCount[room] == 0) {
    QObject * rcv = owners[room];
    if (rcv != 0) {
      releaseMutex.lock();
      QObject::connect(this, SIGNAL(releaseRoom(QObject *, int)), rcv, SLOT(releaseRoom(QObject *, int)), DirectConnection);
      emit releaseRoom(owners[room], room->getId());
      QObject::disconnect(this, SIGNAL(releaseRoom(QObject *, int)), 0, 0);
      releaseMutex.unlock();
    }
    owners.erase(room);
    holdCount.erase(room);
  }
}

void RoomSignalHandler::keep(Room * room) {
  QObject * rcv = owners[room];
  if (rcv != 0) {
    releaseMutex.lock();
    QObject::connect(this, SIGNAL(keepRoom(QObject *, int)), rcv, SLOT(keepRoom(QObject *, int)), DirectConnection);
    emit keepRoom(owners[room], room->getId());
    QObject::disconnect(this, SIGNAL(keepRoom(QObject *, int)), 0, 0);
    releaseMutex.unlock();
  }

  owners.erase(room);
  holdCount.erase(room);
}
