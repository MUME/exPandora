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


using namespace std;

typedef struct property {
	char * begin;
	char * end;
} property;

/**	
 * the ParseEvents will walk around in the SearchTree
 * The terrain type is remembered outside the Event and passed to the renderer if we found a new room
 */
class ParseEvent : public list<property> {
	public:
		void push(char * begin, char * end);
		void push(property newProp) {push_back(newProp);}
		property pop();
		char type;
		void clear();
};
#endif
