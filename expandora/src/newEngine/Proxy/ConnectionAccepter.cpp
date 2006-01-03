#include "ConnectionAccepter.h"
#include "Proxy.h"

void ConnectionAccepter::newConnection(int socket) {
  home->acceptConnection(this, socket);
}

ConnectionAccepter::ConnectionAccepter(Proxy * in_home, quint16 port) : 
   home(in_home) {listen(QHostAddress::Any, port);}
  
