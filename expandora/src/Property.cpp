#include "Property.h"
#include <string.h>

void Property::skip() {
	used = SKIPPED_ONE;
}

void Property::skipMany() {
	used = SKIPPED_MANY;
}

int Property::size() {
	return used;
}

void Property::clear() {
	used = 0;
}

Property::Property() {
	used = 0;
	length = 0;
	begin = 0;
}

int Property::operator==(Property other) {
	if (other.size() != used) return 0;
	for (int i = 0; i<used; i++) if (begin[i] != other[i]) return 0;
	return 1;
}

void Property::operator+=(char c) {
	enlarge(used+1);
	if (used >= 0) begin[used] = c;
	used++;
}

void Property::operator+=(char * string) {
	int needed = used + strlen(string);
	enlarge(needed);
	
	for (int i = used; i < needed; i++) {
		if (i >= 0) begin[i] = string[i-used];
	}
	used = needed;
}


void Property::enlarge(int neededSpace) {
	while (neededSpace > length) {
		length = length * 2 + 1;
		char * newBegin = new char[length];
		for (int i = 0 ; i < used; i++) newBegin[i] = begin[i];
		for (int i = used; i <length; i++) newBegin[i] = 0;
		delete(begin);
		begin = newBegin;
	}
}

char & Property::operator[](int offset) {
	if (offset > length) enlarge(offset);
	return begin[offset];
}
	

