#include "ParseEvent.h"
#include <math.h>
#include "defines.h"
#include "utils.h"

ObjectRecycler<ParseEvent> pemm;

/**
 * shallow clear: the Properties aren't given back to the object recycler
 */
void ParseEvent::clear() {
	required.clear();
	optionals.clear();
	type = 0;
	timestamp = 0;
	pos = 0;
}


void ParseEvent::copy(ParseEvent * other) {
	list<Property *> & otherProps = other->getProperties;
	list<Property *>::iterator p = otherProps.begin();
	list<Property *>::iterator otherPos = other->getPos();
	Property * prop = 0;
	for (; p != otherProps.end(); p++) {
		prop = pmm.activate();
		prop.copy(*p);
		required.push_back(prop);
		if (p == otherPos) {
			pos = required.end();
			pos--;
		}
	}
	otherProps = other->getOptionals();
	p = otherProps.begin();
	for (; p != otherProps.end(); p++) {
		prop = pmm.activate();
		prop.copy(*p);
		optionals.push_back(prop);
	}
}


/**
 * give the properties back to the recycler - should only be done if they aren't needed in a newly created room
 */
void ParseEvent::recycleProperties() {
	while (!required.empty()) {
		pmm.deactivate(required.front());
		required.pop_front();
	}
	while (!optionals.empty()) {
		pmm.deactivate(optionals.front());
		optionals.pop_front();
	}
	pos = 0;
}

ParseEvent::ParseEvent(ParseEvent * other) {
	timestamp = other->timestamp;
	type = other->type;
	pos = other->getPos();	
	optionals = other->getOptionals();
	required = other->getProperties(); 
}

void ParseEvent::push(Property * newProp) {
	required.push_back(newProp);
	if (timestamp == 0) timestamp = m_timestamp();
}


	

void ParseEvent::pushOptional(Property * newProp) {
	optionals.push_back(newProp);
	if (timestamp == 0) timestamp = m_timestamp();
}

Property * ParseEvent::pop() {
	Property * ret = required.front();
	required.pop_front();
	return ret;
}


Property * ParseEvent::next() {
	if (pos == 0) pos = required.begin();
	else pos++;
	if (pos == required.end()) {
		pos = 0;
		return 0;
	}
	else return *pos;
}

Property * ParseEvent::prev() {
	if (pos == 0) pos = required.end();
	if (pos == required.begin()) {
		pos = 0;
		return 0;
	}
	else return *--pos;
}		
	
Property * ParseEvent::current() {
	if (pos == 0) pos = required.begin();
	if (pos == required.end()) return 0;
	else return *pos;
}




