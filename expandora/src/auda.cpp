/* automapper/database module for powwow. Part of the Pandora Project (c) 2003 */

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <QThread>
#include <QMutex>




#include "defines.h"
#include "configurator.h"


#include "struct.h"

#include "rooms.h"
#include "xml2.h"

#include "dispatch.h"
#include "renderer.h"
#include "event.h"
#include "stacks.h"
#include "forwarder.h"
#include "utils.h"
#include "engine.h"

#ifdef Q_OS_MACX
#include <CoreFoundation/CoreFoundation.h>
#endif

/* global flags */
int glredraw = 1;		/* redraw is needed */
int glquit = 0;			/* quiting ... */
int mud_emulation = 0;          /* we are in emulation mode */




/* RENDERER THREAD MAIN DEFINES*/
class RendererThread : public QThread {

public:
        virtual void run();
} renderer_thread;

void RendererThread::run()
{
  renderer_main();
  print_debug(DEBUG_RENDERER, "quiting child thread function"); 
}
/* RENDERER THREAD ENDS*/



/* PROXY THREAD DEFINES */
class ProxyThread : public QThread {
public:
        virtual void run();
} proxy_thread;

void ProxyThread::run()
{
  proxy_loop();
}
/* PROX THREAD ENDS */


const char *exitnames[] = { "north", "east", "south", "west", "up", "down" };

/* global base settings */
int     auda_argc;
char    **auda_argv;
    
void print_usage()
{
  printf("Usage: pandora <options>\r\n");
  printf("Options:\r\n\r\n");
  printf("  --help / -h                   - this helpfile.\r\n");
  printf("  --base / -b  <filename>       - override the database file.\r\n");
  printf("  --localport / -lp  <port>     - override the local port number.\r\n");
  printf("  --hostname / -hn  <host>      - override the remote (game) host name.\r\n");
  printf("  --remoteport / -rp  <port>    - override the remote (game) port number.\r\n");
  printf("  --emulate / -e  <port>        - emulate mud environment (disabled).\r\n");
  printf("  --config / -c  <configfile>   - load config file.\r\n");
  
  printf("\r\n");
}

int main(int argc, char *argv[])
{
    int i;
    char *  resPath = 0;
    char    override_base_file[MAX_STR_LEN] = "";
    int     override_local_port = 0;
    char    override_remote_host[MAX_STR_LEN] = "";
    int     override_remote_port = 0;
    char    configfile[MAX_STR_LEN] = "configs/default.conf"; 
    int     default_local_port = 3000;
    int     default_remote_port = 4242;

    
    
    
#ifdef Q_OS_MACX
    CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef, 
						  kCFURLPOSIXPathStyle);
    const char *appPath = CFStringGetCStringPtr(macPath, 
						CFStringGetSystemEncoding());
    resPath = (char *)malloc(strlen(appPath)+25);
    strcpy(resPath, appPath);
    strcat(resPath, "/Contents/Resources/");

    char    default_base_file[MAX_STR_LEN] = "databases/base.xml";   
    char    default_remote_host[MAX_STR_LEN] = "";
    strcpy(configfile, "configs/default.conf");

    CFRelease(pluginRef);
    CFRelease(macPath);

#else
    resPath = "";
    char    default_base_file[MAX_STR_LEN] = "mume.xml";   
    char    default_remote_host[MAX_STR_LEN] = "129.241.210.221";
#endif


  
    /* parse arguments */
    auda_argc = argc;
    auda_argv = argv;

    for (i=1; i < argc; i++) {

      if ((strcmp(argv[i], "--config") == 0) || ( strcmp(argv[i], "-c") == 0)) 
      {
        if (i == argc) {
          printf("Too few arguments. Missing config file name.\r\n");
          print_usage();
          exit(1);
        }
        i++;

        strcpy(configfile, argv[i]);
	resPath = ""; // obviously the user has an own config file - including the path
      } 
      
      if ((strcmp(argv[i], "--emulate") == 0) || ( strcmp(argv[i], "-e") == 0)) 
      {
        printf("Pandora: Starting in MUD emulation mode.\r\n");
        mud_emulation = 1;
      } 

      if ((strcmp(argv[i], "--base") == 0) || ( strcmp(argv[i], "-b") == 0)) 
      {
        if (i == argc) {
          printf("Too few arguments. Missing database.\r\n");
          print_usage();
          exit(1);
        }
        i++;
        strcpy(override_base_file, argv[i]); // overriding the database file is possible even with default config file
      } 

      if ((strcmp(argv[i], "--hostname") == 0) || ( strcmp(argv[i], "-hn") == 0)) 
      {
        if (i == argc) {
          printf("Too few arguments. Wrong hostname given.\r\n");
          print_usage();
          exit(1);
        }
        i++;
        strcpy(override_remote_host, argv[i]);
      } 

      if ((strcmp(argv[i], "--localport") == 0) || ( strcmp(argv[i], "-lp") == 0)) 
      {
        if (i == argc) {
          printf("Too few arguments. Missing localport.\r\n");
          print_usage();
          exit(1);
        }
        i++;
        override_local_port = atoi(argv[i]);
      } 

      if ((strcmp(argv[i], "--remoteport") == 0) || ( strcmp(argv[i], "-rp") == 0)) 
      {
        if (i == argc) {
          printf("Too few arguments. Missing targetport.\r\n");
          print_usage();
          exit(1);
        }
        i++;
        override_remote_port = atoi(argv[i]);
      } 

      
      if ((strcmp(argv[i], "--help") == 0) || ( strcmp(argv[i], "-h") == 0)) 
      {
        print_usage();
        exit(1);
      }
      
    }



    /* set analyzer engine defaults */
    engine_init();
    
    printf("Using config file : %s.\r\n", configfile);
    conf.load_config(resPath, configfile);
    conf.load_engine_config(resPath, "configs/engine.cfg");
    
    
    if (override_base_file[0] != 0) {
      conf.set_base_file(override_base_file);
    } else if ( conf.get_base_file() == "") {
      conf.set_base_file(default_base_file);
    }
    printf("Using database file : %s.\r\n", (const char*) conf.get_base_file() );
    
    if (override_remote_host[0] != 0) {
      conf.set_remote_host(override_remote_host);
    } else if ( conf.get_remote_host().isEmpty() ) {
      conf.set_remote_host(default_remote_host);
    }
    printf("Using target hostname : %s.\r\n", (const char*) conf.get_remote_host() );

    if (override_local_port != 0) {
      conf.set_local_port(override_local_port);
    } else if ( conf.get_local_port() == 0) {
      conf.set_local_port(default_local_port);
    }
    printf("Using local port : %i.\r\n", conf.get_local_port());

    if (override_remote_port != 0) {
      conf.set_remote_port(override_remote_port);
    } else if (conf.get_remote_port() == 0) {
      conf.set_remote_port(default_remote_port);
    }
    printf("Using target port : %i.\r\n", conf.get_remote_port());

    conf.set_conf_mod( false );

    printf("-- Starting Pandora\n");
  
  
    proxy_init();

    printf("Loading the database ... \r\n");
    xml_readbase( conf.get_base_file() );
//    roomer.database_integrity_check("After xml reading");
    
    printf("Successfuly loaded %i rooms!\n", roomer.amount);

    /* init */
    Ctop = new Tevent;
    Ctop->type = C_EMPTY;
    Ctop->next = NULL;
    Ctop->prev = NULL;
    Ctop->data[0] = 0;
    Cbottom = Ctop;

    Rtop = new Tevent;
    Rtop->type = R_EMPTY;
    Rtop->next = NULL;
    Rtop->prev = NULL;
    Rtop->data[0] = 0;
    Rbottom = Rtop;

    pre_Cstack=NULL;
    pre_Rstack=NULL;
    
//    renderer_main();

    /* special init for the mud emulation */
    if (mud_emulation) {
      printf("Starting in MUD emulation mode...\r\n");
      
      strcpy(last_prompt, "-->");
      stacker.put(1);
      stacker.swap();
    }
 
    renderer_thread.start();
    proxy_thread.start();

    renderer_thread.wait();
    proxy_thread.wait();

    

    printf("Quiting !\n");


    return 0;
}
