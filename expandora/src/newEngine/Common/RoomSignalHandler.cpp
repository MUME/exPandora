#include "RoomSignalHandler.h"

using namespace Qt;

void RoomSignalHandler::hold(Room * room, QObject * owner, QObject * locker) {
  owners[room] = owner;
  if (locker) lockers[room].insert(locker);
  ++holdCount[room];
}

void RoomSignalHandler::release(Room * room) {
  if (holdCount.find(room) == holdCount.end()) 
    throw "room can't be released because it isn't held";
  if (--holdCount[room] == 0) {
    QObject * rcv = owners[room];
    if(rcv) {
      QObject::connect(this, SIGNAL(releaseRoom(QObject *, int)), rcv, SLOT(releaseRoom(QObject *, int)), DirectConnection);
      for(set<QObject *>::iterator i = lockers[room].begin(); i != lockers[room].end(); ++i) {
	emit releaseRoom(*i, room->getId());
      }
      QObject::disconnect(this, SIGNAL(releaseRoom(QObject *, int)));
      lockers.erase(room);
      owners.erase(room);
      holdCount.erase(room);
    }
  }
}

void RoomSignalHandler::keep(Room * room) {
  if (holdCount.find(room) == holdCount.end()) 
    throw "room can't be kept because it isn't held";
  --holdCount[room];
  QObject * rcv = owners[room];
  if (rcv) {
    QObject::connect(this, SIGNAL(keepRoom(QObject *, int)), rcv, SLOT(keepRoom(QObject *, int)), DirectConnection);
    emit keepRoom(*(lockers[room].begin()), room->getId());
    QObject::disconnect(this, SIGNAL(keepRoom(QObject *, int)));
  }
  if (holdCount[room] == 0) {
    lockers.erase(room);
    owners.erase(room);
    holdCount.erase(room);
  }
  else 
    lockers[room].erase(lockers[room].begin());
}
