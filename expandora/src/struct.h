
#define TIMESTAMP_FORMAT "dd.MM.yyyy - hh:mm:ss"
#define TIMESTAMP_LEN    21

struct Troom {
  unsigned int id; 		/* identifier */
  char *name; 			/* POINTER to the room name */
  char *note; 			/* note, if needed, additional info etc */
  char *desc;			/* descrition */
  struct room_sectors_data *sector; /* terrain sector */ 		
                                /* no need to free this one */
  
  unsigned int exits[6];	/* 0 if no connection, id if there is any */ 
  char *doors[6];		/* if the door is secret */

  char timestamp[ TIMESTAMP_LEN + 1 ];
  
  int x, y, z;		/* coordinates on our map */
  struct Troom *prev, *next;    /* list organisation */
};

struct room_flag_data {
  char *name;
  char *xml_name;
  unsigned int flag;
};


struct TFailureData {
  char *pattern;                /* pattern */
  char *data;                   /* data for event structure */
  char type;                    /* failure type */
  struct TFailureData *next;
};

struct event_types_type {
  char  *name;
  char  type;
  char  group;
};

struct engine_flags_type {
  char done;                    /* finish */       
  char exits_check;             /* apply exits check to stacks */
  char terrain_check;           /* apply terrain check to stacks */
  char addingroom;              /* adding room right now */
  char resync;                  /* do full resync */
  char mapping;                 /* mapping is On/OFF */
  char gettingfirstroom;        /* getting the very first room in base */
  char autorefresh;             /* automatic room desc refresh */
  char automerge;               /* automatic twins merging based on roomdesc */
  char angrylinker;             /* automatic linking based on coordinates */
  
  
  char redraw;  
  
  char last_roomname[MAX_STR_LEN];
  char last_desc[MAX_DATA_LEN];
  char last_exits[MAX_DATA_LEN];
  char last_terrain;
  
};

extern struct engine_flags_type engine_flags;

#ifdef DMALLOC
#include <dmalloc.h>
#endif
