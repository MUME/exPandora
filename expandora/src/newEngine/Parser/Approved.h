#ifndef APPROVED_H
#define APPROVED_H

#include <qobject.h>
#include "Room.h"
#include "ParserEvent.h"


class Approved : public QObject{
 private:
   Q_OBJECT
   Room * matchedRoom;
   bool moreThanOne;
   int matchingTolerance;
   ParseEvent * myEvent;
   QObject * owner;

 public slots:
   void foundRoom(QObject *, Room *);

 signals:
   void releaseRoom(int);

 public:
   Approved(ParseEvent * event, int tolerance);
   Room * oneMatch();
   QObject * getOwner(); 
   void reset();
}
