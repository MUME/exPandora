#ifndef PATH
#define PATH

#include "RoomSignalHandler.h"
#include "Room.h"
#include "RoomRecipient.h"
#include "RoomAdmin.h"
#include "PathParameters.h"

class Path {

 public:
  Path();  
  void removeChild(Path * p);
  void setParent(Path * p);
  bool hasChildren() {return (!children.empty());};
  void init(Room * room, RoomAdmin * owner, RoomRecipient * locker, RoomSignalHandler * signaler);
  Room * getRoom() {return room;};
  Path * fork(Room * room, Coordinate * expectedCoordinate, RoomAdmin * owner, PathParameters params, RoomRecipient * locker); 
  //new Path is fetched from pamm, distance between rooms is calculated and probability is updated accordingly
  double getProb() {return probability;};	
  void approve();
  void deny(); 	// removes this path and all parents up to the next branch 
  // and gives them back to the pamm 
  // and removes the respective rooms if experimental
  void clear();
  void setProb(double p) {probability = p;};
  ~Path() {clear();}
  
 private:
  bool active;
  Path * parent;
  set<Path *> children;
  double probability;
  Room * room; // in fact a path only has one room, one parent and soem children(forks).
  RoomSignalHandler * signaler;
};

extern ObjectRecycler<Path> pamm;

#endif

#ifdef DMALLOC
#include <mpatrol.h>
#endif
