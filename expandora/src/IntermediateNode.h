#ifndef INTERMEDIATENODE
#define INTERMEDIATENODE
#include "SearchTreeNode.h"
#include "RoomCollection.h"
#include "Room.h"
#include <vector>
using namespace std;
/**
 * IntermediateNodes represent possible ends of a property
 * they hold a RoomCollection if this property can be the last one
 */
class IntermediateNode : public SearchTreeNode {
	public:
		IntermediateNode(char * _myChars, RoomSearchNode * parent);
		Room * insertRoom(vector<char *> & properties, int pos); //increments pos
		RoomCollection * getRooms(vector<char *> & properties, int pos); // increments pos
	private:
		RoomCollection * rooms;
};

#endif
