#ifndef EXPERIMENTING_H
#define EXPERIMENTING_H

#include "Room.h"
#include "Path.h"
#include "RoomAdmin.h"
#include "RoomRecipient.h"
#include "PathParameters.h"

class Parser;

class Experimenting : public RoomRecipient {
 private:
  
  Parser * parent;
  uint direction;
  std::list<Path *> * shortPaths;
  std::list<Path *> * paths;
  Path * best;
  Path * second;
  PathParameters params;
  
  int numPaths;
  

 public:
  Experimenting(Parser * parent, std::list<Path *> * paths, uint direction);
  std::list<Path *> * evaluate();
  void receiveRoom(RoomAdmin *, Room *);

};
#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif

