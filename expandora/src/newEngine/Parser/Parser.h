#ifndef PARSER
#define PARSER
#include <queue>
#include <list>
#include <stack>
#include <qobject.h>
#include <qmutex.h>
#include <qthread.h>
#include <QWaitCondition>

#include "ParseEvent.h"
#include "Property.h"
#include "Path.h"
#include "Room.h"
#include "Coordinate.h"
#include "Experimenting.h"
#include "Approved.h"
#include "Syncing.h"
#include "Component.h"

#define APPROVED 0
#define EXPERIMENTING 1
#define SYNCING 2


/**
 * the parser determines the relations between incoming move- and room-events
 * and decides if rooms have to be added (and where) and where the player is
 * the results are published via signals
 */
class Parser : public Component {
 public slots:
  void event(ParseEvent *);
  /* the setTerrain signal should be replaced by some "hint" in the properties */
  void setTerrain(Property *);

 signals:
  // the QObjects in the signals denote the recipient of the results
  // the map shoud keep track of them and only deliver results to 
  // the interested objects
  void lookingForRooms(QObject *,ParseEvent *);
  void lookingForRooms(QObject *,int);
  void lookingForRooms(QObject *,Coordinate *);
  void playerMoved(Coordinate *, Coordinate *);
  void createRoom(ParseEvent *, Coordinate *, int);
  void addExit(int, int, int);

 public:
  Parser();
  Coordinate * getExpectedCoordinate(Room * base);
  virtual Qt::ConnectionType requiredConnectionType(const char *);
  
 private:
  Q_OBJECT
  void mudPop();
  void playerPop();
  void checkQueues();
		
  void dropNote(ParseEvent * ev);
  void experimenting();
  void syncing();
  void approved();
  void evaluatePaths();

  //QWaitCondition parserSync;
  //QMutex parserMutex;
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
