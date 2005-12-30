#ifndef ROOMS_H 
#define ROOMS_H 

#include <vector>
#include "defines.h"
#include "croom.h"
using namespace std;


#define MAX_ROOMS       30000		/* maximal amount of rooms */

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
    vector<Croom *> rooms;

    
    CSquare(int leftx, int lefty, int rightx, int righty);
    CSquare();
    ~CSquare();

    /* mode == SquareType */
    int         get_mode(Croom *room);
    int         get_mode(int x, int y);
    bool        to_be_passed();
    bool        is_inside(Croom *room);  
    
    void        add_subsquare_by_mode(int mode);
    void        add_room_by_mode(Croom *room, int mode);
        
    void        add(Croom *room);
    void        remove(Croom *room);
};

class CPlane {
    /* levels/planes. each plane stores a tree of CSquare type and its z coordinate */
public:
    int         z;

    CSquare     *squares;
    CPlane      *next;

    CPlane();
    ~CPlane();
    CPlane(Croom *room);
};

class roommanager {
public:
  vector<Croom *> rooms;   		/* rooms */
  Croom *ids[MAX_ROOMS];	/* array of pointers */

  
  unsigned int size()  { return rooms.size(); }
  unsigned int next_free; 	/* next free id */

  unsigned int oneway_room_id;

  /* plane support */  
  CPlane        *planes;        /* planes/levels of the map, sorted by the Z coordinate, lower at first */
  void          add_to_plane(Croom *room);
  void          remove_from_plane(Croom *room);
  void          expand_plane(CPlane *plane, Croom *room);

  roommanager();
  void init();
  void reinit();			/* reinitializer/utilizer */
  
  void addroom_nonsorted(Croom *room);   /* use only at loading */
  void addroom(Croom *room);
  
  
  Croom * getroom(unsigned int id); /* get room by id */
  char *getname(unsigned int id);
  
  
    
  int try_merge_rooms(Croom *room, Croom *copy, int j);
    


  void fixfree();
  
  
  void delete_room(Croom *r, int mode);  /* user interface function */
  void small_delete_room(Croom *r);  /* user interface function */
};

extern class roommanager Map;/* room manager */

#endif


#ifdef DMALLOC
#include <dmalloc.h>
#endif
