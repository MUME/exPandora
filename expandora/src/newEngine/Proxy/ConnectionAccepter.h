#ifndef CONNECTIONACCEPTER
#define CONNECTIONACCEPTER


#include <QTcpServer>

class Proxy;

class ConnectionAccepter : public QTcpServer {
 public:
  ConnectionAccepter(Proxy * home, quint16 port);
  virtual void newConnection(int socket);
  
 private:
   Q_OBJECT
   Proxy * home;
};

#endif
