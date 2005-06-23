#ifndef PROXY
#define PROXY


#include <qobject.h>
#include <qsocket.h>
#include "Component.h"
#include "ConnectionAccepter.h"

class Proxy : public Component {
 signals:
  void analyzeUserStream(char *, int);
  void analyzeMudStream(char *, int);

 public slots:
  void processUserStream();
  void processMudStream();

 public:
  Proxy() {};
  ~Proxy() {};
  void start(QThread::Priority priority);
  void acceptConnection(ConnectionAccepter * source, int socket);

 private:
  Q_OBJECT
    
  QSocket mudSocket;
  QSocket userSocket;
  char buffer[8192];

};
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
