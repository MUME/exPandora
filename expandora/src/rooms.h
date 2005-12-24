#ifndef ROOMS_H 
#define ROOMS_H 

#include <vector>
#include "defines.h"
using namespace std;


#define MAX_ROOMS       30000		/* maximal amount of rooms */
#define EXIT_UNDEFINED  (MAX_ROOMS+1)
#define EXIT_DEATH      (MAX_ROOMS+2)
#define MAX_SQUARE_SIZE         40
#define MAX_SQUARE_ROOMS        40


  
extern const struct room_flag_data room_flags[];

void reset_room(struct Troom *r);	/* Only for new rooms */

enum SquareTypes {
    Left_Upper = 0,             
    Right_Upper = 1,            
    Left_Lower = 2,
    Right_Lower = 3
};


class CSquare {
public:
    /* subsquares */
    CSquare     *subsquares[4];
    /* coordinates of this square's left (upper) and right (lower) points */
    int         leftx, lefty;
    int         rightx, righty;
    int         centerx, centery;
    
    /* amount of rooms in this square, -1 for empty */
    vector<unsigned int> ids;

    
    CSquare(int leftx, int lefty, int rightx, int righty);
    CSquare();
    ~CSquare();

    /* mode == SquareType */
    int         get_mode(struct Troom *room);
    int         get_mode(int x, int y);
    bool        to_be_passed();
    bool        is_inside(struct Troom *room);  
    
    void        add_subsquare_by_mode(int mode);
    void        add_room_by_mode(struct Troom *room, int mode);
        
    void        addroom(struct Troom *room);
    void        removeroom(struct Troom *room);
};

class CPlane {
    /* levels/planes. each plane stores a tree of CSquare type and its z coordinate */
public:
    int         z;

    CSquare     *squares;
    CPlane      *next;

    CPlane();
    ~CPlane();
    CPlane(struct Troom *room);
};

class roommanager {
  /* structures */
  Troom *rooms;   		/* rooms */
  Troom *ids[MAX_ROOMS];	/* array of pointers */

  /* additional structures */
  int mark[EXIT_UNDEFINED+1];
    
  

public:
  unsigned int amount;		/* the amount of rooms in the base */
  unsigned int next_free; 	/* next free id */

  unsigned int oneway_room_id;

  /* plane support */  
  CPlane        *planes;        /* planes/levels of the map, sorted by the Z coordinate, lower at first */
  void          add_to_plane(struct Troom *room);
  void          remove_from_plane(struct Troom *room);
  void          expand_plane(CPlane *plane, struct Troom *room);

  roommanager();
  void init();
  void reinit();			/* reinitializer/utilizer */
  
  void addroom_nonsorted(struct Troom *room);   /* use only at loading */
  void addroom(struct Troom *room);
  void remove_door(struct Troom *room, int dir);
  
  struct Troom * getroom(unsigned int id); /* get room by id */
  char *getname(unsigned int id);
  
  void changenote(char *note, struct Troom *p);
    
  int try_merge_rooms(struct Troom *room, struct Troom *copy, int j);
    
  void update_timestamp(struct Troom *r);
    
  void room_modified(struct Troom *r);


  void fixfree();
  
  
  void delete_room(struct Troom *r, int mode);  /* user interface function */
  void small_delete_room(struct Troom *r);  /* user interface function */

  struct Ttree * findrooms(const char *name);  

  void setx(unsigned int id, int x);
  void sety(unsigned int id, int x);
  void setz(unsigned int id, int x);
  
  int is_connected(struct Troom *r, int dir);
  void send_room(struct Troom *r);
  
  int add_door(struct Troom *r, char *door, int dir);
  void add_terrain(struct Troom *r, char terrain);

  int desc_cmp(struct Troom *r, QByteArray desc);
  int roomname_cmp(struct Troom *r, QByteArray name);
  void refresh_desc(unsigned int id, QByteArray newdesc);
  void refresh_roomname(unsigned int id, QByteArray newname);
  void refresh_terrain(unsigned int id, char terrain);
  void refresh_note(unsigned int id, QByteArray note);
  void refresh_door(unsigned int id, char dir, QByteArray door);
  
  
  /* general check for database integrity */
  void database_integrity_check(char *entry_point);
};

extern class roommanager roomer;/* room manager */

#endif


#ifdef DMALLOC
#include <dmalloc.h>
#endif
