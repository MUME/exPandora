#ifndef EXPERIMENTING_H
#define EXPERIMENTING_H

#include <qobject.h>
#include <qmutex.h>
#include "Room.h"
#include "Path.h"
#include "RoomAdmin.h"
#include "RoomRecipient.h"

class Parser;

class Experimenting : public RoomRecipient {
 private:
  
  list<Path *> * paths;
  list<Path *> * shortPaths;
  double prevBest;
  Path * best;
  Path * second;
  double pathAcceptance;
  Parser * parent;
  QMutex deleteLock;

 public:
  Experimenting(Parser * parent, list<Path *> * paths, double pa);
  list<Path *> * evaluate();
  void receiveRoom(RoomAdmin *, Room *);
 
  

 
};
#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif

