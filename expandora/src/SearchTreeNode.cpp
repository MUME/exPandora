#include "SearchTreeNode.h"
#include "IntermediateNode.h"



SearchTreeNode::SearchTreeNode(ParseEvent * event, TinyList * _children) {
	myChars = event->current().rest();
	children = _children;
}


SearchTreeNode::SearchTreeNode(char * _myChars, TinyList * _children) {
	myChars = _myChars; 
	children = _children;
}

/**
 */
RoomCollection * SearchTreeNode::getRooms(ParseEvent * event) {
	RoomSearchNode * selectedChild = 0;
	for (int i = 0; myChars[i] != 0; i++) if (event->current().next() != myChars[i]) return 0;
	selectedChild = children->get(event->current().next());
	
	if(selectedChild == 0) return 0; // no such room
	else return selectedChild->getRooms(event);	// the last character of name is 0, 
							// at position 0 there is a roomCollection, if we have rooms here
							// else there is 0, so name[depth] should work.
}


void SearchTreeNode::setChild(char position, RoomSearchNode * node) {
	children->put(position, node);
}


/**
 */
Room * SearchTreeNode::insertRoom(ParseEvent * event) {
	RoomSearchNode * selectedChild = 0;
	char c;
	for (int i = 0; myChars[i] != 0; i++) if (c = event->current().next() != myChars[i]) {
		
		// we have to split, as we encountered a difference in the strings ...	
		
		selectedChild = new SearchTreeNode(myChars + i + 1, children);	// first build the lower part of this node	
		children = new TinyList();	// and update the upper part of this node
		children->put(myChars[i], selectedChild);
		
		if (c == 0) selectedChild = new IntermediateNode(event);	// then build the branch
		else selectedChild = new SearchTreeNode(event);
			
		children->put(c, selectedChild); // and update again
		myChars[i] = 0; // the string is separated in [myChars][0][selectedChildChars][0] so we don't have to copy anything ... 
		
		return selectedChild->insertRoom(event);
	}
	
	// we reached the end of our string and can pass the event to the next node (or create it)
	selectedChild = children->get(c = event->current().next());
	if (selectedChild != 0) return selectedChild->insertRoom(event);
	else {
	       	if (c != 0) selectedChild = new SearchTreeNode(event);
		else selectedChild = new IntermediateNode(event);
		children->put(c, selectedChild);
	}
	selectedChild->insertRoom(event);
}
			
/**
 * checking if another property needs to be skipped is done in the intermediate nodes
 */
RoomCollection * SearchTreeNode::skipDown(ParseEvent * event) {
	RoomSearchNode * selectedChild = 0;
	RoomCollection * ret = rcmm.activate();
	RoomCollection * add;
	for (int i = 0; i < 128; i++) {
		if ((selectedChild = children->get(i)) != 0)
			if ((add = selectedChild->skipDown(event)) != 0) {
				ret->merge(add);
				rcmm.deactivate(add);
			}
	}
	return ret;
}
		


