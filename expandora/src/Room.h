#ifndef ROOM_CLASS
#define ROOM_CLASS
#include "ParseEvent.h"
#include "Property.h"
#include "Terrain.h"
#include <list>
using namespace std;

// essential exits used by any mud that can be represented with a map ...
#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3
#define UP 4
#define DOWN 5
#define NONE 6

class Exit;

class Room {
	public:
		Room(ParseEvent * event);
		void setId(int _id) {id = _id;};
		int containsOptionals(list<Property *> & optionals);
		int fastCompare(list<Property *> & props, int tolerance);
	private:
		list<Property *> properties;		/* name, desc, exit names - properties we need for tree searching */
		list<Property *> important;		/* pointers to the parts of the properties defining important things 
							   the user wants to know each time she enters */
		list<Property *> optionalProperties;	/* secret exit names for example - properties we can match if they are present */
		int id; 			/* identifier */
  		char * note; 			/* note, if needed, additional info etc */
  		Terrain * terrain; 		/* terrain type */ 		
  
  		list<Exit *> exits;	 	/* in fact we don't need to know the correlation between Exits and properties: 
						   we just define an exit as soon as the player leaves through the exit.. */

  		long timestamp; 		/*last modification */
  
  		int x, y, z;			/* coordinates on our map */

};

#endif
