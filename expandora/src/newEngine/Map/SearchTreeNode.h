#ifndef SEARCHTREENODE
#define SEARCHTREENODE
#include <vector>
#include <iostream>
#include "Room.h"
#include "TinyList.h"
#include "RoomSearchNode.h"


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
		SearchTreeNode(char * string = 0, TinyList<RoomSearchNode *> * children = 0);
		virtual ~SearchTreeNode() {cerr << "warning: SearchTreeNode destroyed"<< endl;}
		virtual AbstractRoomContainer * getRooms(ParseEvent * event);
		virtual RoomCollection * insertRoom(ParseEvent * event);
		
		virtual void setChild(char, RoomSearchNode *);
		virtual AbstractRoomContainer * skipDown(ParseEvent * event);
};


#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
