#include "ParseEvent.h"

void ParseEvent::clear() {
	list<property>::clear();
	type = 0;
}


void ParseEvent::push(char * begin, char * end) {
	property ins = {begin, end};
	push_back(ins);
}


property ParseEvent::pop() {
	property ret = front();
	pop_front();
	return ret;
}






