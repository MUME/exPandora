#include "Room.h"
#include "RoomCollection.h"
#include "Exit.h"

void Room::init(ParseEvent * event) {
	experimental = false;
	properties = event->getProperties();
	optionalProperties = event->getOptionals();
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
	home->removeRoom(this);
}
	

/** compare the optional properties that are not present in the search tree
 * perhaps we should allow a tolerance, too?
 */
int Room::containsOptionals(list<Property *> & optionals) {
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
		if (!matched) return 0;
		else matched = 0;
	}
	return 1;
}

/** we only compare the first num properties only to this room here
 * we allow a tolerance for typoes and such like pandora does
 */
int Room::fastCompare(list<Property *> & props, int tolerance) {
	list<Property *>::iterator j = properties.begin();
	list<Property *>::iterator i = props.begin(); 
	for (; i != props.end() && j != properties.end(); i++, j++) {
		tolerance -= (*i)->comp(**j);
		if (tolerance <= 0) return 0;
	}
	return 1;
}
				
