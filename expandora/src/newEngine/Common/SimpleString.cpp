#include <string>
#include "SimpleString.h"

ObjectRecycler<SimpleString> ssmm;

int SimpleString::compare(SimpleString * other) {
    return strcmp(list, other->getText());
}

bool SimpleString::equals(SimpleString * other) {
  if (other->size() != listSize) return false;
  for (unsigned int i = 0; i < listSize; i++) {
    if (list[i] != other->get(i)) return false;
  }
  return true;
}

SimpleString * SimpleString::copy() {
  SimpleString * other = ssmm.activate();
  other->copy(this);
  return other;
}

void SimpleString::copy(SimpleString * other) {
  char * otherText = other->getText();
  int newLen = other->size();
  put (newLen-1, 0);
  strncpy(list, otherText, newLen);
  hint = other->getHint();

}
    
