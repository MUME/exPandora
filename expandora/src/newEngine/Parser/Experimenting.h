#ifndef EXPERIMENTING_H
#define EXPERIMENTING_H

#include <qobject.h>
#include <qmutex.h>
#include "Room.h"
#include "Path.h"
#include "RoomAdmin.h"
#include "RoomRecipient.h"
#include "PathParameters.h"

class Parser;

class Experimenting : public RoomRecipient {
 private:
  
  list<Path *> * paths;
  list<Path *> * shortPaths;
  Path * best;
  Path * second;
  PathParameters params;
  Parser * parent;
  int numPaths;

 public:
  Experimenting(Parser * parent, list<Path *> * paths);
  list<Path *> * evaluate();
  void receiveRoom(RoomAdmin *, Room *);

};
#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif

