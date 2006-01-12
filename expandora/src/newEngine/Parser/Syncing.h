#ifndef SYNCING_H
#define SYNCING_H

#include <list>
#include "Path.h"

class Syncing : public QObject {
 private:
   Q_OBJECT
   list<Path *> * paths;
   
 public slots:
   void receiveRoom(QObject *, Room *);

 public:
   Syncing(list<Path *> * in);
   list<Path *> * evaluate();
};
#ifdef DMALLOC
#include <dmalloc.h>
#endif
#endif
   
