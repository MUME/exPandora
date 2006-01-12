
#ifndef PROPERTY
#define PROPERTY

#define SKIPPED_ONE INT_MAX
#define SKIPPED_MANY INT_MAX-1

#include <stdlib.h>
#include "ObjectRecycler.h"
#include "ListCycler.h"
#include "ListStack.h"
#include "SimpleString.h"

class Property : public ListCycler<char>, private ListStack<char> {
 public:	

  Property(); 
  ~Property() {}
  void clear();
  void skip();
  void skipMany();
		
  char * rest();

  int size() {return tos;}
  void add(char c);
  void add(char * string);
  void add(const char * other, const char * end);
  char get(unsigned int i) {return data.get(i);}
  char * getText() {return data.getText();}
  Property * copy();
  SimpleString * copyString() {return data.copy();}
  bool equals (SimpleString * other) {return data.equals(other);}
  int compare(SimpleString * other) {return data.compare(other);}
  void hint(char hint) {data.setHint(hint);}
 private:
  SimpleString data;
  //int used;
};


extern ObjectRecycler<Property> pmm;


#ifdef DMALLOC
#include <dmalloc.h>
#endif
#endif

