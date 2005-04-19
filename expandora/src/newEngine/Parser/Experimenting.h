#ifndef EXPERIMENTING_H
#define EXPERIMENTING_H

#include <qobject.h>
#include <qmutex.h>
#include "Room.h"

class Parser;

class Experimenting : public QObject {
 private:
  Q_OBJECT
  map<Room *, int> releaseSchedule;
  map<Room *, QObject *> roomOwners;
  list<Path *> * paths;
  list<Path *> * shortPaths;
  double prevBest;
  Room * best;
  Room * second;
  double pathAcceptance;
  Parser * parent;
  QMutex deleteLock;

 public:
  Experimenting(Parser * parent, list<Path *> * paths, double pa);
  list<Path *> * evaluate();
  
 public slots:
  void foundRoom(QObject *, Room *);

 signals:
  void releaseRoom(int);
}

#endif

