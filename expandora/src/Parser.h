#ifndef PARSER
#define PARSER
#include "Terrain.h"
#include "ParseEvent.h"
#include "Property.h"
#include "Path.h"
#include "Room.h"
#include <queue>
#include <list>
using namespace std;

#define APPROVED 0
#define EXPERIMENTING 1
#define SYNCING 2

class Parser {
	public:
		void setTerrain(Property * ter);
		void event(ParseEvent * ev);
	private:
		void dropNote(ParseEvent * ev);
		void checkQueues();
		char state;
		queue<ParseEvent *> playerEvents;
		queue<ParseEvent *> mudEvents;
		Terrain * activeTerrain;
		list<Path *> possiblePaths;
		Room * mostLikelyRoom;
};	

extern Parser parser;
#endif
