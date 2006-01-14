#ifndef SYNCING_H
#define SYNCING_H

#include <list>
#include "Path.h"
#include "RoomAdmin.h"
#include "RoomRecipient.h"

class Syncing : public RoomRecipient {
 private:
   list<Path *> * paths;

 public:
   Syncing(list<Path *> * in);
   void receiveRoom(RoomAdmin *, Room *);
   list<Path *> * evaluate();
};
#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
   
