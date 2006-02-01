#include "Syncing.h"

using namespace std;

Syncing::Syncing(list<Path *> * pa, RoomSignalHandler * in_signaler) :
  paths(pa),
  signaler(in_signaler)
{
}

void Syncing::receiveRoom(RoomAdmin * sender, Room * room) {
  Path * path = pamm.activate();
  path->init(room, sender, this, signaler);
  paths->push_front(path);
}
  
list<Path *> * Syncing::evaluate() {
  return paths;
}
