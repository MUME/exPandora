#include "Approved.h"

void Approved::foundRoom(QObject * sender, Room * perhaps) {  
  if (matchedRoom != 0 && perhaps->fastCompare(myEvent, matchingTolerance)) {
      matchedRoom = perhaps;
      owner = sender;
  }
  else {
    QObject::connect(this, SIGNAL(releaseRoom(int)), sender, SLOT(releaseRoom(int)));
    emit releaseRoom(perhaps->getId());
    QObject::disconnect(this, SIGNAL(releaseRoom(int)), 0, 0);
    if (matchedRoom != 0) moreThanOnde = true; 
  }
}

Approved(ParseEvent * event, int tolerance) :
  myEvent(event),
  matchingTolerance(tolerance),
  owner(0),
  moreThanOne(false);
{
}


Room * Approved::oneMatch() {
  if (moreThanOne) 
    return 0;
  else
    return matchedRoom;
}

void Approved::reset() {
  matchedRoom = 0;
  moreThanOne = false;
  owner = 0;
}

QObject * Approved::getOwner() {
  if (moreThanOne) return 0;
  else return owner;
}
