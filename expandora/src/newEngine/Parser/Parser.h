#ifndef PARSER
#define PARSER
#include <queue>
#include <list>
#include <stack>
#include <qobject.h>
#include <qmutex.h>
#include "ParseEvent.h"
#include "Property.h"
#include "Path.h"
#include "Room.h"
#include "Coordinate.h"


#define APPROVED 0
#define EXPERIMENTING 1
#define SYNCING 2
#define DANGLING_APPROVED 3
#define INSERTING 4

class Parser : public QObject{
 public slots:
  void event(ParseEvent *);
  void setTerrain(Property *);
  void match(Room *);
  void newRoom(Room *);

 signals:
  void lookingForRooms(ParseEvent *);
  void lookingForRooms(int);
  void lookingForRooms(Coordinate *);
  void playerMoved(Coordinate *, Coordinate *);
  void newSimilarRoom(ParseEvent *, Coordinate *, int);


 public:
  Parser();


  Room * getMostLikely() {return mostLikelyRoom;}
 private:
  Q_OBJECT
  void mudPop();
  void playerPop();
  void unify();
		
  void dropNote(ParseEvent * ev);
  void checkQueues();
  void experimenting();
  void syncing();
  void approved();
  void enlargePaths(RoomCollection * enlargingRooms);
  void buildPaths(RoomCollection * initialRooms);
  Coordinate * getExpectedCoordinate(Room * base);

  // for inserting newly created rooms
  Coordinate * c;  
  list<Path *>::iterator i;
  QMutex insertLock;
  stack<Room *> releaseSchedule;
  list<Path *> * shortPaths;
  Path * best;
  Path * second;
  double prevBest;

  // others
  char state;
  int matchingTolerance;
  queue<ParseEvent *> playerEvents;
  queue<ParseEvent *> mudEvents;
  int activeTerrain;
  list<Path *> * paths;
  Room * mostLikelyRoom;
  double pathAcceptance;

};	

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
