#ifndef INTERMEDIATENODE
#define INTERMEDIATENODE

#include "AbstractRoomContainer.h"
#include "SearchTreeNode.h"
#include "RoomSearchNode.h"
#include "RoomCollection.h"
#include "Room.h"


/**
 * IntermediateNodes represent possible ends of a property
 * they hold a RoomSearchNode if this property can be the last one
 */
class IntermediateNode : public SearchTreeNode {
 public:
  IntermediateNode() : SearchTreeNode("") {rooms = 0;}
  IntermediateNode(ParseEvent * event);
  RoomCollection * insertRoom(ParseEvent * event); 
  AbstractRoomContainer * getRooms(ParseEvent * event);
  AbstractRoomContainer * skipDown(ParseEvent * event);       	
 private:
  RoomCollection * rooms;
};

#endif

#ifdef DMALLOC
#include <mpatrol.h>
#endif
