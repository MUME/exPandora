#ifndef PARSEEVENT
#define PARSEEVENT

// skipped properties
#define SKIPPED_PROPERTIES 0
#define UKNOWN_NUMBER 0

//event types
#define ROOM -3 
#define NOTE -4


#include <list>
#include <time.h>
#include "ObjectRecycler.h"
#include "Property.h"
#include "BaseEvent.h"

using namespace std;


/**	
 * the ParseEvents will walk around in the SearchTree
 * The terrain type is remembered outside the Event and passed to the renderer if we found a new room
 */
class ParseEvent : public BaseEvent {
	public:
		ParseEvent() {pos = 0; type = 0;}
		
		void push(Property * newProp) ;
		void pushOptional(Property * newProp);
		
		void clear();	// for the ObjectRecycler
		ParseEvent * copy();

		Property * next();	// next Property according to pos (updating pos)
		Property * prev();	// previous  -"-
		Property * current();	// current Property according to pos (pos stays as it is)
		
		list<Property *>::iterator getPos() {return pos;}	// for (shallow) copying
		list<Property *> * getOptionals() {return &optionals;}	
		list<Property *> * getProperties() {return &required;}
	private:
		void copy(ParseEvent * other);
		void recycleProperties();
		list<Property *> required;
		list<Property *> optionals;
		list<Property *>::iterator pos;
};

extern ObjectRecycler<ParseEvent> pemm;

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
