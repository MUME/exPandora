#ifndef BASEEVENT
#define BASEEVENT
#define UNIQUE -1
#define MOVE_FAIL -2 
//#include "ObjectRecycler.h"



class BaseEvent {
	public:
		void clear() {timestamp = 0; type = 0;};
		double timestamp;
		char type;
};

//extern ObjectRecycler<BaseEvent> bemm;
#endif
