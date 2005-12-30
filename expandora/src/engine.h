#ifndef ENGINE_H
#define ENGINE_H

#include "croom.h"

struct engine_flags_type {
  char done;                    /* finish */       
  char addingroom;              /* adding room right now */
  char resync;                  /* do full resync */
  char mapping;                 /* mapping is On/OFF */
  char gettingfirstroom;        /* getting the very first room in base */
  
  
  char redraw;  
  
  QByteArray last_roomname;
  QByteArray last_desc;
  QByteArray last_exits;
  char last_terrain;
};

extern struct engine_flags_type engine_flags;

extern Croom *addedroom;	/* new room, contains new data if addinrroom==1 */


extern QMutex analyzer_mutex;
extern char    engine_config[];


void engine();                          /* main entry point */
void engine_init();
int engine_parse_command_line(char cause, char result, char *line);


int check_roomdesc();
void angrylinker(Croom *r);

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
