#ifndef CONNECTIONACCEPTER
#define CONNECTIONACCEPTER

#include <qserversocket.h>

class Proxy;

class ConnectionAccepter : public QServerSocket {
 public:
  ConnectionAccepter(Proxy * home, Q_UINT16 port);
  virtual void newConnection(int socket);
  
 private:
   Q_OBJECT
   Proxy * home;
};

#endif
