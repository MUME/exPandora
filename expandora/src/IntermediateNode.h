#ifndef INTERMEDIATENODE
#define INTERMEDIATENODE
#include "SearchTreeNode.h"
#include "RoomCollection.h"
#include "Room.h"
/**
 * IntermediateNodes represent possible ends of a property
 * they hold a RoomCollection if this property can be the last one
 */
class IntermediateNode : public SearchTreeNode {
	public:
		IntermediateNode(char * _myChars, RoomSearchNode * parent);
		Room * insertRoom(char ** properties, int pos, int numProperties); //increments pos
		RoomCollection * getRooms(char ** properties, int pos, int numProperties); // increments pos
	private:
		RoomCollection * rooms;
};

#endif
