#define UNIQUE -1
#define MOVE_FAIL -2 


// essential exits used by any mud that can be represented with a map ...
#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3
#define UP 4
#define DOWN 5
#define NONE 6


class BaseEvent {
	public:
		double timestamp;
		char type;
};
