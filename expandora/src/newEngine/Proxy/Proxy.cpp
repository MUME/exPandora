/* Packet Proxy by WWW <www@cyber-wizard.com>
 * Proxy with ssl protocol support by Glip <glip@cyber-wizard.com>
 *
 * for solaris: gcc Proxy.c -lsocket -lnsl
 *
 */

#include "Proxy.h"
#include <qvariant.h>

void Proxy::start()
{
  // we will get the accepter back when it is connected
  new ConnectionAccepter(this, property("localPort").toInt());
}

void Proxy::acceptConnection(ConnectionAccepter * source, int playerSocket) {
  userSocket.setSocket(playerSocket);

  mudSocket.connectToHost(property("remoteHost").asString(), property("remotePort").toInt());
  delete source; // we only accept one connection
  QObject::connect(&userSocket, SIGNAL(readyRead()), this, SLOT(processUserStream()));
  QObject::connect(&mudSocket, SIGNAL(readyRead()), this, SLOT(processMudStream()));
}

void Proxy::processUserStream() {
  int read;
  while(userSocket.bytesAvailable()) {
    read = userSocket.readBlock(buffer, 8191);
    if (read != -1) {
      buffer[read] = 0;
      mudSocket.writeBlock(buffer, read);
      emit analyzeUserStream(buffer, read);
    }
  }
}
  
void Proxy::processMudStream() {
  int read;
  while(mudSocket.bytesAvailable()) {
    read = mudSocket.readBlock(buffer, 8191);
    if (read != -1) {
      buffer[read] = 0;
      userSocket.writeBlock(buffer, read);
      emit analyzeUserStream(buffer, read);
    }
  }
}

 
