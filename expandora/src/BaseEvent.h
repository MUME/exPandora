#ifndef BASEEVENT
#define BASEEVENT
#define UNIQUE -1
#define MOVE_FAIL -2 




class BaseEvent {
	public:
		double timestamp;
		char type;
};

extern ObjectRecycler<BaseEvent> bemm;
#endif
