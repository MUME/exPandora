#ifndef PATH
#define PATH
#include <list>
#include "Room.h"

using namespace std;

class Path {
	public:
		bool hasChildren() {return (!children.empty());};
		void init(Room * room);
		Room * getRoom() {return room;};
		Path * fork(Room * room); //new Path is fetched from pamm, distance between rooms is calculated and probability is updated accordingly
		double getProb() {return probability;};	
		void approve();
		void deny(); 	// removes this path and all parents up to the next branch 
				// and gives them back to the pamm 
				// and removes the respective rooms if experimental
		void clear();
		void cutDeadBranch(); 	// removes this path and recursively all chldren 
					// and gives them back to the pamm
					// and removes all respective experimental rooms
	private:
		Path * parent;
		list<Path *> children;
		double probability;
		Room * room; // in fact a path only has one room, one parent and soem children(forks).
};

extern ObjectRecycler<Path> pamm;

#endif
