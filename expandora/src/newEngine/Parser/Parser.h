#ifndef PARSER
#define PARSER
#include <queue>
#include <list>
#include <qobject.h>
#include "ParseEvent.h"
#include "Property.h"
#include "Path.h"
#include "Room.h"
#include "Coordinate.h"


#define APPROVED 0
#define EXPERIMENTING 1
#define SYNCING 2


class Parser : public QObject{
 public slots:
  void setTerrain(Property *);
  void matchFound(Room *);

 signals:
  void lookingForRooms(Parser *, ParseEvent *);
  void lookingForRooms(Parser *, int);
  void lookingForRooms(Parser *, Coordinate *);
  void playerMoved(Coordinate *, Coordinate *);
  void newSimilarRoom(ParseEvent *, Coordinate *, int);

 public:
  Parser();

  void event(ParseEvent * ev);
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

  char state;
  int matchingTolerance;
  queue<ParseEvent *> playerEvents;
  queue<ParseEvent *> mudEvents;
  int activeTerrain;
  list<Path *> paths;
  Room * mostLikelyRoom;
  double pathAcceptance;

};	

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
