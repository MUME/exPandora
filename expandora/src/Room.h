#ifndef ROOM
#define ROOM 
#include "Terrain.h"
#include <vector.h>
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
		Room(char * _properties, int _numProperties);
		void setId(int _id) {id = _id;};
		int containsOptionals(char ** optionals, int num);
		int fastCompare(char ** impProps, int num, int tolerance);
	private:
		vector<char *> properties;		/* name, desc, exit names - properties we need for tree searching */
		vector<char *> doors;			/* pointers to the parts of the properties defining exit names, by convention the last ones */
		vector<char *> optionalProperties;	/* secret exit names for example - properties we can match if they are present */
		int id; 			/* identifier */
  		char * note; 			/* note, if needed, additional info etc */
  		Terrain * terrain; 		/* terrain type */ 		
  
  		vector<Exit> exits;	 

  		long timestamp; 		/*last modification */
  
  		int x, y, z;			/* coordinates on our map */

};

#endif
