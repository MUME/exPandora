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
		Parser(RoomAdmin * _admin);
		void event(ParseEvent * ev);
		Room * getMostLikely() {return mostLikelyRoom;}
	private:
		void mudPop();
		void playerPop();
		void unify();
		Coordinate * getExpectedCoordinate();
		void dropNote(ParseEvent * ev);
		void checkQueues();
		void experimenting();
		void syncing();
		void approved();
		void enlargePaths(RoomCollection * enlargingRooms, bool createNew);
		void buildPaths(RoomCollection * initialRooms);

		char state;
		queue<ParseEvent *> playerEvents;
		queue<ParseEvent *> mudEvents;
		Terrain * activeTerrain;
		list<Path *> paths;
		Room * mostLikelyRoom;
		RoomAdmin * admin;
};	

extern Parser parser;
#endif
