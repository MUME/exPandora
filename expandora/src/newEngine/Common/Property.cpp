#include <string>
#include "Property.h"


ObjectRecycler<Property> pmm;

Property::Property()
{
  data.put(127, 0);
  ListCycler<char>::attachList(&data);
  ListStack<char>::attachList(&data);
}

char * Property::rest()
{
  if (tos == SKIPPED_ONE || tos == SKIPPED_MANY)
    throw "can't get a string from a SKIP property";
  return data.getText() + pos;
}

void Property::skip()
{
  tos = SKIPPED_ONE;
}

void Property::skipMany()
{
  tos = SKIPPED_MANY;
}


void Property::clear()
{
  data.clear();
  pos = data.size();
  tos = 0;
}

Property * Property::copy()
{
  Property * other = pmm.activate();
  if (tos == SKIPPED_ONE) {
    other->skip();
  }
  else if (tos == SKIPPED_MANY) {
    other->skipMany();
  }
  else {
    char * text = data.getText();
    other->add(text, text+tos);
  }
  return other;
}


void Property::add(char c)
{
  if (tos == SKIPPED_ONE || tos == SKIPPED_MANY)
    throw "can't add a char to a SKIP property";
  ListStack<char>::push(c);
  pos = data.size();
}

void Property::add(char * string)
{
  if (tos == SKIPPED_ONE || tos == SKIPPED_MANY)
    throw "can't add a string to a SKIP property";
  signed char newUsed = tos + strlen(string);
  data.put(newUsed-1, 0);
  strncpy(data.getText()+tos, string, newUsed - tos);
  pos = data.size();
  tos = newUsed;

}

void Property::add(const char * other, const char * end)
{
  if (tos == SKIPPED_ONE || tos == SKIPPED_MANY)
    throw "can't add a substring to a SKIP property";
  if (other == end) return; // nothing to be done
  signed char needed = tos + end - other;


  data.put(needed-1, 0);
  strncpy(data.getText()+tos, other, end-other);
  pos = data.size();
  tos = needed;

}
