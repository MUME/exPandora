
#include <stdlib.h>
#include "TinyList.h"

/**
 * extremely shrinked array list implementation to be used for each room
 * because we have so many rooms and want to search them really fast, we
 *	- allocate as little memory as possible
 *	- allow only 128 elements (1 per character)
 *	- only need 3 lines to access an element
 */



TinyList::TinyList() {
	list = new(RoomSearchNode *)[1];
	listSize = 1;
	list[0] = 0;
}

TinyList::~TinyList() {
	delete[] list;
}


RoomSearchNode * TinyList::get(char c) {
	if (c < 0) c+=128;
	if (c >= listSize) return 0;
	else return list[c];
}

void TinyList::put(char c, RoomSearchNode * object) {	
	if (c < 0) c+=128;
	if (c >= listSize) {
		char i;
		RoomSearchNode ** nlist = new (RoomSearchNode *)[c+1];
		for (i = 0; i < listSize; i++) nlist[i] = list[i];
		listSize = c+1;
		for (; i < c; i++) nlist[i] = 0;
		delete[] list;
		list = nlist;
	}
	list[c] = object;
}


void TinyList::remove(char c) {
	if (c < 0) c+=128;
	if (c < listSize) list[c] = 0;
}
