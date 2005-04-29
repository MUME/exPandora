#ifndef SEARCHTREENODE
#define SEARCHTREENODE
#include <vector>
#include "TinyList.h"
#include "RoomSearchNode.h"
#include "Room.h"

using namespace std;

/**
 * keeps a substring of the properties, and a table of other RoomSearchNodes pointing to the possible following characters
 */

class SearchTreeNode : public RoomSearchNode {
	protected:
		TinyList<RoomSearchNode *> * children; 
		//RoomSearchNode * parent;
		char * myChars;
		//int start; // position of the first character of myChars
		//int next; // position of the first character after myChars
	public:
		SearchTreeNode(ParseEvent * event, TinyList<RoomSearchNode *> * children = 0);
		SearchTreeNode(char * string, TinyList<RoomSearchNode *> * children = 0);
		virtual ~SearchTreeNode() {printf("warning: SearchTreeNode destroyed");}
		virtual RoomSearchNode * getRooms(ParseEvent * event);
		virtual Room * insertRoom(ParseEvent * event);
		
		virtual void setChild(char, RoomSearchNode *);
		virtual RoomSearchNode * skipDown(ParseEvent * event);
};
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
