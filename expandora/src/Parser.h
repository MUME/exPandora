#ifndef PARSER
#define PARSER
#include "Terrain.h"
#include "ParseEvent.h"
#include "Property.h"
#include "Path.h"
#include "Room.h"
#include "Coordinate.h"
#include <queue>
#include <list>
using namespace std;

#define APPROVED 0
#define EXPERIMENTING 1
#define SYNCING 2

class Parser {
	public:
		void setTerrain(Property * ter);
		void event(BaseEvent * ev);
	private:
		Coordinate * getExpectedCoordinate();
       		void buildPaths(RoomCollection * rooms);
		void dropNote(ParseEvent * ev);
		void checkQueues();
		void experimenting();
		void syncing();
		void approved();

		char state;
		queue<BaseEvent *> playerEvents;
		queue<BaseEvent *> mudEvents;
		Terrain * activeTerrain;
		list<Path *> possiblePaths;
		Room * mostLikelyRoom;
};	

extern Parser parser;
#endif
