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
  if (holdCount == 0) {
    admin->removeRoom(id);
    rmm.deactivate(this);
  }
  else if (holdCount < 0) 
    printf("fatal: trying to double-remove room\n");
}


void Room::init(ParseEvent * event, RoomCollection * _home) {

  TinyList<Property *> * _properties = event->getProperties();

  #ifdef DEBUG
  fprintf(stderr, "creating room: %s\n", _properties->get(0)->getText());
  #endif

  for (char i = 0; _properties->get(i); i++)
    properties.put(i, _properties->get(i)->copyString());

  TinyList<Property *> * _optionalProperties = event->getOptionals();
  for (char i = 0; _optionalProperties->get(i) != 0; i++)
    optionalProperties.put(i, _optionalProperties->get(i)->copyString());

  home = _home;
}

RoomCollection * Room::getNeighbours(int k) {
  return exits.get(k);
}

void Room::addExit(int direc, Room * target) {
  RoomCollection * roomsInDir = exits.get(direc);
  if (roomsInDir == 0) {
    roomsInDir = rcmm.activate();
    exits.put(direc, roomsInDir);
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
      ret->merge(exits.get(i));
    }
    return ret;
  }
  else {
    ret->merge(exits.get(dir->type));
    return ret;
  }
}

/*
 * return all properties to pmm and remove this room from the room collection
 */
void Room::clear() {

  #ifdef DEBUG
  fprintf(stderr, "deleting room: '%s' with id %i\n", properties.get(0)->getText(), id);
  #endif

  for (char i = 0; properties.get(i) != 0; i++) {
    ssmm.deactivate(properties.get(i));
    properties.remove(i);
  }
  for (char i = 0; optionalProperties.get(i) != 0; i++) {
    ssmm.deactivate(optionalProperties.get(i));
    optionalProperties.remove(i);
  }
  for (unsigned int i = 0; i < exits.size(); i++) 
    if (exits.get(i) != 0) exits.get(i)->clear();
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
bool Room::containsOptionals(TinyList<Property *> * optionals) {
  char i;
  char j;
	
  bool matched = false;
  for (i = 0; optionals->get(i) != 0; i++) {
    for (j = 0; optionalProperties.get(j) != 0; j++) {
      if (optionals->get(i)->equals(optionalProperties.get(j))) {
	matched = true;
	break;
      }
    }
    if (!matched) return false;
    else matched = false;
  }
  return true;
}

/**
 * we allow a tolerance for typoes and such like pandora does
 */
bool Room::fastCompare(ParseEvent * ev, int tolerance) {
  char j = 0;
  TinyList<Property *> * props = ev->getProperties(); 
  char i = 0;
    		
  for (; props->get(i) != 0 && properties.get(j) != 0; i++, j++) {
    tolerance -= props->get(i)->compare(properties.get(j));
    if (tolerance <= 0) return false;
  }
  return true;
}
				
