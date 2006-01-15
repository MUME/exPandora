#ifndef PARSER
#define PARSER
#include <queue>
#include <list>
#include <stack>
#include <qobject.h>
#include <qmutex.h>
#include <qthread.h>
#include <QWaitCondition>
#include <qmap.h>

#include "ParseEvent.h"
#include "Property.h"
#include "Path.h"
#include "Room.h"
#include "Coordinate.h"
#include "Experimenting.h"
#include "Approved.h"
#include "Syncing.h"
#include "Component.h"
#include "RoomCollection.h"

#define APPROVED 0
#define EXPERIMENTING 1
#define SYNCING 2


/**
 * the parser determines the relations between incoming move- and room-events
 * and decides if rooms have to be added (and where) and where the player is
 * the results are published via signals
 */
class Parser : public Component {
Q_OBJECT
 public slots:
  void event(ParseEvent *);
  /* the setTerrain signal should be replaced by some "hint" in the properties */
  void setTerrain(Property *);

 signals:
  // the QObjects in the signals denote the recipient of the results
  // the map shoud keep track of them and only deliver results to 
  // the interested objects
  void lookingForRooms(RoomRecipient *,ParseEvent *);
  void lookingForRooms(RoomRecipient *,unsigned int);
  void lookingForRooms(RoomRecipient *,Coordinate *);
  void playerMoved(Coordinate *, Coordinate *);
  void createRoom(ParseEvent *, Coordinate *, char);
  void addExit(int, int, int);
  

 public:
  Parser();
  Coordinate * getExpectedCoordinate(Room * base);
  virtual Qt::ConnectionType requiredConnectionType(const QString &);
  
  
 private:
  void mudPop();
  void playerPop();
  void checkQueues();
  
  void dropNote(ParseEvent * ev);
  void experimenting();
  void syncing();
  void approved();
  void evaluatePaths();

  char state;
  int matchingTolerance;
  int remoteMapDelay;
  queue<ParseEvent *> playerEvents;
  queue<ParseEvent *> mudEvents;
  char activeTerrain;
  list<Path *> * paths;
  Room * mostLikelyRoom;
  
  


};	

#endif

#ifdef DMALLOC
#include <mpatrol.h>
#endif
