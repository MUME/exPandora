/* automapper/database module for powwow. Part of the Pandora Project (c) 2003 */

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <QThread>
#include <QMutex>




#include "defines.h"
#include "CRoom.h"
#include "configurator.h"


#include "Map.h"
#include "xml2.h"

#include "dispatch.h"
#include "mainwindow.h"
#include "stacks.h"
#include "forwarder.h"
#include "utils.h"
#include "engine.h"

#ifdef Q_OS_MACX
#include <CoreFoundation/CoreFoundation.h>
#endif

/* global flags */
int glredraw = 1;		/* redraw is needed */





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
    char    configfile[MAX_STR_LEN] = "configs/mume.xml"; 
    int     default_local_port = 3000;
    int     default_remote_port = 4242;
    int     mud_emulation = 0;

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
    char    default_base_file[MAX_STR_LEN] = "database/mume.xml";   
    char    default_remote_host[MAX_STR_LEN] = "129.241.210.221";
#endif


  
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
    //engine_init();
    
    printf("Using config file : %s.\r\n", configfile);
    conf.load_config(resPath, configfile);
    
    
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
  
    printf("Loading the database ... \r\n");
    xml_readbase( conf.get_base_file() );
    printf("Successfuly loaded %i rooms!\n", Map.size());

    /* special init for the mud emulation */
    if (mud_emulation) {
      printf("Starting in MUD emulation mode...\r\n");
      
      Engine.set_prompt("-->");
      stacker.put(1);
      stacker.swap();
    }

    proxy.setMudEmulation( mud_emulation );


    printf("Starting renderer ...\n");

    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication app( argc, argv );

    if ( !QGLFormat::hasOpenGL() ) {
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }

    renderer_window = new MainWindow( 0 );

    QGLFormat f;
    f.setDoubleBuffer( TRUE );
    f.setDirectRendering( TRUE );
    f.setRgba( TRUE );
    f.setDepth( TRUE );

    QGLFormat::setDefaultFormat( f );


    renderer_window->show();

    proxy.init();
    proxy.start();
    
    return app.exec();
}
