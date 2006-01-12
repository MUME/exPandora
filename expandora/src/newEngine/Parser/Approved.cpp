
#include "Approved.h"

void Approved::receiveRoom(QObject * sender, Room * perhaps) {  
  if (matchedRoom == 0 && perhaps->fastCompare(myEvent, matchingTolerance)) {
    matchedRoom = perhaps;
    owner = sender;
  }
  else {
    QObject::connect(this, SIGNAL(releaseRoom(QObject *, int)), sender, SLOT(releaseRoom(QObject *, int)), Qt::DirectConnection);
    emit releaseRoom(this, perhaps->getId());
    QObject::disconnect(this, SIGNAL(releaseRoom(QObject *, int)), sender, 0);
    if (matchedRoom != 0) moreThanOne = true; 
  }
}

Approved::~Approved() {
  if(owner) {
    
    if (moreThanOne) {
      QObject::connect(this, SIGNAL(releaseRoom(QObject *, int)), owner, SLOT(releaseRoom(QObject *, int)), Qt::DirectConnection);
      emit releaseRoom(this, matchedRoom->getId());
      QObject::disconnect(this, SIGNAL(releaseRoom(QObject *, int)), owner, 0);
    }
    else {
      QObject::connect(this, SIGNAL(keepRoom(QObject *, int)), owner, SLOT(keepRoom(QObject *, int)), Qt::DirectConnection);
      emit keepRoom(this, matchedRoom->getId());
      QObject::disconnect(this, SIGNAL(keepRoom(QObject *, int)), owner, 0);
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
    QObject::connect(this, SIGNAL(releaseRoom(QObject *, int)), owner, SLOT(releaseRoom(QObject *, int)), Qt::DirectConnection);
    emit releaseRoom(this, matchedRoom->getId());
    QObject::disconnect(this, SIGNAL(releaseRoom(QObject *, int)), owner, 0);
  }
  matchedRoom = 0;
  moreThanOne = false;
  owner = 0;
}



QObject * Approved::getOwner() {
  if (moreThanOne) return 0;
  else return owner;
}
