#include "Room.h"
#include "RoomCollection.h"
#include "LexDefs.h"
#include "RoomAdmin.h"

int defaultTolerance = 1;

ObjectRecycler<Room> rmm;


Room::Room() {
  c = 0;
  timestamp = 0;
  holdCount = 0;
  id = 0;
  terrain = 0;
  unique = false;
  home = 0;
}


void Room::release(RoomAdmin * admin) {
  holdCount--;
  if (holdCount <= 0) {
    admin->removeRoom(id);
    rmm.deactivate(this);
  }
}

/**
 * what happens with the original properties and optional properties?
 */
void Room::init(ParseEvent * event, RoomCollection * _home) {
  
  list<Property *> _properties = event->getProperties();
  for (list<Property *>::iterator i = _properties.begin(); i != _properties.end(); i++)
    properties.push_back(*i);

  list<Property *> _optionalProperties = event->getOptionals();
  for (list<Property *>::iterator i = _optionalProperties.begin(); i != _optionalProperties.end(); i++)
    optionalProperties.push_back(*i);
  home = _home;
}

RoomCollection * Room::getNeighbours(int k) {
  if (k < (int)exits.size()) {
    if (exits[k] != 0) exits[k]->checkConsistency();
    return exits[k];
  } 
  else return 0;
}

void Room::addExit(int direc, Room * target) {
  if ((int)(exits.size()) <= direc) {
    exits.resize(direc + 1, 0);
  }
  RoomCollection * roomsInDir = exits[direc];
  if (roomsInDir == 0) {
    roomsInDir = rcmm.activate();
    exits[direc] = roomsInDir;
  }
  roomsInDir->addRoom(target);
}


RoomCollection * Room::go(BaseEvent * dir) {
  RoomCollection * ret = rcmm.activate();
  if (dir->type == NONE) {
    ret->addRoom(this);
    return ret;
  }
  else if (dir->type == UNKNOWN) {
    for (unsigned int i = 0; i < exits.size(); i++) {
      ret->merge(exits[i]);
    }
    return ret;
  }
  else if (dir->type <  0 || dir->type >= (int)exits.size()) return ret;
  else {
    ret->merge(exits[dir->type]);
    return ret;
  }
}

/*
 * return all properties to pmm and remove this room from the room collection
 */
void Room::clear() {
  
  while(!properties.empty()) {
    pmm.deactivate(properties.front());
    properties.pop_front();
  }
  while(!optionalProperties.empty()) {
    pmm.deactivate(optionalProperties.front());
    optionalProperties.pop_front();
  }
  for (int i = exits.size() - 1; i >= 0 ; i--) 
    if (exits[i] != 0) exits[i]->clear();
  terrain = 0;
  timestamp = 0;
  holdCount = 0;
  home->removeRoom(this);
  cmm.deactivate(c);
  c = 0;
  unique = false;
  home = 0;
  id = 0;
}
	

/** compare the optional properties that are not present in the search tree
 * perhaps we should allow a tolerance, too?
 */
bool Room::containsOptionals(list<Property *> & optionals) {
  list<Property *>::iterator i;
  list<Property *>::iterator j;
	
  int matched = 0;
  for (i = optionals.begin(); i != optionals.end(); i++) {
    for (j = optionalProperties.begin(); j != optionalProperties.end(); j++) {
      if (!(*i)->comp(**j)) {
	matched = 1;
	break;
      }
    }
    if (!matched) return false;
    else matched = 0;
  }
  return true;
}

/** we only compare the first num properties only to this room here
 * we allow a tolerance for typoes and such like pandora does
 */
bool Room::fastCompare(ParseEvent * ev, int tolerance) {
  list<Property *>::iterator j = properties.begin();
  list<Property *> & props = ev->getProperties(); 
  list<Property *>::iterator i = props.begin();
    		
  for (; i != props.end() && j != properties.end(); i++, j++) {
    tolerance -= (*i)->comp(**j);
    if (tolerance <= 0) return false;
  }
  return true;
}
				
