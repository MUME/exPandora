#ifndef PARSEEVENT
#define PARSEEVENT

// skipped properties
#define SKIPPED_PROPERTIES 0
#define UKNOWN_NUMBER 0

// event types
#define MOVE_FAIL 1
#define ROOM 2
#define MOVE 3
#define SCOUT 4

#include <queue>
using namespace std;

/**	
 * the ParseEvents will walk around in the SearchTree
 * The terrain type is remembered outside the Event and passed to the renderer if we found a new room
 */
class ParseEvent {
	public:
		char type;
		void clear();
		queue<char *> propertyBeginnings;
		queue<char *> propertyEnds;
};
#endif
