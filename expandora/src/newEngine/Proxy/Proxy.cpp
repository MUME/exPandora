/* Packet Proxy by WWW <www@cyber-wizard.com>
 * Proxy with ssl protocol support by Glip <glip@cyber-wizard.com>
 *
 * for solaris: gcc Proxy.c -lsocket -lnsl
 *
 */

#include "Proxy.h"
#include <qvariant.h>


/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */

extern "C" MY_EXPORT Component * createComponent() {
  return new Proxy;
}


void Proxy::run()
{
  // we will get the accepter back when it is connected
  new ConnectionAccepter(this, property("localPort").toInt());
}

void Proxy::acceptConnection(ConnectionAccepter * source, int playerSocket) {
  userSocket.setSocketDescriptor(playerSocket);

  mudSocket.connectToHost(property("remoteHost").toString(), property("remotePort").toInt());
  delete source; // we only accept one connection
  QObject::connect(&userSocket, SIGNAL(readyRead()), this, SLOT(processUserStream()));
  QObject::connect(&mudSocket, SIGNAL(readyRead()), this, SLOT(processMudStream()));
}

void Proxy::processUserStream() {
  int read;
  while(userSocket.bytesAvailable()) {
    read = userSocket.read(buffer, 8191);
    if (read != -1) {
      buffer[read] = 0;
      mudSocket.write(buffer, read);
      emit analyzeUserStream(buffer, read);
    }
  }
}
  
void Proxy::processMudStream() {
  int read;
  while(mudSocket.bytesAvailable()) {
    read = mudSocket.read(buffer, 8191);
    if (read != -1) {
      buffer[read] = 0;
      userSocket.write(buffer, read);
      emit analyzeUserStream(buffer, read);
    }
  }
}

 
