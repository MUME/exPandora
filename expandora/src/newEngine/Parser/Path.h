#ifndef PATH
#define PATH

#include "Room.h"
#include <set>
#incldue <qobject.h>

class PathSignalHandler : public QObject {
 private:
  Q_OBJECT
  map<Room *, QObject *> owners;
  map<Room *, int> holdCount;

 public:
  hold(Room * room, QObject * owner);
  release(Room * room);
  forget(Room * room);

 signals:
  holdRoom(int);
  releaseRoom(int);
}

class Path {
 public:
  Path() {active = false;}
  void removeChild(Path * p);
  void setParent(Path * p);
  bool hasChildren() {return (!children.empty());};
  void init(Room * room, QObject * owner);
  Room * getRoom() {return room;};
  Path * fork(Room * room, Coordinate * expectedCoordinate, QObject * owner); //new Path is fetched from pamm, distance between rooms is calculated and probability is updated accordingly
  double getProb() {return probability;};	
  void approve();
  void deny(); 	// removes this path and all parents up to the next branch 
  // and gives them back to the pamm 
  // and removes the respective rooms if experimental
  void clear();
  void setProb(double p) {probability = p;};
  //void cutDeadBranch(); 	// removes this path and recursively all chldren 
  // and gives them back to the pamm
  // and removes all respective experimental rooms
 private:
  static PathSignalHandler signaler;
  bool active;
  Path * parent;
  set<Path *> children;
  double probability;
  Room * room; // in fact a path only has one room, one parent and soem children(forks).
};

extern ObjectRecycler<Path> pamm;

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
