#include "Property.h"
#include <string.h>

Property NO_PROPERTY; // 0,0,0,0
ObjectRecycler<Property> pmm;

char * Property::rest() {
	return begin + currentOffset;
}

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
	currentOffset = 0;
}

void Property::copy(Property & other) {
	for (int i = 0; other[i] != 0; i++) {
		add(other[i]);
	}
	add('\0');
	currentOffset = other.getOffset();
}

Property::Property() {
	used = 0;
	length = 0;
	begin = 0;
	currentOffset = 0;
}

Property::Property(char * in) {
	begin = in;
	length = strlen(in);
	used = length;
	currentOffset = 0;
}

int Property::comp(Property & other) {
	int ret = 0;
	for (int i = 0; i<used; i++) if (begin[i] != other[i]) ret++;
	return ret;
}

int Property::operator==(Property & other) {
	if (used != other.size()) return 0;
	for (int i = 0; i<used; i++) if (begin[i] != other[i]) return 0;
	return 1;
}

void Property::add(char c) {
	enlarge(used+1);
	if (used >= 0) begin[used] = c;
	used++;
}

void Property::add(char * string) {
	int needed = used + strlen(string);
	enlarge(needed);
	
	for (int i = used; i < needed; i++) {
		if (i >= 0) begin[i] = string[i-used];
	}
	used = needed;
}

void Property::add(char * other, char * end) {
	int needed = used + other - end;
	enlarge(needed);

	for (int i = used; i < needed; i++) {
	if (i >= 0) begin[i] = other[i-used];
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

char Property::operator[](int offset) {
	if (offset >= length) enlarge(offset+1);
	else if (offset < 0) return begin[length - 1];
	return begin[offset];
}


char Property::current() {
	return operator[](currentOffset);
}
		
char Property::next() {
	return operator[](++currentOffset);
}
		
char Property::prev() {
	return operator[](--currentOffset);
}

