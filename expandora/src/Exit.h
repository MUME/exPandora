#ifndef EXIT
#define EXIT

class Room;

class Exit {
	private:
		Room * comingFrom;
		Room ** goingTo;	// possibly a random exit
		int numGoingTo;
}

#endif
