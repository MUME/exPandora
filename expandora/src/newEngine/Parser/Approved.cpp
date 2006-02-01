
#include "Approved.h"

void Approved::receiveRoom(RoomAdmin * sender, Room * perhaps) {  
  if (matchedRoom == 0 && perhaps->matches(myEvent, matchingTolerance)) {
    matchedRoom = perhaps;
    owner = sender;
  }
  else {
    
    sender->releaseRoom(this, perhaps->getId());
    
  }
}

Approved::~Approved() {
  if(owner) {
    
    if (moreThanOne) {
      
      owner->releaseRoom(this, matchedRoom->getId());
      
    }
    else {
      
      owner->keepRoom(this, matchedRoom->getId());
      
    }
    
  }
  
}


Approved::Approved(Parser * in_parser, ParseEvent * event, int tolerance) :
  matchedRoom(0),
  myEvent(event),
  matchingTolerance(tolerance),
  owner(0),
  moreThanOne(false),
  parser(in_parser)
  
{
}


Room * Approved::oneMatch() {
  if (moreThanOne) 
    return 0;
  else
    return matchedRoom;
}

void Approved::reset() {
  if(matchedRoom) {
    
    owner->releaseRoom(this, matchedRoom->getId());
    
  }
  matchedRoom = 0;
  moreThanOne = false;
  owner = 0;
}



RoomAdmin * Approved::getOwner() {
  if (moreThanOne) return 0;
  else return owner;
}
