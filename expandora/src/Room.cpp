#include "Room.h"
#include "RoomCollection.h"
#include "LexDefs.h"

int defaultTolerance = 1;

ObjectRecycler<Room> rmm;

void Room::init(ParseEvent * event) {
	experimental = true;
	properties = event->getProperties();
	optionalProperties = event->getOptionals();
	unique = false;
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
	terrain = 0;
	timestamp = 0;
	home->removeRoom(this);
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
				
