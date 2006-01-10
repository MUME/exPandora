#include "Room.h"
#include "RoomCollection.h"



ObjectRecycler<Room> rmm;


Room::Room() {
  c = 0;
  id = 0;
  terrain = 0;
  unique = false;
}


void Room::init(ParseEvent * event) {

  TinyList<Property *> * x_properties = event->getProperties();

#ifdef DEBUG
  fprintf(stderr, "creating room: %s\n", x_properties->get(0)->getText());
#endif

  for (char i = 0; x_properties->get(i); ++i)
    properties.put(i, x_properties->get(i)->copyString());

  TinyList<Property *> * x_optionalProperties = event->getOptionals();
  for (char i = 0; x_optionalProperties->get(i) != 0; i++)
    optionalProperties.put(i, x_optionalProperties->get(i)->copyString());
}


void Room::addExit(int direc, int target) {
  set<int> * roomsInDir = exits.get(direc);
  if (roomsInDir == 0) {
    roomsInDir = new set<int>;
    exits.put(direc, roomsInDir);
  }
  roomsInDir->insert(target);
}


set<int> * Room::go(BaseEvent * dir) {
  set<int> * ret  = new set<int>();;
  Coordinate * move = Coordinate::stdMoves[dir->type];
  if (move->x == move->y == move->z == 0) {
    ret->insert(id);
    return ret;
  }
  else if (dir->type == UNKNOWN) {
    for (unsigned int i = 0; i < exits.size(); i++) {
      ret->insert(exits.get(i)->begin(), exits.get(i)->end());
    }
    return ret;
  }
  else {
    set<int> * dirExits = exits.get(dir->type);
    if (dirExits) ret->insert(exits.get(dir->type)->begin(), exits.get(dir->type)->end());
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
  for (char i = 0; optionalProperties.get(i) != 0; ++i) {
    ssmm.deactivate(optionalProperties.get(i));
    optionalProperties.remove(i);
  }
  for (unsigned int i = 0; i < exits.size(); ++i) {
    if (exits.get(i)) exits.get(i)->clear();
  }
  for (unsigned int i = 0; i < reverseExits.size(); ++i) {
    if (reverseExits.get(i)) reverseExits.get(i)->clear();
  }

  terrain = 0;
  cmm.deactivate(c);
  c = 0;
  unique = false;
  id = 0;
}
	
Room::~Room() {
  clear();
  for (unsigned int i = 0; i < exits.size(); ++i) {
    if (exits.get(i) != 0) delete(exits.get(i));
  }
  for (unsigned int i = 0; i < reverseExits.size(); ++i) {
    if (reverseExits.get(i) != 0) delete(reverseExits.get(i));
  }
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
				
bool Room::isNew() {
  for (unsigned int i = 0; i < reverseExits.size(); ++i) {
    if (reverseExits.get(i) && !reverseExits.get(i)->empty()) return false;
  }
  return true;
}
