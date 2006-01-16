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
  
  Parser * parent;
  char direction;
  list<Path *> * shortPaths;
  list<Path *> * paths;
  Path * best;
  Path * second;
  PathParameters params;
  
  int numPaths;
  

 public:
  Experimenting(Parser * parent, list<Path *> * paths, char direction);
  list<Path *> * evaluate();
  void receiveRoom(RoomAdmin *, Room *);

};
#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif

