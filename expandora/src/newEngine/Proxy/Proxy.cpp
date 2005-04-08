/* Packet Proxy by WWW <www@cyber-wizard.com>
 * Proxy with ssl protocol support by Glip <glip@cyber-wizard.com>
 *
 * for solaris: gcc Proxy.c -lsocket -lnsl
 *
 */
#include "Proxy.h"

#if defined Q_OS_LINUX || defined Q_OS_MACX

void Proxy::refresh(int)
  {
     if (fork()>0)
        exit(0);
     signal (SIGXCPU, refresh);
  }

#endif


/* always works for user 0, since supposedly we have only 1 user */
void Proxy::send_line_to_mud(char *line) 
{
  int rd;
  
  tcp_mutex.lock();
  rd = strlen(line);
  if (rd > PROXY_BUFFER_SIZE) {
    printf("proxy: I/O BUFFER OVERFLOW!\r\n");
    return;
  }
  
  send(juzer[0].sout, line, rd, 0);

  tcp_mutex.unlock();
  return;
}

/* always works for user 0, since supposedly we have only 1 user */
int Proxy::send_line_to_user(char *line) 
{
  int rd;
  
  tcp_mutex.lock();
  rd = strlen(line);
  if (rd > PROXY_BUFFER_SIZE) {
    printf("proxy: I/O BUFFER OVERFLOW!\r\n");
    return 1;
  }
  
  send(juzer[0].sock, line, rd, 0);

  tcp_mutex.unlock();
  return 0;
}


void Proxy::userinit(void)
{
  int i;
  for (i=0; i<MAXCONNECTIONS; i++)
      juzer[i].sock=0;
}

int Proxy::userfindfree(void)
{
  int i;
  for (i=0; i<MAXCONNECTIONS; i++)
    if (!juzer[i].sock) return i;
  return -1;
}

int Proxy::useradd(int sock)
{
  int newsock;

  if((newsock=userfindfree())== -1) 
      return -1;
  
  juzer[newsock].sock=sock;
  juzer[newsock].realident[0]=(char)0;
  juzer[newsock].sout=0;
  return newsock;
}

void Proxy::userdel(int sock)
{
  int i;
  for (i = 0; i < MAXCONNECTIONS; i++)
    if (juzer[i].sock == sock)
      {
        shutdown (sock, 2);
        closesocket (sock);
        if (!mud_emulation) {
          shutdown (juzer[i].sout, 2);
          closesocket (juzer[i].sout);
        }
        juzer[i].sock = 0;
      }
}


int Proxy::proxy_init()
{
  struct hostent *hent;
/*  char proxy_s3ng[256];
 */

#ifdef Q_OS_WIN32
    WSADATA wsadata;
#define WSVERS MAKEWORD(2,2)
#endif

    //fprintf(stderr, "Proxy initialising\n");
    
#ifdef Q_OS_WIN32
    if (WSAStartup(WSVERS, &wsadata) != 0)
    {
        printf("Failed to initialise Windows Sockets.\n");
        exit(1);
    }
#endif



#if defined Q_OS_LINUX || defined Q_OS_MACX
  signal (SIGXCPU, refresh);
#endif

  #ifdef DEBUG
    debug_file = fopen(DEBUG_FILE_NAME, "w+");
  #endif
  
  printf("proxy: initializing...\r\n");
  if ((proxy_hangsock = socket (PF_INET, SOCK_STREAM, 0))<0)
  {
    fprintf (stderr, "proxy: Cannot open socket\n");
    exit(1);
  }

  my_net_name.sin_family=AF_INET;
  my_net_name.sin_addr.s_addr=INADDR_ANY;
  my_net_name.sin_port=htons(local_port);

  his_net_name.sin_family=AF_INET;
  his_net_name.sin_port=htons(remote_port);

  if ((hent=gethostbyname(remote_host))==NULL)
  {
    fprintf (stderr, "proxy: %s: Unknown host\n", remote_host);
    exit(1);
  }

  his_net_name.sin_addr.s_addr=*((unsigned int *)hent->h_addr);

  if (bind(proxy_hangsock, (struct sockaddr *)&my_net_name, sizeof(struct sockaddr_in)))
  {
    fprintf (stderr, "proxy: Cannot bind socket\n");
    exit(1);
  }

  
  printf("Proxy: ready and listening...\r\n");
  listen(proxy_hangsock, MAXCONNECTIONS);


  return 0;
}


void Proxy::run()
{
  int i, n, max;

  while (1) {
    
    FD_ZERO(&proxy_descr1);
    FD_ZERO(&proxy_descr2);

    max = 0;
    n = 0;
    for (i=0; i<MAXCONNECTIONS; i++)
      if (juzer[i].sock > 0) {
        n++;
        FD_SET(juzer[i].sock, &proxy_descr1);
        FD_SET(juzer[i].sock, &proxy_descr2);
        if (juzer[i].sock>max) max = juzer[i].sock;
          
        if (!mud_emulation) {
          FD_SET(juzer[i].sout, &proxy_descr1);
          FD_SET(juzer[i].sout, &proxy_descr2);
          if (juzer[i].sout>max) max = juzer[i].sout;
        }
      }
  
    FD_SET(proxy_hangsock, &proxy_descr1);

    if (proxy_hangsock>max) 
      max = proxy_hangsock;
    
    n = select (max+1, &proxy_descr1, NULL, &proxy_descr2, NULL);
    {
    int x, saddrlen;
    saddrlen=sizeof(struct sockaddr_in);
    if (FD_ISSET(proxy_hangsock, &proxy_descr1))
    {
      struct sockaddr_in mytempname;
      int newsock;
      
      newsock=accept(proxy_hangsock, (struct sockaddr *)&mytempname, (socklen_t *) &saddrlen);
      if (mud_emulation) {
        char emulation_welcome_message[] = "Pandora MUD Emulation.\r\n";
        useradd(newsock);
        
        send(newsock, emulation_welcome_message, strlen(emulation_welcome_message), 0); 
        
        send_line_to_user(">");

      }
      
      
      if (newsock>0) {
        int index, snew;
        
        mytempname.sin_family = his_net_name.sin_family;
        mytempname.sin_addr.s_addr = his_net_name.sin_addr.s_addr;
        mytempname.sin_port = his_net_name.sin_port;

        if (!mud_emulation) {
          if ((snew=socket(PF_INET, SOCK_STREAM, 0))>0) {
            if (!connect(snew, (struct sockaddr *)&mytempname, sizeof(mytempname))) {
              if ((index=useradd (newsock))>=0)
                juzer[index].sout=snew;
              else {
                send(newsock, "------[ ERROR: Cannot create forward entry\n\r", 29, 0); 
                shutdown (newsock,2); 
                closesocket (newsock);
              }
            } else {
              send(newsock, "------[ ERROR: Proxy cannot connect to remote host.\n\r", 16, 0); 
              shutdown (newsock,2); 
              closesocket (newsock);
            }
          } else {
            send(newsock, "------[ ERROR: Cannot create socket\n\r", 22, 0); 
            shutdown (newsock,2); 
            closesocket (newsock);
          }
        }
      
        
      }
    }
    
    for (x=0; x<MAXCONNECTIONS; x++)
      if (juzer[x].sock > 0)
      {
        if (FD_ISSET(juzer[x].sock, &proxy_descr2) || FD_ISSET(juzer[x].sout, &proxy_descr2))
        {
          userdel(juzer[x].sock);
        } else {
          if (FD_ISSET(juzer[x].sock,&proxy_descr1)) {
            int rd;
            char intbuff[PROXY_BUFFER_SIZE];

            /* user stream */
            
            rd = recv(juzer[x].sock, intbuff, sizeof(intbuff), 0);
            if (rd>0) {
              #ifdef DEBUG
                intbuff[rd] = 0;
                //fprintf(debug_file, "\r\n-received_from_user(len %i)->", rd);
                fwrite(intbuff, rd, 1, debug_file);
                fflush(debug_file);
              #endif
              
	      rd = filter->analyzeUserStream(intbuff, rd);

              if (!mud_emulation) {
                tcp_mutex.lock();

                #ifdef DEBUG
                  intbuff[rd] = 0;
                  //fprintf(debug_file, "\r\n-sent_to_mud(len %i)->", rd);
                  fwrite(intbuff, rd, 1, debug_file);
                  fflush(debug_file);
                #endif

                send(juzer[x].sout, intbuff, rd, 0);
              
                tcp_mutex.unlock();
              }
              
            } else {
              userdel(juzer[x].sock);
            }

          }
          
          if (mud_emulation)
            continue;
          
          if (FD_ISSET(juzer[x].sout,&proxy_descr1)) {
            int rd;
            char intbuff[PROXY_BUFFER_SIZE];
            
            /* mud stream */
            
            rd = recv(juzer[x].sout, intbuff, sizeof(intbuff), 0);
            if (rd>0) {
              #ifdef DEBUG
                intbuff[rd] = 0;
                //fprintf(debug_file, "\r\n<-receive_from_mud(len %i)-", rd);
                fwrite(intbuff, rd, 1, debug_file);
                fflush(debug_file);
              #endif
              


	      rd = filter->analyzeMudStream(intbuff, rd);

              tcp_mutex.lock();

              #ifdef DEBUG
                intbuff[rd] = 0;
                //fprintf(debug_file, "\r\n<-sent_to_user(len %i)-", rd);
                fwrite(intbuff, rd, 1, debug_file);
                fflush(debug_file);
              #endif
              
              send(juzer[x].sock, intbuff, rd, 0);

              tcp_mutex.unlock();
              
            } else { 
              userdel(juzer[x].sock);
            }

          }
        }
    
      } /* if in for-cycle ends */
      
      
    }
  }
  return;

}


/* shutdown function to close sockets properly. */
/* used in userfunc in mquit command */
void Proxy::proxy_shutdown()
{
  int i;
    
  for (i=0; i<MAXCONNECTIONS; i++)
    if (!juzer[i].sock) {
      userdel(i);
    }

  shutdown(proxy_hangsock, 2);
  closesocket(proxy_hangsock);
      
#ifdef Q_OS_WIN32
    WSACleanup();
#endif

}
