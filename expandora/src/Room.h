#ifndef ROOM_CLASS
#define ROOM_CLASS
#include "ParseEvent.h"
#include "Property.h"
#include "Terrain.h"
#include "Coordinate.h"
#include "ObjectRecycler.h"
#include "TinyList.h"

using namespace std;

class RoomAdmin;
class RoomCollection;
extern int defaultTolerance;

class Room {
 public:
  void setTerrain(Terrain * t) {terrain = t;}
  Terrain * getTerrain() {return terrain;}
  void hold() {holdCount++;}
  void release(RoomAdmin * admin);
  void addExit(int direction, Room * destination);	
  RoomCollection * go(BaseEvent * event);
  RoomCollection * getNeighbours(int k); 
  Room();
  void setUnique(){unique = true;};
  bool isUnique(){return unique;};
  void addOptional(Property * note) {optionalProperties.put(optionalProperties.size(), note->copyString());}
  void init(ParseEvent * event, RoomCollection * _home);
  void setId(int _id) {id = _id;};
  bool containsOptionals(TinyList<Property *> * optionals);
  bool fastCompare(ParseEvent * props, int tolerance = defaultTolerance);
  void clear();
  void setCoordinate(Coordinate * _c) {c = _c;};
  Coordinate * getCoordinate(){return c;};
  RoomCollection * getHome() {return home;};
  void resetTime(double ts) {timestamp = ts;}
 private:
  RoomCollection * home;
  TinyList<SimpleString *> properties;		/* name, desc, exit names - properties we need for tree searching */
  TinyList<SimpleString *> important;		/* pointers to the parts of the properties defining important things 
					   the user wants to know each time she enters */
  TinyList<SimpleString *> optionalProperties;	/* secret exit names for example - properties we can match if they are present */
  
  TinyList<RoomCollection *> exits;	 	/* we only match the defined standard exits and keep one field for "weird" exits */						   
  int holdCount;
  bool unique;
  double timestamp; 		/*last modification */
  Coordinate * c;			/* coordinates on our map */
  int id; 			/* identifier */
  Terrain * terrain; 		/* terrain type */ 		

};

extern ObjectRecycler<Room> rmm;
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
