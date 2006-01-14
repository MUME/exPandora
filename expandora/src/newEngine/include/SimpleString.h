#ifndef SIMPLESTRING
#define SIMPLESTRING
#include "TinyList.h"
#include "ObjectRecycler.h"

class SimpleString : public TinyList<char> {
 public:
  virtual void setHint(char newHint) {hint = newHint;}
  virtual char getHint() {return hint;}
  virtual ~SimpleString() {hint = 0;}
  virtual int compare(SimpleString * other);
  virtual bool equals(SimpleString * other);
  virtual char * getText() {return TinyList<char>::list;}
  virtual SimpleString * copy();
  virtual void clear() {for (unsigned int i = 0; i < listSize; i++) list[i] = 0; hint = 0;}
  SimpleString() : hint(0) {}
 protected:
  void copy(SimpleString * other);
  char hint; // the hint will denote the supposed meaning of the string, for example "Description" or "Exit"
};

extern ObjectRecycler<SimpleString> ssmm;


#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
