#ifndef APPROVED_H
#define APPROVED_H

#include <qobject.h>
#include "Parser.h"
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
   Parser * parser;
   



 public slots:
   void receiveRoom(QObject *, Room *);

 signals:
   void releaseRoom(QObject *, int);
   void keepRoom(QObject *, int);
 public:
   Approved(Parser * parser, ParseEvent * event, int tolerance);
   ~Approved();
   
   Room * oneMatch();
   QObject * getOwner(); 
   void reset();
};
#ifdef DMALLOC
#include <mpatrol.h>
#endif

#endif
