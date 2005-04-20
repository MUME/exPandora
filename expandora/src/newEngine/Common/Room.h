#ifndef ROOM_CLASS
#define ROOM_CLASS

#include <set>
#include "ParseEvent.h"
#include "Property.h"
#include "Coordinate.h"
#include "ObjectRecycler.h"
#include "TinyList.h"

using namespace std;


class RoomCollection;


class Room {
 public:
  void setTerrain(int t) {terrain = t;}
  int getTerrain() {return terrain;}
  void addExit(int direction, int destination);	
  void addReverseExit(int direction, int source);
  set<int> * go(BaseEvent * event);
  set<int> * getNeighbours(int k) {return exits.get(k);} 
  unsigned int numExits() {return exits.size();}
  Room();
  void setUnique(){unique = true;};
  bool isUnique(){return unique;};
  void addOptional(Property * note) {optionalProperties.put(optionalProperties.size(), note->copyString());}
  void init(ParseEvent * event);
  void setId(int in_id) {id = in_id;};
  int getId() {return id;}
  bool containsOptionals(TinyList<Property *> * optionals);
  bool fastCompare(ParseEvent * props, int tolerance);
  void clear();
  void setCoordinate(Coordinate * in_c) {c = in_c;};
  Coordinate * getCoordinate(){return c;};
  void resetTime(double ts) {timestamp = ts;}

 private:
  TinyList<SimpleString *> properties;		/* name, desc, exit names - properties we need for tree searching */
  TinyList<SimpleString *> important;		/* pointers to the parts of the properties defining important things 
						   the user wants to know each time she enters */
  TinyList<SimpleString *> optionalProperties;	/* secret exit names for example - properties we can match if they are present */
  
  TinyList<set<int> *> exits;	 	        /* we only match the defined standard exits and keep one field for "weird" exits */						   
  TinyList<set<int> *> reverseExits;
  bool unique;
  double timestamp; 		/* last modification */
  Coordinate * c;		/* coordinates on our map */
  int id; 			/* identifier */
  int terrain; 		        /* terrain type */ 		
};

extern ObjectRecycler<Room> rmm;
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
