#ifndef RANDOMEXIT 
#define RANDOMEXIT
#include "Exit.h"
#include <list>
using namespace std;

class RandomExit : public Exit {
	private:
		list<Room *> goingTo;
}

#endif
