#include "IntermediateNode.h"


IntermediateNode::IntermediateNode(ParseEvent * event) : SearchTreeNode((char *)0) {
  Property * prop = event->next();
  if (prop != 0) {
    myChars = new char[strlen(prop->rest()) + 1];
    strcpy(myChars, prop->rest());
  }
  else {
    myChars = new char[1];
    myChars[0] = 0;
  }
  rooms = 0;
  event->prev();
}

Room * IntermediateNode::insertRoom(ParseEvent * event) {
	
  if (event->next() == 0) {
    if (rooms == 0) rooms = rcmm.activate();
    Room * ret = rooms->insertRoom(event);
    
    return ret;
  }
  else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return 0;
	
  return SearchTreeNode::insertRoom(event);
}
	

RoomSearchNode * IntermediateNode::getRooms(ParseEvent * event) {
  RoomSearchNode * ret = 0;
  if (event->next() == 0) {
    ret = rcmm.activate();
    ret->merge(rooms);
    return ret;
  }
  else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return SearchTreeNode::skipDown(event);
  else {
    ret = SearchTreeNode::getRooms(event);
    if (ret == (SearchTreeNode *)this) event->prev();
    return ret;
  }
}

RoomSearchNode * IntermediateNode::skipDown(ParseEvent * event) {
  bool many = false;
  ParseEvent * copy = 0;

  if (event->current()->size() == SKIPPED_MANY) {
    many = true;
    copy = event->copy();
  }
  else many = false; 
  RoomCollection * r = getRooms(event);

  if (many) {
    RoomCollection * temp = 0;
    r->merge(rooms);
    temp = SearchTreeNode::skipDown(copy);
    r->merge(temp);
    rcmm.deactivate(temp);
    pemm.deactivate(copy);
  }

  return r;
}
