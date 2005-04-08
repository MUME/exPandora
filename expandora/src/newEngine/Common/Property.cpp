#include "Property.h"
#include <string.h>

ObjectRecycler<Property> pmm;

Property::Property() {
  data.put(127, 0); 
  ListCycler<char>::attachList(&data);
  ListStack<char>::attachList(&data);
}

char * Property::rest() {
  return data.getText() + pos;
}

void Property::skip() {
  tos = SKIPPED_ONE;
}

void Property::skipMany() {
  tos = SKIPPED_MANY;
}


void Property::clear() {
  data.clear();
  pos = data.size();
  tos = 0;
}






Property * Property::copy() {
  Property * other = pmm.activate();
  char * text = data.getText();
  other->add(text, text+used);
  return other;
}


void Property::add(char c) {
  ListStack<char>::push(c);
  pos = data.size();
}

void Property::add(char * string) {
  char newUsed = tos + strlen(string);
  if (newUsed < 0); //throw exception
  else { 
    data.put(newUsed-1, 0);
    strncpy(data.getText()+tos, string, newUsed - tos);
    pos = data.size();
    tos = newUsed;
  }
}

void Property::add(const char * other, const char * end) {
  char needed = tos + end - other;
  if (needed < 0); //throw exception
  else {
    data.put(needed-1, 0);
    strncpy(data.getText()+tos, other, end-other);
    pos = data.size();
    tos = needed;
  }
}	
