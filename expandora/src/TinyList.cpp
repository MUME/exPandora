
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
	list = (RoomSearchNode **)malloc(sizeof(RoomSearchNode *));
	listSize = 1;
	listLast = -1;
}

TinyList::~TinyList() {
	free(list);
}


RoomSearchNode * TinyList::get(char c) {
	if (c < 0) c+=128;
	if (c >= listLast) return 0;
	else return list[c];
}

void TinyList::put(char c, RoomSearchNode * object) {	
	if (c < 0) c+=128;
	if (c >= listSize) {
		char i;
		list = (RoomSearchNode **)realloc(list, c+1);
		listSize = c+1;
		for (i = listLast + 1; i < c; i++) list[i] = 0;
		listLast = c;
	}
	else if (c > listLast) listLast = c;
	list[c] = object;
}


void TinyList::remove(char c) {
	if (c < 0) c+=128;
	if (c < listSize) list[c] = 0;
}