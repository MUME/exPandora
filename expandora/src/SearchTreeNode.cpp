#include "SearchTreeNode.h"
#include "IntermediateNode.h"
#include <string>
using namespace std;


SearchTreeNode::SearchTreeNode(ParseEvent * event, TinyList<RoomSearchNode> * _children) {
	myChars = new char[strlen(event->current()->rest())];
	strcpy(myChars, event->current()->rest()); 	// we copy the string so that we can remove rooms independetly of tree nodes
							// of courxe that needs memory
	children = _children;
}


SearchTreeNode::SearchTreeNode(char * _myChars, TinyList<RoomSearchNode> * _children) {
	myChars = _myChars; 
	children = _children;
}

/**
 */
RoomSearchNode * SearchTreeNode::getRooms(ParseEvent * event) {
	RoomSearchNode * selectedChild = 0;
	for (int i = 0; myChars[i] != 0; i++) if (event->current()->next() != myChars[i]) return this;
	selectedChild = children->get(event->current()->next());
	
	if(selectedChild == 0) return this; // no such room
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
	char c = event->current()->current();
	for (int i = 0; myChars[i] != 0; i++) {
		if (c != myChars[i]) {
			//printf ("no match: \n%s\n%s\n", myChars + i, event->current()->rest());
			// we have to split, as we encountered a difference in the strings ...	
			selectedChild = new SearchTreeNode(myChars + i + 1, children);	// first build the lower part of this node	
			//printf("string transferred to lower node: %s\n", myChars + i + 1);
			children = new TinyList<RoomSearchNode>();	// and update the upper part of this node
			children->put(myChars[i], selectedChild);
		
			if (c == 0) selectedChild = new IntermediateNode(event);	// then build the branch
			else {
				event->current()->next();
				selectedChild = new SearchTreeNode(event);
			}
				
			children->put(c, selectedChild); // and update again
			myChars[i] = 0; // the string is separated in [myChars][0][selectedChildChars][0] so we don't have to copy anything ... 
		
			return selectedChild->insertRoom(event);
		}
		else c = event->current()->next();
	}
	
	// we reached the end of our string and can pass the event to the next node (or create it)
	selectedChild = children->get(c);
	event->current()->next();
	if (selectedChild == 0) { 
	       	if (c != 0) selectedChild = new SearchTreeNode(event);
		else selectedChild = new IntermediateNode(event);
		children->put(c, selectedChild);
	}
	return selectedChild->insertRoom(event);
}
			
/**
 * checking if another property needs to be skipped is done in the intermediate nodes
 */
RoomSearchNode * SearchTreeNode::skipDown(ParseEvent * event) {
	RoomSearchNode * selectedChild = 0;
	RoomSearchNode * ret = rcmm.activate();
	RoomSearchNode * add;
	ParseEvent * copy = pemm.activate();
	for (int i = 0; i < 128; i++) {
		if ((selectedChild = children->get(i)) != 0)
			copy->copy(event);
			if ((add = selectedChild->skipDown(event)) != 0) {
				ret->merge(add);
				if (add->numRooms() > -1) rcmm.deactivate((RoomCollection *)add);
			}
			copy->recycleProperties();
			copy->clear();
	}
	pemm.deactivate(copy);
	return ret;
}
		


