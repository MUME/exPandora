#ifndef SEARCHTREENODE
#define SEARCHTREENODE
#include "TinyList.h"
#include "RoomSearchNode.h"
#include "Room.h"
#include <vector>
using namespace std;

/**
 * keeps a substring of the properties, and a table of other RoomSearchNodes pointing to the possible following characters
 */

class SearchTreeNode : public RoomSearchNode {
	protected:
		TinyList * children; 
		//RoomSearchNode * parent;
		char * myChars;
		//int start; // position of the first character of myChars
		//int next; // position of the first character after myChars
	public:
		SearchTreeNode(ParseEvent * event, TinyList * children = new TinyList());
		SearchTreeNode(char * string, TinyList * children = new TinyList());
		virtual ~SearchTreeNode() {}
		virtual RoomSearchNode * getRooms(ParseEvent * event);
		virtual Room * insertRoom(ParseEvent * event);
		virtual void setChild(char, RoomSearchNode *);
		virtual RoomSearchNode * skipDown(ParseEvent * event);
};
#endif
