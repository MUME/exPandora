#ifndef PROPERTY
#define PROPERTY

#define SKIPPED_ONE -1
#define SKIPPED_MANY -2

#include "ObjectRecycler.h"

class Property {
 public:	

  Property(); 
  Property(char *);

  void clear();
  void skip();
  void skipMany();
  int size();
		
  char current();
  char next();
  char prev();
  char get(int k);
		
  char * rest();
  char * getText();	

  void add(char c);
  void add(char * string);
  void add(const char * other, const char * end);

  int comp(Property * other);
  void copy(Property * other);
  Property * copy();
  
  int getOffset(){return currentOffset;};
		
 private:
  int currentOffset;
  char * begin;
  int length;
  int used;
  void enlarge(int neededSpace);
		
};


extern ObjectRecycler<Property> pmm;
#endif
