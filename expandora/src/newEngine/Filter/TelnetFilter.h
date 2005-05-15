#ifndef TELNETFILTER
#define TELNETFILTER

#include "Component.h"
#include <qthread.h>





class TelnetFilter : public Component
{
private:
  Q_OBJECT
  char * purgeProtocolSequences(char * input, int length);

public slots:

  /** these methods make a "clean" copy of the passed string, 
   *  wake the lexer thread and return
   */
  int analyzeMudStream(char * input, int length);
  int analyzeUserStream(char * input, int length);
  void start(QThread::Priority) {}
  
signals:
  void newMudInput(char *);
  void newUserInput(char *);
};

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
