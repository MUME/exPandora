#ifndef APPROVED_H
#define APPROVED_H

#include <qobject.h>
#include "Parser.h"
#include "Room.h"
#include "ParseEvent.h"
#include "RoomAdmin.h"
#include "RoomRecipient.h"


class Approved : public RoomRecipient {
 private:

   Room * matchedRoom;
   ParseEvent * myEvent;
   int matchingTolerance;   
   RoomAdmin * owner;
   bool moreThanOne;
   Parser * parser;

 public:
   Approved(Parser * parser, ParseEvent * event, int tolerance);
   ~Approved();
   void receiveRoom(RoomAdmin *, Room *);
   Room * oneMatch();
   RoomAdmin * getOwner(); 
   void reset();
};
#ifdef DMALLOC
#include <mpatrol.h>
#endif

#endif
