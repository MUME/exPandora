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
#ifndef MONOLITHIC
extern "C" MY_EXPORT Component * createComponent()
{
  return new Proxy;
}
#else
Initializer<Proxy> proxy("Proxy");
#endif

Proxy::Proxy() : Component(true), userSocket(0)
{}

void Proxy::init()
{
  server = new QTcpServer(this);
  mudSocket = new QTcpSocket(this);
  server->setMaxPendingConnections(1);
  if(!connect(server, SIGNAL(newConnection()), SLOT(acceptConnection()))) throw "can't connect to TcpServer";
  localPort = options["localPort"].toInt();
  remotePort = options["remotePort"].toInt();
  remoteHost = options["remoteHost"].toString();
  server->listen(QHostAddress::Any, localPort);
}


void Proxy::acceptConnection()
{
  if (userSocket) return; // ignore additional connections
  userSocket = server->nextPendingConnection();
  
  mudSocket->connectToHost(remoteHost, remotePort);

  QObject::connect(userSocket, SIGNAL(readyRead()), this, SLOT(processUserStream()));
  QObject::connect(mudSocket, SIGNAL(readyRead()), this, SLOT(processMudStream()));
  QObject::connect(mudSocket, SIGNAL(disconnected()), this, SLOT(resetServer()));
  QObject::connect(userSocket, SIGNAL(disconnected()), this, SLOT(resetServer()));
}

void Proxy::resetServer()
{
  userSocket->disconnect();
  mudSocket->disconnect();
  userSocket->disconnectFromHost();
  mudSocket->disconnectFromHost();
  delete(userSocket);
  userSocket = 0;
  
}

void Proxy::processUserStream()
{
  int read;
  while(userSocket->bytesAvailable())
  {
    read = userSocket->read(buffer, 8191);
    if (read != -1)
    {
      buffer[read] = 0;
      mudSocket->write(buffer, read);
      emit analyzeUserStream(purgeProtocolSequences(buffer, read));
    }
  }
}

void Proxy::processMudStream()
{
  int read;
  while(mudSocket->bytesAvailable())
  {
    read = mudSocket->read(buffer, 8191);
    if (read != -1)
    {
      buffer[read] = 0;
      userSocket->write(buffer, read);
      emit analyzeMudStream(purgeProtocolSequences(buffer, read));
    }
  }
}

char * Proxy::purgeProtocolSequences(char * input, int length)
{
  char * lexingBuffer = new char[length+1];
  int lag = 0;
  for (int i = 0; i < length; ++i)
  {
    if (input[i] == '\xFF')
    {
      i += 2;
      lag += 3;
    }
    else lexingBuffer[i-lag] = input[i];
  }
  lexingBuffer[length-lag] = 0;
  return lexingBuffer;
}
