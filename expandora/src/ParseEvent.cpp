#include "ParseEvent.h"
#include <math.h>
#include "defines.h"
#include "utils.h"

ObjectRecycler<ParseEvent> pemm;

void ParseEvent::clear() {
	required.clear();
	optionals.clear();
	type = 0;
	timestamp = 0;
	pos = 0;
}

ParseEvent::ParseEvent(ParseEvent * other) {
	timestamp = other->timestamp;
	type = other->type;
	pos = other->getPos();	
	optionals = other->getOptionals();
	required = other->getProperties(); 
}

void ParseEvent::push(Property & newProp) {
	required.push_back(newProp);
	if (timestamp == 0) timestamp = m_timestamp();
}

void ParseEvent::push(char * begin) {
	Property ins(begin);
	required.push_back(ins);
	if (timestamp == 0) timestamp = m_timestamp();
}

void ParseEvent::pushOptional(char * begin) {
	Property ins(begin);
	optionals.push_back(ins);
	if (timestamp == 0) timestamp = m_timestamp();
}
	

void ParseEvent::pushOptional(Property & newProp) {
	optionals.push_back(newProp);
	if (timestamp == 0) timestamp = m_timestamp();
}

Property ParseEvent::pop() {
	Property ret = required.front();
	required.pop_front();
	return ret;
}


Property & ParseEvent::next() {
	if (pos == 0) pos = required.begin();
	else pos++;
	if (pos == required.end()) {
		pos = 0;
		return NO_PROPERTY;
	}
	else return *pos;
}

Property & ParseEvent::prev() {
	if (pos == 0) pos = required.end();
	if (pos == required.begin()) {
		pos = 0;
		return NO_PROPERTY;
	}
	else return *--pos;
}		
	
Property & ParseEvent::current() {
	if (pos == 0) pos = required.begin();
	if (pos == required.end()) return NO_PROPERTY;
	else return *pos;
}




