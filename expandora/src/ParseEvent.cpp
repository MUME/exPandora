#include "ParseEvent.h"
#include <math.h>
#include "defines.h"
#include "utils.h"

property NO_PROPERTY = {(char *)INT_MAX, (char *)INT_MAX};

void ParseEvent::clear() {
	list<property>::clear();
	type = 0;
	timestamp = 0;
}


void ParseEvent::push(property newProp) {
	push_back(newProp);
	if (timestamp == 0) timestamp = m_timestamp();
}

void ParseEvent::push(char * begin, char * end) {
	property ins = {begin, end};
	push_back(ins);
	if (timestamp == 0) timestamp = m_timestamp();
}


property ParseEvent::pop() {
	property ret = front();
	pop_front();
	return ret;
}


property ParseEvent::next() {
	if (pos == 0) pos = begin();
	else pos++;
	if (pos == end()) {
		pos = 0;
		return NO_PROPERTY;
	}
	else return *pos;
}

property ParseEvent::prev() {
	if (pos == 0) pos = end();
	if (pos == begin()) {
		pos = 0;
		return NO_PROPERTY;
	}
	else return *--pos;
}		
	
property ParseEvent::current() {
	if (pos == 0) pos = begin();
	if (pos == end()) return NO_PROPERTY;
	else return *pos;
}




