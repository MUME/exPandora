#include "Property.h"
#include <cstring>

ObjectRecycler<Property> pmm;

char * Property::rest() {
  return data.getText() + pos;
}

void Property::skip() {
  used = SKIPPED_ONE;
}

void Property::skipMany() {
  used = SKIPPED_MANY;
}


void Property::clear() {
  data.clear();
  pos = data.size();
  used = 0;
}






Property * Property::copy() {
  Property * other = pmm.activate();
  char * text = data.getText();
  other->add(text, text+used);
  return other;
}


void Property::add(char c) {
  data.put(used, c);
  used++;
  pos = data.size();
}

void Property::add(char * string) {
  char newUsed = used + strlen(string);
  if (newUsed < 0); //throw exception
  else { 
    data.put(newUsed-1, 0);
    strncpy(data.getText()+used, string, newUsed - used);
    pos = data.size();
    used = newUsed;
  }
}

void Property::add(const char * other, const char * end) {
  char needed = used + end - other;
  if (needed < 0); //throw exception
  else {
    data.put(needed-1, 0);
    strncpy(data.getText()+used, other, end-other);
    pos = data.size();
    used = needed;
  }
}	
