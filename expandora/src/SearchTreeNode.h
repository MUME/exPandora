#ifndef SEARCHTREENODE
#define SEARCHTREENODE
#include "TinyList.h"
#include "RoomSearchNode.h"
#include "RoomCollection.h"
#include "Room.h"
/**
 * keeps a substring of the properties, and a table of other RoomSearchNodes pointing to the possible following characters
 */

class SearchTreeNode : public RoomSearchNode, protected TinyList {
	protected:
		RoomSearchNode * parent;
		RoomSearchNode * selectedChild;
		char * myChars;
		int start; // position of the first character of myChars
		int next; // position of the first character after myChars
	public:
		SearchTreeNode(char * _myChars, RoomSearchNode * _parent, int _start);
		~SearchTreeNode();
		virtual RoomCollection * getRooms(char ** properties, int pos, int numProperties);
		virtual Room * insertRoom(char ** properties, int pos, int numProperties);
		Room * insertMatchingRoom(char ** properties, int pos, int numProperties);
		void setChild(char, RoomSearchNode *);
};
#endif
