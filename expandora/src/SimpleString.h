#ifndef SIMPLESTRING
#define SIMPLESTRING
#include "TinyList.h"
#include "ObjectRecycler.h"

class SimpleString : public TinyList<char> {
 public:
  virtual ~SimpleString() {}
  virtual int compare(SimpleString * other);
  virtual bool equals(SimpleString * other);
  virtual char * getText() {return TinyList<char>::list;}
  virtual SimpleString * copy();
  virtual void clear();
 protected:
  void copy(SimpleString * other);
};

extern ObjectRecycler<SimpleString> ssmm;

#endif
