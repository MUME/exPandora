#ifndef PATH
#define PATH
#include <set>
#include "Room.h"
#include "RoomAdmin.h"

using namespace std;

class Path {
 public:
  Path() {}
  void removeChild(Path * p);
  void setParent(Path * p);
  bool hasChildren() {return (!children.empty());};
  void init(Room * room, RoomAdmin * _admin);
  Room * getRoom() {return room;};
  Path * fork(Room * room, Coordinate * expectedCoordinate); //new Path is fetched from pamm, distance between rooms is calculated and probability is updated accordingly
  double getProb() {return probability;};	
  void approve();
  void deny(); 	// removes this path and all parents up to the next branch 
  // and gives them back to the pamm 
  // and removes the respective rooms if experimental
  void clear();
  void setProb(double p) {probability = p;};
  void cutDeadBranch(); 	// removes this path and recursively all chldren 
  // and gives them back to the pamm
  // and removes all respective experimental rooms
 private:
  Path * parent;
  set<Path *> children;
  double probability;
  Room * room; // in fact a path only has one room, one parent and soem children(forks).
  RoomAdmin * admin;
};

extern ObjectRecycler<Path> pamm;

#endif
