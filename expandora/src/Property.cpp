#include "Property.h"
#include <string.h>

ObjectRecycler<Property> pmm;

char * Property::rest() {
  if(currentOffset == -1) return begin;
  else return begin + currentOffset;
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
  currentOffset = -1;
}

char * Property::getText() {
  return begin;
}

char Property::get(int offset) {
  if (used >= 0 && offset < used && offset >= 0) return begin[offset];
  else return 0;
}

void Property::copy(Property * other) {
  currentOffset = other->getOffset();
  char * text = other->getText();
  used = other->size();
  if (text >= 0) {
    begin = new char[used+1];
    strncpy(begin, text, used);
    begin[used] = 0;
    length = used+1;
  }
  else {
    begin = 0;
    length = 0;
  }
}

Property * Property::copy() {
  Property * other = pmm.activate();
  other->copy(this);
  return other;
}

Property::Property() {
  used = 0;
  length = 0;
  begin = 0;
  currentOffset = -1;
}

Property::Property(char * in) {
  begin = in;
  length = strlen(in);
  used = length;
  currentOffset = -1;
}

int Property::comp(Property * other) {
  return strcmp(begin, other->getText());
}

void Property::add(char c) {
  enlarge(used+1);
  if (used >= 0) {
    begin[used] = c;
  }
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

void Property::add(const char * other, const char * end) {
  int needed = used + end - other;
  enlarge(needed);

  for (int i = used; i < needed; i++) {
    if (i >= 0) begin[i] = other[i-used];
  }
  used = needed;
}	
	
void Property::enlarge(int neededSpace) {
  // make sure the new string is 0-terminated
  while (neededSpace >= length) {
    length = length * 2 + 1;
    char * newBegin = new char[length];
    for (int i = 0 ; i < used; i++) newBegin[i] = begin[i];
    for (int i = used; i <length; i++) newBegin[i] = 0;
    if (begin != 0) delete[] begin;
    begin = newBegin;
  }
  begin[neededSpace] = 0;
}

char Property::current() {
  if(currentOffset == -1) currentOffset = 0;
  return get(currentOffset);
}
		
char Property::next() {
  return get(++currentOffset);
}
		
char Property::prev() {
  return get(--currentOffset);
}

