#include "IntermediateNode.h"


IntermediateNode::IntermediateNode(ParseEvent * event) : SearchTreeNode(event->next().rest()) {
	rooms = 0;
}

Room * IntermediateNode::insertRoom(ParseEvent * event) {
	
	if (event->next() == NO_PROPERTY) {
		if (rooms == 0) rooms = rcmm.activate();
		return rooms->insertRoom(event);
	}
	
	return SearchTreeNode::insertRoom(event);
}
	

RoomCollection * IntermediateNode::getRooms(ParseEvent * event) {
	if (event->next() == NO_PROPERTY) {
		RoomCollection * ret = rcmm.activate();
		ret->merge(rooms);
		return ret;
	}
	else if (event->current().size() == SKIPPED_ONE || event->current().size() == SKIPPED_MANY) return SearchTreeNode::skipDown(event);
	else return SearchTreeNode::getRooms(event);
}

RoomCollection * IntermediateNode::skipDown(ParseEvent * event) {
	RoomCollection * r = rcmm.activate();
	if (event->current().size() == SKIPPED_ONE) return getRooms(event);
	
	ParseEvent * copy = new ParseEvent(event);
	
	if (event->next() == NO_PROPERTY) r->merge(rooms);
	else {
		char c;
		if (children->get(c = event->current().next()) != 0)  r->merge(children->get(c)->getRooms(event));
	}

	r->merge(SearchTreeNode::skipDown(copy));

	delete(copy);
	return r;
}
