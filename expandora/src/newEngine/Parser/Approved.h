#ifndef APPROVED_H
#define APPROVED_H

#include <qobject.h>
#include "Room.h"
#include "ParseEvent.h"


class Approved : public QObject{
 private:
   Q_OBJECT
   Room * matchedRoom;
   ParseEvent * myEvent;
   int matchingTolerance;   
   QObject * owner;
   bool moreThanOne;
   
   



 public slots:
   void foundRoom(QObject *, Room *);

 signals:
   void releaseRoom(int);

 public:
   Approved(ParseEvent * event, int tolerance);
   Room * oneMatch();
   QObject * getOwner(); 
   void reset();
};
#endif
