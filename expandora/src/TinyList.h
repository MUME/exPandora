#ifndef TINYLIST
#define TINYLIST

#include "RoomSearchNode.h"

class TinyList {
	public:
		TinyList();
		~TinyList();
		RoomSearchNode * get(char pos);
		void put(char pos, RoomSearchNode * object);
		void remove(char pos);
		
	private:
		RoomSearchNode ** list;
		char listSize;
		char listLast;
};

#endif
