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
		IntermediateNode(ParseEvent * event);
		Room * insertRoom(ParseEvent * event); 
		RoomCollection * getRooms(ParseEvent * event);
		RoomCollection * skipDown(ParseEvent * event);       	
	private:
		RoomCollection * rooms;
};

#endif
