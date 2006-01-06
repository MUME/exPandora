
#include "Approved.h"

void Approved::receiveRoom(QObject * sender, Room * perhaps) {  
  if (matchedRoom == 0 && perhaps->fastCompare(myEvent, matchingTolerance)) {
    matchedRoom = perhaps;
    owner = sender;
  }
  else {
    QObject::connect(this, SIGNAL(releaseRoom(int)), sender, SLOT(releaseRoom(int)), Qt::DirectConnection);
    emit releaseRoom(perhaps->getId());
    QObject::disconnect(this, SIGNAL(releaseRoom(int)), sender, 0);
    if (matchedRoom != 0) moreThanOne = true; 
  }
}

Approved::Approved(ParseEvent * event, int tolerance) :
  matchedRoom(0),
  myEvent(event),
  matchingTolerance(tolerance),
  owner(0),
  moreThanOne(false)
  
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
    QObject::connect(this, SIGNAL(releaseRoom(int)), owner, SLOT(releaseRoom(int)), Qt::DirectConnection);
    emit releaseRoom(matchedRoom->getId());
    QObject::disconnect(this, SIGNAL(releaseRoom(int)), owner, 0);
  }
  matchedRoom = 0;
  moreThanOne = false;
  owner = 0;
}

QObject * Approved::getOwner() {
  if (moreThanOne) return 0;
  else return owner;
}
