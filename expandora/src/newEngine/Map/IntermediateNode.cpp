#include "IntermediateNode.h"


IntermediateNode::IntermediateNode(ParseEvent * event) {
  Property * prop = event->next();
  if (prop == 0 || prop->size() == SKIPPED_ONE || prop->size() == SKIPPED_MANY) {
    myChars = new char[1];
    myChars[0] = 0;
  }
  else {
    myChars = new char[strlen(prop->rest()) + 1];
    strcpy(myChars, prop->rest());
  }
  rooms = 0;
  event->prev();
}

RoomCollection * IntermediateNode::insertRoom(ParseEvent * event) {
	
  if (event->next() == 0) {
    if (rooms == 0) rooms = rcmm.activate();
    return rooms;
  }
  else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return 0;
	
  return SearchTreeNode::insertRoom(event);
}
	

AbstractRoomContainer * IntermediateNode::getRooms(ParseEvent * event) {
  AbstractRoomContainer * ret = 0;
  if (event->next() == 0) {
    ret = rcmm.activate();
    ret->merge(rooms);
    return ret;
  }
  else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return SearchTreeNode::skipDown(event);
  else {
    ret = SearchTreeNode::getRooms(event);
    return ret;
  }
}

AbstractRoomContainer * IntermediateNode::skipDown(ParseEvent * event) {
  bool many = false;
  ParseEvent * copy = 0;

  if (event->current()->size() == SKIPPED_MANY) {
    many = true;
    copy = event->copy();
  }
  else many = false; 
  RoomCollection * r = (RoomCollection *)getRooms(event);

  if (many) {
    RoomCollection * temp = 0;
    r->merge(rooms);
    temp = (RoomCollection *)SearchTreeNode::skipDown(copy);
    r->merge(temp);
    rcmm.deactivate(temp);
    pemm.deactivate(copy);
  }

  return r;
}
