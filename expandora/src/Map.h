#ifndef ROOMS_H 
#define ROOMS_H 

#include <vector>
#include "defines.h"
#include "CRoom.h"
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
    vector<CRoom *> rooms;

    
    CSquare(int leftx, int lefty, int rightx, int righty);
    CSquare();
    ~CSquare();

    /* mode == SquareType */
    int         get_mode(CRoom *room);
    int         get_mode(int x, int y);
    bool        to_be_passed();
    bool        is_inside(CRoom *room);  
    
    void        add_subsquare_by_mode(int mode);
    void        add_room_by_mode(CRoom *room, int mode);
        
    void        add(CRoom *room);
    void        remove(CRoom *room);
};

class CPlane {
    /* levels/planes. each plane stores a tree of CSquare type and its z coordinate */
public:
    int         z;

    CSquare     *squares;
    CPlane      *next;

    CPlane();
    ~CPlane();
    CPlane(CRoom *room);
};

class roommanager {
public:
  vector<CRoom *> rooms;   		/* rooms */
  CRoom *ids[MAX_ROOMS];	/* array of pointers */

  
  unsigned int size()  { return rooms.size(); }
  unsigned int next_free; 	/* next free id */

  unsigned int oneway_room_id;

  /* plane support */  
  CPlane        *planes;        /* planes/levels of the map, sorted by the Z coordinate, lower at first */
  void          add_to_plane(CRoom *room);
  void          remove_from_plane(CRoom *room);
  void          expand_plane(CPlane *plane, CRoom *room);

  roommanager();
  void init();
  void reinit();			/* reinitializer/utilizer */
  
  void addroom_nonsorted(CRoom *room);   /* use only at loading */
  void addroom(CRoom *room);
  
  
  CRoom * getroom(unsigned int id); /* get room by id */
  char *getname(unsigned int id);
  
  
    
  int try_merge_rooms(CRoom *room, CRoom *copy, int j);
    


  void fixfree();
  
  
  void delete_room(CRoom *r, int mode);  /* user interface function */
  void small_delete_room(CRoom *r);  /* user interface function */
};

extern class roommanager Map;/* room manager */

#endif

