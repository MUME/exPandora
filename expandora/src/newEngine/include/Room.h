#ifndef ROOM_CLASS
#define ROOM_CLASS

#include <set>
#include <QMutex>
#include <QMutexLocker>
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

  void addExit(uint direction, int destination);
  void addReverseExit(uint direction, int source);
  bool isNew(); // room is new if no reverse exits are present
  set<int> * go(BaseEvent * event);
  set<int> getNeighbours(uint k);
  set<int> getReverseNeighbours(uint k);
  uint numExits() {QMutexLocker locker(&roomMutex);return exits.size();}
  uint numReverseExits() {QMutexLocker locker(&roomMutex);return reverseExits.size();}

  void setUnique(){unique = true;};
  bool isUnique(){return unique;};

  void addOptional(Property * note) {QMutexLocker locker(&roomMutex);optionalProperties.put(optionalProperties.size(), note->copyString());}
  bool containsOptionals(TinyList<Property *> * optionals);
  void init(ParseEvent * event);
  bool fastCompare(ParseEvent * props, int tolerance);

  void setId(int in_id) {id = in_id;};
  int getId() {return id;}

  void setCoordinate(Coordinate in_c) {c = in_c;};
  Coordinate getCoordinate(){return c;};

private:
  TinyList<SimpleString *> properties;		/* name, desc, exit names - properties we need for tree searching */
  TinyList<SimpleString *> important;		/* pointers to the parts of the properties defining important things
  						   the user wants to know each time she enters */
  TinyList<SimpleString *> optionalProperties;	/* secret exit names for example - properties we can match if they are present */

  vector<set<int> > exits;	 	        
  vector<set<int> > reverseExits;
  QMutex roomMutex;
  bool unique;
  Coordinate c;		/* coordinates on our map */
  int id; 			/* identifier */
  char terrain; 		        /* terrain type */
};

extern ObjectRecycler<Room> rmm;


#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
