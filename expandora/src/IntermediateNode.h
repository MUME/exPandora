#ifndef INTERMEDIATENODE
#define INTERMEDIATENODE
#include "SearchTreeNode.h"
#include "RoomSearchNode.h"
#include "RoomCollection.h"
#include "Room.h"
#include <vector>
using namespace std;
/**
 * IntermediateNodes represent possible ends of a property
 * they hold a RoomSearchNode if this property can be the last one
 */
class IntermediateNode : public SearchTreeNode {
	public:
		IntermediateNode(ParseEvent * event);
		Room * insertRoom(ParseEvent * event); 
		RoomSearchNode * getRooms(ParseEvent * event);
		RoomSearchNode * skipDown(ParseEvent * event);       	
	private:
		RoomCollection * rooms;
};

#endif
