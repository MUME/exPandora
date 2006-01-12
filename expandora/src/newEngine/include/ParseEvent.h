#ifndef PARSEEVENT
#define PARSEEVENT

// skipped properties
#define SKIPPED_PROPERTIES 0
#define UKNOWN_NUMBER 0


#include <stdlib.h>
#include "ObjectRecycler.h"
#include "TinyList.h"
#include "ListStack.h"
#include "Property.h"
#include "BaseEvent.h"
#include "ListCycler.h"



/**	
 * the ParseEvents will walk around in the SearchTree
 * The terrain type is remembered outside the Event and passed to the renderer if we found a new room
 */
class ParseEvent : public BaseEvent, public ListCycler<Property *> {
 public:
  ParseEvent(); 

  void push(Property * newProp) ;
  void pushOptional(Property * newProp);
		
  void clear();	// for the ObjectRecycler
  ParseEvent * copy();
  void reset();
	
  TinyList<Property *> * getOptionals() {return &optionals;}	
  TinyList<Property *> * getProperties() {return &required;}

 private:
  void copy(ParseEvent * other);
  void recycleProperties();
  TinyList<Property *> required;
  TinyList<Property *> optionals;
  ListStack<Property *> reqStack;
  ListStack<Property *> optStack;
};

extern ObjectRecycler<ParseEvent> pemm;



#ifdef DMALLOC
#include <dmalloc.h>
#endif
#endif
