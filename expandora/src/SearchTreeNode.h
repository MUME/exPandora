#ifndef SEARCHTREENODE
#define SEARCHTREENODE
#include "TinyList.h"
#include "RoomSearchNode.h"
#include "RoomCollection.h"
#include "Room.h"
#include <vector>
using namespace std;

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
		virtual RoomCollection * getRooms(vector<char *> & properties, int pos);
		virtual Room * insertRoom(vector<char *> & properties, int pos);
		Room * insertMatchingRoom(vector<char *> & properties, int pos);
		void setChild(char, RoomSearchNode *);
};
#endif
