#ifndef BASEEVENT
#define BASEEVENT

//event types
#define ROOM 11 
#define NOTE 12
#define UNIQUE 13
#define MOVE_FAIL 14 
#define UNKNOWN 255
#define MOVE 10


class BaseEvent {
	public:
		void clear() {timestamp = 0; type = 0; subType = 0;};
		unsigned int timestamp;
		unsigned int type;
		unsigned int subType;
};




#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
