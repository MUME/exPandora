#include "ParseEvent.h"
#include <math.h>
#include "defines.h"
#include "utils.h"

property NO_PROPERTY = {(char *)INT_MAX, (char *)INT_MAX};

void ParseEvent::clear() {
	required.clear();
	optionals.clear();
	type = 0;
	timestamp = 0;
}


void ParseEvent::push(property newProp) {
	required.push_back(newProp);
	if (timestamp == 0) timestamp = m_timestamp();
}

void ParseEvent::push(char * required.begin, char * required.end) {
	property ins = {begin, end};
	required.push_back(ins);
	if (timestamp == 0) timestamp = m_timestamp();
}

void pushOptional(char * begin, char * end) {
	property ins = {begin, end};
	optionals.push_back(ins);
	if (timestamp == 0) timestamp = m_timestamp();
}
	

void pushOptional(property newProp);
	optionals.push_back(newProp);
	if (timestamp == 0) timestamp = m_timestamp();
}

property ParseEvent::pop() {
	property ret = required.front();
	required.pop_front();
	return ret;
}


property ParseEvent::next() {
	if (pos == 0) pos = required.begin();
	else pos++;
	if (pos == required.end()) {
		pos = 0;
		return NO_PROPERTY;
	}
	else return *pos;
}

property ParseEvent::prev() {
	if (pos == 0) pos = required.end();
	if (pos == required.begin()) {
		pos = 0;
		return NO_PROPERTY;
	}
	else return *--pos;
}		
	
property ParseEvent::current() {
	if (pos == 0) pos = required.begin();
	if (pos == required.end()) return NO_PROPERTY;
	else return *pos;
}




