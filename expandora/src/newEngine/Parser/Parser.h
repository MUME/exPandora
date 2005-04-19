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
#include "Experimenting.h"
#include "Approved.h"
#include "Syncing.h"

#define APPROVED 0
#define EXPERIMENTING 1
#define SYNCING 2



class Parser : public QObject{
 public slots:
  void event(ParseEvent *);
  void setTerrain(Property *);

 signals:
  // the QObjects in the signals denote the recipient of the results
  // the map shoud keep track of them and only deliver results to 
  // the interested objects
  void lookingForRooms(QObject *,ParseEvent *);
  void lookingForRooms(QObject *,int);
  void lookingForRooms(QObject *,Coordinate *);
  void playerMoved(Coordinate *, Coordinate *);
  void createRoom(QObject *,ParseEvent *, Coordinate *, int);
  void addExit(int, int, char);

 public:
  Parser();
  Coordinate * getExpectedCoordinate(Room * base);

 private:
  Q_OBJECT
  void mudPop();
  void playerPop();
		
  void dropNote(ParseEvent * ev);
  void checkQueues();
  void experimenting();
  void syncing();
  void approved();
  void evaluatePaths();

  QMutex parserMutex;
  char state;
  int matchingTolerance;
  int remoteMapDelay;
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
