#include "IntermediateNode.h"


IntermediateNode::IntermediateNode(ParseEvent * event) : SearchTreeNode((char *)0,(TinyList *)0) {
 	Property * prop = event->next();
 	if (prop != 0) myChars = prop->rest();
	else myChars = "";
	rooms = 0;
	event->prev();
}

Room * IntermediateNode::insertRoom(ParseEvent * event) {
	
	if (event->next() == 0) {
		if (rooms == 0) rooms = rcmm.activate();
		return rooms->insertRoom(event);
	}
	
	return SearchTreeNode::insertRoom(event);
}
	

RoomSearchNode * IntermediateNode::getRooms(ParseEvent * event) {
	if (event->next() == 0) {
		RoomSearchNode * ret = rcmm.activate();
		ret->merge(rooms);
		return ret;
	}
	else if (event->current()->size() == SKIPPED_ONE || event->current()->size() == SKIPPED_MANY) return SearchTreeNode::skipDown(event);
	else return SearchTreeNode::getRooms(event);
}

RoomSearchNode * IntermediateNode::skipDown(ParseEvent * event) {
	RoomSearchNode * r = rcmm.activate();
	if (event->current()->size() == SKIPPED_ONE) return getRooms(event);
	
	
	if (event->next() == 0) r->merge(rooms);
	else {
		ParseEvent * copy = pemm.activate();
		copy->copy(event);
		char c = 0;
		if ((myChars[0] == 0 && children->get(c = event->current()->current()) != 0) || myChars[0] == c)  r->merge(SearchTreeNode::getRooms(event));
		r->merge(SearchTreeNode::skipDown(copy));
		copy->recycleProperties();
		pemm.deactivate(copy);
	}

	return r;
}
