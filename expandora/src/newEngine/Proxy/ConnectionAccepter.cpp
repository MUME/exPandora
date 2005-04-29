#include "ConnectionAccepter.h"
#include "Proxy.h"

void ConnectionAccepter::newConnection(int socket) {
  home->acceptConnection(this, socket);
}

ConnectionAccepter::ConnectionAccepter(Proxy * in_home, Q_UINT16 port) : 
  QServerSocket(port), home(in_home) {}
  
