#ifndef EXIT
#define EXIT
#include "Roomcollection.h"

class Room;

class Exit {
	public:
		Room * comingFrom;
		RoomCollection * goingTo;	
};

#endif
