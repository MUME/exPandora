
#ifndef PROPERTY
#define PROPERTY

#define SKIPPED_ONE -1
#define SKIPPED_MANY -2

#include "ObjectRecycler.h"
#include "ListCycler.h"
#include "SimpleString.h"

class Property : public ListCycler<char> {
 public:	

  Property() : used(0) {data.put(127, 0); attachList(&data);}
  ~Property() {;}
  void clear();
  void skip();
  void skipMany();
		
  char * rest();

  int size() {return used;}
  void add(char c);
  void add(char * string);
  void add(const char * other, const char * end);
  char get(unsigned int i) {return data.get(i);}
  char * getText() {return data.getText();}
  Property * copy();
  SimpleString * copyString() {return data.copy();}
  bool equals (SimpleString * other) {return data.equals(other);}
  int compare(SimpleString * other) {return data.compare(other);}
  
 private:
  SimpleString data;
  int used;
};


extern ObjectRecycler<Property> pmm;
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
