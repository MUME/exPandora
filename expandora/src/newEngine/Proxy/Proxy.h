#ifndef PROXY
#define PROXY

#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <qglobal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <qmutex.h>

#if defined Q_OS_LINUX || defined Q_OS_MACX
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAEINPROGRESS EINPROGRESS
#define WSAGetLastError() errno
#define INVALID_SOCKET (-1)
#define closesocket close
#define SOCKET int
#define ioctlsocket ioctl
#elif defined Q_OS_WIN32
#define socklen_t int
#include <winsock.h>
#endif

#define PROXY_BUFFER_SIZE 4096
#define MAXCONNECTIONS 1

#define XX() printf ("DBG\n");
#define NOBODY -1

#ifdef DEBUG
#define DEBUG_FILE_NAME "completelog.txt"
FILE *debug_file;
#endif

#include "TelnetFilter.h"
#include "Lexer.h"



class Proxy : public QThread{
 public:
  Proxy(int _local, char * _rh, int _rp) : local_port(_local), remote_host(_rh), remote_port(_rp) {proxy_init();}
  int proxy_init();

  void run();
  void send_line_to_mud(char *line);
  int send_line_to_user(char *line);
  void proxy_shutdown();
  void attachFilter(TelnetFilter * _filter) {filter = _filter;}

 private:
  int     local_port;
  char *  remote_host;
  int     remote_port;

  bool mud_emulation;
#if defined Q_OS_LINUX || defined Q_OS_MACX
  static void refresh(int);
#endif
  void userinit(void);
  int userfindfree(void);
  int useradd(int sock);
  void userdel(int sock);

  QMutex tcp_mutex;
  TelnetFilter * filter;
  int proxy_hangsock;
  fd_set proxy_descr1, proxy_descr2;
  
  struct sockaddr_in my_net_name, his_net_name;
  
  struct juzer_s {
    int sock;
    int sout;
    char realident[64];
  } juzer[MAXCONNECTIONS];

};
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif