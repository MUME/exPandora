#include "RoomSignalHandler.h"

using namespace Qt;

void RoomSignalHandler::hold(Room * room, RoomAdmin * owner, RoomRecipient * locker) {
  owners[room] = owner;
  if (lockers[room].empty()) holdCount[room] = 0;
  if (locker) lockers[room].insert(locker);
  ++holdCount[room];
}

void RoomSignalHandler::release(Room * room) {
  if (holdCount.find(room) == holdCount.end()) 
    throw "room can't be released because it isn't held";
  if (--holdCount[room] == 0) {
    RoomAdmin * rcv = owners[room];
    if(rcv) {
      for(set<RoomRecipient *>::iterator i = lockers[room].begin(); i != lockers[room].end(); ++i) {
	rcv->releaseRoom(*i, room->getId());
      }
      lockers[room].clear();
      owners[room] = 0;
    }
  }
}

void RoomSignalHandler::keep(Room * room) {
  if (holdCount.find(room) == holdCount.end()) 
    throw "room can't be kept because it isn't held";
  --holdCount[room];
  RoomAdmin * rcv = owners[room];
  if (rcv) {
    
    rcv->keepRoom(*(lockers[room].begin()), room->getId());
    
  }
  if (holdCount[room] == 0) {
    lockers[room].clear();
    owners[room] = 0;
  }
  else 
    lockers[room].erase(lockers[room].begin());
}
