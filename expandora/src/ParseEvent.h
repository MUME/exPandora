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

#include <list>
#include <time.h>


using namespace std;

typedef struct property {
	char * begin;
	char * end;
} property;
extern property NO_PROPERTY;


/**	
 * the ParseEvents will walk around in the SearchTree
 * The terrain type is remembered outside the Event and passed to the renderer if we found a new room
 */
class ParseEvent : private list<property> {
	public:
		ParseEvent() : list<property>() {pos = 0; type = 0;}
		void push(char * begin, char * end);
		void push(property newProp) ;
		property pop();
		char type;
		void clear();
		property next();
		property prev();
		property current();
	private:
		iterator pos;
		double timestamp; // when did the event occur? - needed to determine causality in the parser
};
#endif
