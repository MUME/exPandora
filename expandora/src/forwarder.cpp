//#define DEBUG

#include <csignal>
#include <qglobal.h>

#if defined Q_OS_LINUX || defined Q_OS_MACX || defined Q_OS_FREEBSD
    #include <unistd.h>
    #include <netdb.h>
    #include <sys/types.h>
    //#include <sys/time.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/ioctl.h>
    
    
    
    #define WSAEWOULDBLOCK EWOULDBLOCK
    #define WSAEINPROGRESS EINPROGRESS
    #define WSAGetLastError() errno
    #define WSACleanup
    #define INVALID_SOCKET (-1)
    #define closesocket close
    #define SOCKET int
    #define ioctlsocket ioctl
#elif defined Q_OS_WIN32
    #define socklen_t int
    #define WSVERS MAKEWORD(2,2)
    #include <winsock.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <QMutex>


#include "defines.h"
#include "configurator.h"
#include "dispatch.h"
#include "Map.h"
#include "mainwindow.h"
#include "engine.h"
#include "forwarder.h"


class Proxy       proxy;

#ifdef DEBUG
  #define DEBUG_FILE_NAME "completelog.txt"
  FILE *debug_file;
#endif

int Proxy::init()
{
    struct sockaddr_in proxy_name;

    user.setXmlTogglable( false );
    mud.setXmlTogglable( true );

    #ifdef Q_OS_WIN32
        WSADATA wsadata;
        if (WSAStartup(WSVERS, &wsadata) != 0)
        {
            printf("Failed to initialise Windows Sockets.\n");
            exit(1);
        } else {
	    printf("Started windows sockets.\r\n");
	}	
    #endif

    #ifdef DEBUG
        debug_file = fopen(DEBUG_FILE_NAME, "w+");
    #endif
  
    printf("proxy: initializing...\r\n");
    if ((proxy_hangsock = socket (AF_INET, SOCK_STREAM, 0))<0) {
        fprintf (stderr, "proxy: Cannot open socket\n");
        exit(1);
    }

    int opt = 1;
    if(setsockopt(proxy_hangsock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == -1) {
        perror("Couldn't set the SO_REUSEADDR.\r\n");
        exit(1);
    } 

    proxy_name.sin_family=AF_INET;
    proxy_name.sin_addr.s_addr=INADDR_ANY;
    proxy_name.sin_port=htons(conf.get_local_port());

    if (bind(proxy_hangsock, (struct sockaddr *)&proxy_name, sizeof(struct sockaddr_in))) {
        fprintf (stderr, "proxy: Cannot bind socket\n");
        exit(1);
    }

  
    printf("Proxy: ready and listening...\r\n");
    listen(proxy_hangsock, 3);

  return 0;
}


int Proxy::loop(void)
{
    fd_set   input, exc;
    int n;
        
  while (1) {
    
    FD_ZERO(&input);
    FD_ZERO(&exc);

    if (user.isConnected() > 0) {
        SOCKET sock = user.getSocket();
        FD_SET(sock, &input);
        FD_SET(sock, &exc);
          
        if (!mudEmulation) {
            SOCKET sock = mud.getSocket();
            FD_SET(sock, &input);
            FD_SET(sock, &exc);
        }
    }
  
    FD_SET(proxy_hangsock, &input);

    n = select (FD_SETSIZE , &input, NULL, &exc, NULL);
    if (n < 0) {
        /* .... */
        printf("Networking error: Select failed.  Quiting.\r\n"); 
        exit(1);
    }
        
    if (FD_ISSET(proxy_hangsock, &input)) 
        if (!user.isConnected())
            incomingConnection();
                
    if (user.isConnected()) {
        if (FD_ISSET(user.getSocket(), &exc) || FD_ISSET(mud.getSocket(), &exc))
            shutdown();
    
        if (FD_ISSET(user.getSocket(),&input)) {
            /* user stream */
            int size;
            
            size = user.read();
            if (size > 0) {
                size = dispatcher.analyzeUserStream(user);
                if (!mudEmulation) {
                    mud.write(user.buffer, size);
                }
            } else {
                if (WSAGetLastError() == WSAEWOULDBLOCK) 
                    continue;
                shutdown();
             }
        }
          
        if (mudEmulation)
            continue;
          
        if (FD_ISSET(mud.getSocket(),&input)) {
            int size;
            
            size = mud.read();
            if (size>0) {
                size = dispatcher.analyzeMudStream(mud);
                user.write( mud.buffer, size );               
            } else { 
                if (WSAGetLastError() == WSAEWOULDBLOCK) 
                    continue;
                shutdown();
            }

        }
    }
    
  }     /* while loop ends */
  
  return 0;
}

void Proxy::run()
{
    loop();
}


void Proxy::send_line_to_mud(char *line) 
{
    mud.send_line(line);    
}

void Proxy::send_line_to_user(char *line) 
{
    user.send_line(line);    
}

void Proxy::sendMudEmulationGreeting()
{
    CRoom *r;
    
    user.send_line( "Welcome to Pandora MUD Emulation!\r\n\r\n" );
    
    r = Map.getRoom( 1 );
    r->sendRoom();
    
    user.send_line( "-->" );
}

bool Proxy::connectToMud()
{
    printf("Trying to connect to MUD...\r\n");

    if (mud.openConnection(conf.get_remote_host(), conf.get_remote_port()) != true) {
        user.send_line( "----[ Pandora: Failed to connect to remote host. See terminal log for details.\r\n");
        return false;
    } else {
        printf("Connected!\r\n");
        return true;        
    }    
}


void Proxy::incomingConnection()
{
    SOCKET newsock;
    int size;
    struct sockaddr_in networkName;
    
//    printf("Incoming connection!\r\n");
    
    size = sizeof(struct sockaddr_in);
    newsock = accept(proxy_hangsock, (struct sockaddr *)&networkName, (socklen_t *) &size);
    if (newsock > 0) {
        user.setConnection( newsock );
//        user.nonblock();                
                
        if (mudEmulation) 
            sendMudEmulationGreeting();
        else 
            if (!connectToMud()) 
                user.close();

        emit connectionEstablished();
        Engine.clear(); /* clear event pipes */   
    } else {
//        printf("Connection to user failed! New  socket is not valid.\r\n");
//          printf(".");
//        ::shutdown(newsock, 2);
//        ::closesocket(newsock);
//        user.close();
    }
}


void Proxy::shutdown()
{
    printf("Proxy is shutting down.\r\n");

    user.close();
    user.clear();
    if (!mudEmulation) {
        mud.close();
        mud.clear();
    }
    
    emit connectionLost();
//    #ifdef Q_OS_WIN32
//        WSACleanup();
//    #endif
}

// ----------------------------------  ProxySocket ------------------------------------------
void ProxySocket::send_line(char *line) 
{
    mutex.lock();
    send(sock, line, strlen(line), 0);
    mutex.unlock();
}

void ProxySocket::clear() { 
    sock = 0;
    mainState = 0;
    subState = 0;
    subchars.clear();
    fragment.clear();
    xmlMode = false;
}

void ProxySocket::close() {
    ::shutdown(sock, 2);
    printf("Closing the socket.\r\n");
    ::closesocket(sock);               
}

ProxySocket::ProxySocket(bool xml) 
{ 
    clear(); 
    setXmlTogglable( xml); 
}

SOCKET ProxySocket::getSocket()     
{ 
    return sock; 
}

bool ProxySocket::isXmlMode()    
{ 
    return xmlMode; 
}

void ProxySocket::setXmlMode( bool b ) 
{ 
    if (xmlTogglable && b) 
        xmlMode = b; 
}

bool ProxySocket::isXmlTogglable()    
{ 
    return xmlTogglable; 
}

void ProxySocket::setXmlTogglable( bool b ) { 
    xmlTogglable = b; 
}

bool ProxySocket::isConnected()
{
    if (sock)
        return true;
    else 
        return false;
}

void ProxySocket::setConnection(SOCKET s)
{
    sock = s;
}

bool ProxySocket::openConnection(QByteArray name, int port)
{
//    struct sockaddr mytempname;
    SOCKET snew;
    struct sockaddr_in networkName;
    struct hostent *h;

    printf("Trying to resolve %s\r\n", (const char*) name);
    if (!(h = gethostbyname ( (const char*) name) ) ) {
        fprintf(stderr, "Can't resolve host: %s.\n", (const char*) name);
        return false;
    }

    networkName.sin_addr.s_addr = *((unsigned int *) h->h_addr);     
    networkName.sin_family = AF_INET;
    networkName.sin_port = htons(port);

    snew=socket(AF_INET, SOCK_STREAM, 0);
    if (snew <= 0) {
        printf("Cannot create new socket!\r\n");
        return false;
    }
    
    printf("Connecting ....\r\n");
    if (!connect(snew, (struct sockaddr *)&networkName, sizeof(struct sockaddr_in))  )  {
        sock = snew;
    } else {
        printf("Failed to connect to remote host!\r\n");
        return false;
    }

//    nonblock();                

    return true;
}

int ProxySocket::read(char * buffer, int len)
{
    int rd;
    
    mutex.lock();
    rd = recv(sock, buffer, len, 0);
    #ifdef DEBUG
        buffer[rd] = 0;
        fprintf(debug_file, "<-Received(len %i)", rd);
        fwrite(buffer, rd, 1, debug_file);
        fflush(debug_file);
    #endif
    mutex.unlock();
    return rd;
}

void ProxySocket::write(char *buffer, int len)
{
    mutex.lock();
    #ifdef DEBUG
        buffer[len] = 0;
        fprintf(debug_file, "<-Written(len %i)", len);
        fwrite(buffer, len, 1, debug_file);
        fflush(debug_file);
    #endif
    send(sock, buffer, len, 0);
    mutex.unlock();
}

int ProxySocket::read()
{
    length = read(buffer, sizeof(buffer) );
    return length;
}

void ProxySocket::nonblock()
{
    unsigned long on = 1;
    
    ioctlsocket(sock, FIONBIO, &on);
}
