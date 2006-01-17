#ifndef ROOM_CLASS
#define ROOM_CLASS

#include <set>
#include "Coordinate.h"
#include "ParseEvent.h"
#include "Property.h"

#include "ObjectRecycler.h"
#include "TinyList.h"

using namespace std;



class Room
{
public:
  Room();
  ~Room();
  void clear();

  void setTerrain(char t) {terrain = t;}
  char getTerrain() {return terrain;}

  void addExit(int direction, int destination);
  void addReverseExit(int direction, int source);
  bool isNew(); // room is new if no reverse exits are present
  set<int> * go(BaseEvent * event);
  set<int> * getNeighbours(int k) {return exits.get(k);}
  set<int> * getReverseNeighbours(int k) {return reverseExits.get(k);}
  unsigned int numExits() {return exits.size();}
  unsigned int numReverseExits() {return reverseExits.size();}

  void setUnique(){unique = true;};
  bool isUnique(){return unique;};

  void addOptional(Property * note) {optionalProperties.put(optionalProperties.size(), note->copyString());}
  bool containsOptionals(TinyList<Property *> * optionals);
  void init(ParseEvent * event);
  bool fastCompare(ParseEvent * props, int tolerance);

  void setId(int in_id) {id = in_id;};
  int getId() {return id;}

  void setCoordinate(Coordinate * in_c) {c = in_c;};
  Coordinate * getCoordinate(){return c;};

private:
  TinyList<SimpleString *> properties;		/* name, desc, exit names - properties we need for tree searching */
  TinyList<SimpleString *> important;		/* pointers to the parts of the properties defining important things
  						   the user wants to know each time she enters */
  TinyList<SimpleString *> optionalProperties;	/* secret exit names for example - properties we can match if they are present */

  TinyList<set<int> *> exits;	 	        
  TinyList<set<int> *> reverseExits;
  bool unique;
  Coordinate * c;		/* coordinates on our map */
  int id; 			/* identifier */
  char terrain; 		        /* terrain type */
};

extern ObjectRecycler<Room> rmm;


#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
