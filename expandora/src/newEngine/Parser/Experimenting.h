#ifndef EXPERIMENTING_H
#define EXPERIMENTING_H

#include <qobject.h>
#include <qmutex.h>
#include "Room.h"
#include "Path.h"

class Parser;

class Experimenting : public QObject {
 private:
  Q_OBJECT
  list<Path *> * paths;
  list<Path *> * shortPaths;
  double prevBest;
  Path * best;
  Path * second;
  double pathAcceptance;
  Parser * parent;
  QMutex deleteLock;

 public:
  Experimenting(Parser * parent, list<Path *> * paths, double pa);
  list<Path *> * evaluate();
  
 public slots:
  void receiveRoom(QObject *, Room *);

 signals:
  void releaseRoom(int);
};

#endif

