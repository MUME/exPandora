#include "IntermediateNode.h"


IntermediateNode::IntermediateNode(char * _myChars, RoomSearchNode * _parent) : SearchTreeNode(_myChars, parent, 0) {
	rooms = 0;
}

Room * IntermediateNode::insertRoom(char ** properties, int pos, int numProperties) {
	pos++;
	if (pos == numProperties) {
		if (rooms == 0) rooms = new RoomCollection();
		return rooms->insertRoom(properties, numProperties);
	}
	char * othersChars;
	for (int i = start; i < next; i++) {
		if (myChars[i-start] != properties[pos][i]) {
			othersChars = (char *)malloc(next - i + 1);
			char * myNewChars = (char *)malloc(i + 1);
			strncpy(othersChars, myChars+i, next-i);
			strncpy(myNewChars, myChars, i); 
			delete(myChars);
			myChars = myNewChars;
			selectedChild = new SearchTreeNode(othersChars, this, i);
			for (int c = 0; c < 128; c++) {
				selectedChild->setChild(c, get(c));
				put(c, 0);
			}
			put(properties[pos][i], selectedChild);
			next = i;
			return selectedChild->insertRoom(properties, pos, numProperties);
		}
	}

	return SearchTreeNode::insertMatchingRoom(properties, pos, numProperties);
		
}




RoomCollection * IntermediateNode::getRooms(char ** properties, int pos, int numProperties) {
	pos++;
	if (pos == numProperties) return rooms;
	else return SearchTreeNode::getRooms(properties, pos, numProperties);
}

