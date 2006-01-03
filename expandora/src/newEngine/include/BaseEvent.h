#ifndef BASEEVENT
#define BASEEVENT

//event types
#define ROOM -3 
#define NOTE -4
#define UNIQUE -1
#define MOVE_FAIL -2 
#define UNKNOWN -5

//#include "ObjectRecycler.h"



class BaseEvent {
	public:
		void clear() {timestamp = 0; type = 0;};
		long timestamp;
		signed char type;
};

//extern ObjectRecycler<BaseEvent> bemm;
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
