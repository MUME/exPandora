#ifndef ROOM_CLASS
#define ROOM_CLASS
#include "ParseEvent.h"
#include "Property.h"
#include "Terrain.h"
#include <list>
#include <vector>
using namespace std;


class Exit;
class RoomCollection;

class Room {
	public:
		bool experimental;
		RoomCollection * go(BaseEvent * event);
		Room();
		void init(ParseEvent * event);
		void setId(int _id) {id = _id;};
		int containsOptionals(list<Property *> & optionals);
		int fastCompare(list<Property *> & props, int tolerance);
		void clear();
	private:
		RoomCollection * home;
		list<Property *> properties;		/* name, desc, exit names - properties we need for tree searching */
		list<Property *> important;		/* pointers to the parts of the properties defining important things 
							   the user wants to know each time she enters */
		list<Property *> optionalProperties;	/* secret exit names for example - properties we can match if they are present */
		int id; 			/* identifier */
  		Terrain * terrain; 		/* terrain type */ 		
  
  		vector<Exit *> exits;	 	/* we only match the defined standard exits and keep one field for "weird" exits */						   
  		long timestamp; 		/*last modification */
  
  		int x, y, z;			/* coordinates on our map */

};

extern ObjectRecycler<Room> rmm;
#endif
