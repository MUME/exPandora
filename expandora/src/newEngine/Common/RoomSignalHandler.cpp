#include "RoomSignalHandler.h"

void RoomSignalHandler::hold(Room * room, QObject * owner) {
  owners[room] == owner;
  ++holdCount[room];
}

void RoomSignalHandler::release(Room * room) {
  if (holdCount.find(room) == holdCount.end()) return;
  if (--holdCount[room] == 0) {
    QObject * rcv = owners[room];
    if (rcv != 0) {
      releaseMutex.lock();
      QObject::connect(this, SIGNAL(releaseRoom(int)), rcv, SLOT(releaseRoom(int)));
      emit(releaseRoom(room->getId()));
      QObject::disconnect(this, SIGNAL(releaseRoom(int)), 0, 0);
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
    QObject::connect(this, SIGNAL(keepRoom(int)), rcv, SLOT(keepRoom(int)));
    emit(keepRoom(room->getId()));
    QObject::disconnect(this, SIGNAL(keepRoom(int)), 0, 0);
    releaseMutex.unlock();
  }

  owners.erase(room);
  holdCount.erase(room);
}
