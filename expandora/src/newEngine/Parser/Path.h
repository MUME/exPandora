#ifndef PATH
#define PATH

#include "RoomSignalHandler.h"
#include "Room.h"



class Path {

 public:
  Path();  
  void removeChild(Path * p);
  void setParent(Path * p);
  bool hasChildren() {return (!children.empty());};
  void init(Room * room, QObject * owner, QObject * locker);
  Room * getRoom() {return room;};
  Path * fork(Room * room, Coordinate * expectedCoordinate, QObject * owner, double pathAcceptance, QObject * locker); 
  //new Path is fetched from pamm, distance between rooms is calculated and probability is updated accordingly
  double getProb() {return probability;};	
  void approve();
  void deny(); 	// removes this path and all parents up to the next branch 
  // and gives them back to the pamm 
  // and removes the respective rooms if experimental
  void clear();
  void setProb(double p) {probability = p;};

 private:
  static RoomSignalHandler * signaler;
  bool active;
  Path * parent;
  set<Path *> children;
  double probability;
  Room * room; // in fact a path only has one room, one parent and soem children(forks).
};

extern ObjectRecycler<Path> pamm;

#endif

#ifdef DMALLOC
#include <mpatrol.h>
#endif
