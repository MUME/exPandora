#include <qgl.h>
#ifdef Q_OS_MACX
#include "cmath"
#endif



#define MAX_ROOMS       30000		/* maximal amount of rooms */
#define EXIT_UNDEFINED  (MAX_ROOMS+1)
#define EXIT_DEATH      (MAX_ROOMS+2)


/* room manager of the Pandora Project (c) Azazello 2003 */

struct room_sectors_data {
  char   *desc;             /* name of this flag */
  char   *filename;         /* appropriate texture's filename */
  char   pattern;           /* appropriate pattern */
  GLuint texture;          /* and texture handler for renderer */
  GLuint gllist;            /* OpenGL display list */
  struct room_sectors_data *next;
};

extern struct room_sectors_data *room_sectors;
extern struct room_sectors_data *death_terrain;
  
extern const struct room_flag_data room_flags[];
  



void reset_room(struct Troom *r);	/* Only for new rooms */


class roommanager {
  /* structures */
  Troom *rooms;   		/* rooms */
  Troom *ids[MAX_ROOMS];	/* array of pointers */

  /* additional structures */
  int check_arraya[2000];
  int mark[EXIT_UNDEFINED+1];
  int check_arrayb[2000];
    

public:
  unsigned int amount;		/* the amount of rooms in the base */
  unsigned int next_free; 	/* next free id */

  unsigned int oneway_room_id;

  unsigned int order[MAX_ROOMS];


  roommanager();
  void init();
  void reinit();			/* reinitializer/utilizer */
  
  void addroom_nonsorted(struct Troom *room);   /* use only at loading */
  void addroom(struct Troom *room);
  void remove_door(struct Troom *room, int dir);
  
  struct Troom * getroom(unsigned int id); /* get room by id */
  char *getname(unsigned int id);
  
  void changenote(char *note, struct Troom *p);
  void changename(char *name, struct Troom *p);
    
  int try_merge_rooms(struct Troom *room, struct Troom *copy, int j);
    
  void update_timestamp(struct Troom *r);
    
  void room_modified(struct Troom *r);


  void fixfree();
  void resort_rooms();
  
  
  void delete_room(struct Troom *r, int mode);  /* user interface function */
  void small_delete_room(struct Troom *r);  /* user interface function */
  void integrity_check();

  struct Ttree * findrooms(char *name);  

  void setx(unsigned int id, int x);
  void sety(unsigned int id, int x);
  void setz(unsigned int id, int x);
  
  int is_connected(struct Troom *r, int dir);
  void send_room(struct Troom *r);
  
  int add_door(struct Troom *r, char *door, int dir);
  void add_terrain(struct Troom *r, char terrain);

  int desc_cmp(struct Troom *r, char *desc);
  int roomname_cmp(struct Troom *r, char *name);
  void refresh_desc(unsigned int id, char *newdesc);
  void refresh_roomname(unsigned int id, char *newname);
  void refresh_terrain(unsigned int id, char terrain);
  
  /* general check for database integrity */
  void database_integrity_check(char *entry_point);
};

extern class roommanager roomer;/* room manager */

#ifdef DMALLOC
#include <dmalloc.h>
#endif
