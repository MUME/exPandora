#include "SearchTreeNode.h"
#include "IntermediateNode.h"
#include <string>
using namespace std;


SearchTreeNode::SearchTreeNode(ParseEvent * event, TinyList<RoomSearchNode *> * in_children) {
  
  myChars = new char[strlen(event->current()->rest())];
  strcpy(myChars, event->current()->rest()+1); 	// we copy the string so that we can remove rooms independetly of tree nodes
  if (in_children == 0) in_children = new TinyList<RoomSearchNode *>();
  children = in_children;
}


SearchTreeNode::SearchTreeNode(char * in_myChars, TinyList<RoomSearchNode *> * in_children) {
  myChars = in_myChars; 
  if (in_children == 0) in_children = new TinyList<RoomSearchNode *>();
  children = in_children;
}

/**
 */
RoomSearchNode * SearchTreeNode::getRooms(ParseEvent * event) {
  RoomSearchNode * selectedChild = 0;
  for (int i = 0; myChars[i] != 0; i++) {
    if (event->current()->next() != myChars[i]) {
      for(; i > 0 ; i--) event->current()->prev();
      return this;
    }
  }
  selectedChild = children->get(event->current()->next());

  if(selectedChild == 0) {
    for (int i = 1; i < myChars[i] != 0; i++) event->current()->prev();
    return this; // no such room
  }
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
  char c = event->current()->next();
  for (int i = 0; myChars[i] != 0; i++) {
    if (c != myChars[i]) {
      // we have to split, as we encountered a difference in the strings ...	
      selectedChild = new SearchTreeNode(myChars + i + 1, children);	// first build the lower part of this node	
      children = new TinyList<RoomSearchNode *>();	// and update the upper part of this node
      children->put(myChars[i], selectedChild);
		
      if (c == 0) selectedChild = new IntermediateNode(event);	// then build the branch
      else selectedChild = new SearchTreeNode(event);
				
      children->put(c, selectedChild); // and update again
      myChars[i] = 0; // the string is separated in [myChars][0][selectedChildChars][0] so we don't have to copy anything
		
      return selectedChild->insertRoom(event);
    }
    else c = event->current()->next();
  }
	
  // we reached the end of our string and can pass the event to the next node (or create it)
  selectedChild = children->get(c);
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
  ParseEvent * copy = 0;
  for (int i = 0; i < 128; i++) {
    if ((selectedChild = children->get(i)) != 0) {
      copy = event->copy();
      add = selectedChild->skipDown(event);
      ret->merge(add);
      if (add->numRooms() > -1) rcmm.deactivate((RoomCollection *)add);
      pemm.deactivate(copy);
      copy = 0;
    }
  }
  return ret;
}
		


