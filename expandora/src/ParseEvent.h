#ifndef PARSEEVENT
#define PARSEEVENT

// skipped properties
#define SKIPPED_PROPERTIES 0
#define UKNOWN_NUMBER 0

//event types
#define ROOM 1
#define MOVE 2
#define SCOUT 3
#define MOVE_FAIL 4

#include "ObjectRecycler.h"
#include "Property.h"
#include <list.h>
#include <time.h>


using namespace std;


/**	
 * the ParseEvents will walk around in the SearchTree
 * The terrain type is remembered outside the Event and passed to the renderer if we found a new room
 */
class ParseEvent {
	public:
		ParseEvent() {pos = 0; type = 0;}
		ParseEvent(ParseEvent * other);
		
		void push(char * begin); // various ways to push properties to the end of the list
		void push(Property & newProp) ;
		void pushOptional(char * begin);
		void pushOptional(Property & newProp);
		
		Property pop(); // gets the first property and removes it
		
		char type;	// bad style
		double timestamp; // when did the event occur? - needed to determine causality in the parser
		
		void clear();	// for the ObjectRecycler
		void copy(ParseEvent * other);
		
		Property & next();	// next Property according to pos (updating pos)
		Property & prev();	// previous  -"-
		Property & current();	// current Property according to pos (pos stays as it is)
		
		list<Property>::iterator getPos() {return pos;}	// for (shallow) copying
		list<Property> & getOptionals() {return optionals;}
		list<Property> & getProperties() {return required;}
	private:
		list<Property> required;
		list<Property> optionals;
		list<Property>::iterator pos;
};

extern ObjectRecycler<ParseEvent> pemm;

#endif
