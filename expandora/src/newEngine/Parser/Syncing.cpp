#include "Syncing.h"


Syncing::Syncing(list<Path *> * pa) :
  paths(pa)
{
}

void Syncing::foundRoom(QObject * sender, Room * room) {
  Path * path = pamm.activate();
  path->init(room);
  paths->push_front(path);
}
  
list<Path *> * Syncing::evaluate() {
  return paths;
}
