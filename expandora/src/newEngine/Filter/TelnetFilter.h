#ifndef TELNETFILTER
#define TELNETFILTER

#include <qobject.h>


// we should chage that prefix ... perhaps '¤' - or is that only present on german keyboards ;)
#define MAP_COMMAND_PREFIX 'm' 


class TelnetFilter : public QObject {
 private:
  Q_OBJECT

    public slots:

  /** these methods purge protocol-specific things from the imput, save a copy 
      in the buffer, then call the analyzer thread via a QWaitCondition; so they
      return before the buffer is analyzed ... */
  int analyzeMudStream(char * input, int length);

  /** this one should probably be handled differently - not everything is to be
      passed on - a hackish way will be modifying the input ... */
  int analyzeUserStream(char * input, int length);
 private:
  char * purgeProtocolSequences(char * input, int length);
 signals:
  void newMudInput(char *);
  void newUserInput(char *);
};

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
