#include "SearchTreeNode.h"
#include "IntermediateNode.h"

/**
 * finds all rooms matching the string name
 */
RoomCollection * SearchTreeNode::getRooms(vector<char *> & properties, int pos) {
	if (strcmp(properties[pos]+start, myChars)) return 0;
	
	selectedChild = get(properties[pos][next]);
	if(selectedChild == 0) return 0; // no such room
	else return selectedChild->getRooms(properties, pos);	// the last character of name is 0, 
							// at position 0 there is a roomCollection, if we have rooms here
							// else there is 0, so name[depth] should work.
}


void SearchTreeNode::setChild(char position, RoomSearchNode * node) {
	put(position, node);
}

/**
 */
Room * SearchTreeNode::insertRoom(vector<char *> & properties, int pos) {
	char * othersChars;
	for (int i = start; i < next; i++) {
		if (myChars[i-start] != properties[pos][i]) {
			othersChars = (char *)malloc(i - start + 1);
			char * myNewChars = (char *)malloc(next - i + 1);
			strncpy(othersChars, myChars, i-start);
			strncpy(myNewChars, myChars+i-start, next-i); 
			delete(myChars);
			myChars = myNewChars;
			parent = new SearchTreeNode(othersChars, parent, start);
			parent->setChild(myChars[0], this);
			start = i;
			return parent->insertRoom(properties, pos);
		}
	}
	// if we got here, our string matched the properties
	return insertMatchingRoom(properties, pos); 
}

Room * SearchTreeNode::insertMatchingRoom(vector<char *> & properties, int pos) { 
	
	char * othersChars;
	RoomSearchNode * selectedChild = get(properties[pos][next]);
	if (selectedChild == 0) {
		if (properties[pos][next] == 0) {
			if (pos < properties.size()-1) {
				othersChars = (char *)malloc(strlen(properties[pos+1])+1);
				strcpy(othersChars, properties[pos+1]);
			}
			else othersChars = "";
			selectedChild = new IntermediateNode(othersChars, this);
			put(0, selectedChild);
		}
		else {
			othersChars = (char *)malloc(strlen(properties[pos]) - next + 1);
			strcpy(othersChars, properties[pos]+next);
			selectedChild = new SearchTreeNode(othersChars, this, next);
			put(othersChars[0], selectedChild);
		}
	}
	return selectedChild->insertRoom(properties, pos);
}


SearchTreeNode::SearchTreeNode(char * _myChars, RoomSearchNode * _parent, int _start) : TinyList() {
	myChars = _myChars;
	start = _start;
	next = start+strlen(myChars);
	parent = _parent;
}
