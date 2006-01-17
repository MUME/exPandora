#include "RoomSignalHandler.h"

using namespace Qt;

void RoomSignalHandler::hold(Room * room, RoomAdmin * owner, RoomRecipient * locker)
{
  owners[room] = owner;
  if (lockers[room].empty()) holdCount[room] = 0;
  lockers[room].insert(locker);
  ++holdCount[room];
}

void RoomSignalHandler::release(Room * room)
{
  if (holdCount.find(room) == holdCount.end())
    throw "room can't be released because it isn't held";
  if (--holdCount[room] == 0)
  {
    RoomAdmin * rcv = owners[room];
    if(rcv)
    {
      for(set<RoomRecipient *>::iterator i = lockers[room].begin(); i != lockers[room].end(); ++i)
      {
        if (*i) rcv->releaseRoom(*i, room->getId());
      }
      lockers[room].clear();
      owners[room] = 0;
    }
  }
}

void RoomSignalHandler::keep(Room * room, char dir, int fromId)
{
  if (holdCount.find(room) == holdCount.end())
    throw "room can't be kept because it isn't held";
  --holdCount[room];
  RoomAdmin * rcv = owners[room];
  if (rcv)
  {
    if (fromId >= 0)
    {
      set<int> * neighbours = room->getNeighbours(dir);
      if (!neighbours || neighbours->find(dir) == neighbours->end())
        emit addExit(fromId, room->getId(), dir);
    }
    RoomRecipient * locker = *(lockers[room].begin());
    if (locker) // the locker is 0 if the room was found by approved - so no keep is needed
      rcv->keepRoom(locker, room->getId());

    
  }
  if (holdCount[room] == 0)
  {
    lockers[room].clear();
    owners[room] = 0;
  }
  else
    lockers[room].erase(lockers[room].begin());
}
