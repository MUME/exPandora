#ifndef PARSEEVENT
#define PARSEEVENT
#define SKIPPED_PROPERTIES 0
#include "ObjectRecycler.h"
#include <queue>
using namespace std;

class ParseEvent {
	public:
		char type;
		void clear();
		queue<char *> propertyBeginnings;
		queue<char *> propertyEnds;
};
#endif
