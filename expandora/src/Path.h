#ifndef PATH
#define PATH
#include <list>
#include "Room.h"
#include "Exit.h"

using namespace std;

class Path {
	private:
		list<Room *> rooms;
		list<Exit *> exits;
};

#endif
