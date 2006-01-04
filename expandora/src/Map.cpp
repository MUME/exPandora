/* Implementation of room manager of Pandora Project (c) Azazello 2003 */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <QDateTime>
#include <vector>
//using namespace std;

#include "defines.h"
#include "configurator.h"

#include "Map.h"
#include "utils.h"
#include "tree.h"
#include "stacks.h"
#include "dispatch.h"
#include "renderer.h"

#define MAX_SQUARE_SIZE         40
#define MAX_SQUARE_ROOMS        40

class roommanager Map;



/*------------ merge_rooms ------------------------- */
int roommanager::try_merge_rooms(CRoom *r, CRoom *copy, int j)
{
  int i;
  CRoom *p;

  if (j == -1) {
    /* oneway ?! */
    print_debug(DEBUG_ROOMS, "fixing one way in previous room, repointing at merged room");
    
    p = getroom(oneway_room_id);
    for (i = 0; i <= 5; i++)
        if (p->exits[i] == copy->id) 
            p->exits[i] = r->id;
    
    small_delete_room(copy);


    stacker.put(r);
    return 1;
  }
  if (r->exits[j] == EXIT_UNDEFINED) {
    r->exits[j] = copy->exits[j];
                      
    p = getroom(copy->exits[j]);
    if (p->exits[reversenum(j)] == copy->id)
        p->exits[reversenum(j)] = r->id;
        
    small_delete_room(copy);

    stacker.put(r);
    return 1;
  }
  return 0;
}

/* ------------ fixfree ------------- */
void roommanager::fixfree()
{
    unsigned int i;

    for (i = 1; i < MAX_ROOMS; i++)
	if (ids[i] == NULL) {
	    next_free = i;
	    return;
	}

    printf
	("roomer: error - no more space for rooms in ids[] array! reached limit\n");
    exit(1);
}

/* ------------ fixfree ENDS -------- */


/* ------------ getroom ------------- */
CRoom *roommanager::getroom(unsigned int id)
{
  return ids[id];
}

/* ----------- getroom ENDS --------- */

char *roommanager::getname(unsigned int id) 
{     
    if (ids[id])
        return (*(ids[id])).name;
    return NULL;
}

/* ------------ addroom --------------*/
void roommanager::addroom_nonsorted(CRoom *room)
{
  if (ids[room->id] != NULL) {
      printf
          ("Error while adding new element to database! This id already exists!\n");
      exit(1);
  }


  rooms.push_back(room);
  ids[room->id] = room;	/* add to the first array */
  NameMap.addname(room->name, room->id);	/* update name-searhing engine */

  fixfree();
  add_to_plane(room);
}

void roommanager::addroom(CRoom *room)
{
  addroom_nonsorted(room);
}
/* ------------ addroom ENDS ---------- */

/*------------- Constructor of the room manager ---------------*/
roommanager::roommanager()
{
    init();
}


void roommanager::init()
{
    next_free = 1;

    print_debug(DEBUG_ROOMS, "In roomer.init()");
  
    /* adding first (empty) root elements to the lists */
    rooms.clear();
    
    ids[0] = NULL;
    planes = NULL;
}

/*------------- Constructor of the room manager ENDS  ---------------*/

/* -------------- reinit ---------------*/
void roommanager::reinit()
{
    next_free = 1;
    {
        CPlane *p, *next;
        
        printf("Resetting Cplane structures ... \r\n");
        p = planes;
        while (p) {
            next = p->next;
            delete p;
            p = next;
        }
        planes = NULL;
    }

    memset(ids, 0, MAX_ROOMS * sizeof (CRoom *) );        
/*
    for (i = 0; i < MAX_ROOMS; i++)
      ids[i] = NULL;
    ids[0] = rooms;
*/
    
    rooms.clear();     
    NameMap.reinit();
}

/* -------------- reinit ENDS --------- */

/* ------------ delete_room --------- */
/* mode 0 - remove all links in other rooms together with exits and doors */
/* mode 1 - keeps the doors and exits in other rooms, but mark them as undefined */
void roommanager::delete_room(CRoom *r, int mode)
{
    int k;
    unsigned int i;
    
    
    if (r->id == 1) {
	printf("Cant delete base room!\n");
	return;
    }

    /* have to do this because of possible oneways leading in */
    for (i = 0; i < rooms.size(); i++) 
	for (k = 0; k <= 5; k++)
	    if (rooms[i]->exits[k] == r->id) {
                if (mode == 0) {
		    rooms[i]->exits[k] = 0;
                    if (rooms[i]->doors[k] != NULL) {
                        delete rooms[i]->doors[k];
                        rooms[i]->doors[k] = NULL;
                    }
                } else if (mode == 1) {
                    rooms[i]->exits[k] = EXIT_UNDEFINED;
                }
	    }

    small_delete_room(r);
}

/* --------- _delete_room ENDS --------- */

/* ------------ small_delete_room --------- */
void roommanager::small_delete_room(CRoom *r)
{
    if (r->id == 1) {
	printf("ERROR (!!): Attempted to delete the base room!\n");
	return;
    }
    remove_from_plane(r);
    stacker.remove_room(r->id);
    
    vector<CRoom *>::iterator i;
    ids[r->id] = NULL;
    
    for (i = rooms.begin(); i != rooms.end(); i++)
        if ((*i)->id == r->id) {
            printf("Deleting the room from rooms vector.\r\n");
            i = rooms.erase(i);
            break;
        }
    
    delete r;    
    fixfree();
}
/* --------- small_delete_room ENDS --------- */


/* -------------------------------------------------------------------------*/
/*  Planes (CPlane) and Square-tree (CSquare) implementation is below       */
/* -------------------------------------------------------------------------*/
CSquare::CSquare()
{
    subsquares[Left_Upper] = NULL;
    subsquares[Right_Upper] = NULL;
    subsquares[Left_Lower] = NULL;
    subsquares[Right_Lower] = NULL;
    
    leftx =  -MAX_SQUARE_SIZE/2;      
    lefty =   MAX_SQUARE_SIZE/2;      
    rightx =  MAX_SQUARE_SIZE/2;
    righty = -MAX_SQUARE_SIZE/2;
    centerx = 0;
    centery = 0;
}

CSquare::~CSquare()
{
    if (subsquares[0]) {
        delete subsquares[0];
    }
    if (subsquares[1]) {
        delete subsquares[1];
    }
    if (subsquares[2]) {
        delete subsquares[2];
    }
    if (subsquares[3]) {
        delete subsquares[3];
    }
}


CSquare::CSquare(int lx, int ly, int rx, int ry)
{
    subsquares[Left_Upper] = NULL;
    subsquares[Right_Upper] = NULL;
    subsquares[Left_Lower] = NULL;
    subsquares[Right_Lower] = NULL;

    leftx = lx;
    lefty = ly;
    rightx = rx;
    righty = ry;
    
    centerx = leftx + (rightx - leftx) / 2;
    centery = righty + (lefty - righty) / 2;
}


void CSquare::add_subsquare_by_mode(int mode)
{
    switch (mode)
    {
            case Left_Upper : 
                    subsquares[Left_Upper] =  new CSquare(leftx, lefty, centerx, centery);
                    break;
            case Right_Upper :
                    subsquares[Right_Upper] = new CSquare(centerx, lefty, rightx, centery);
                    break;
            case Left_Lower:
                    subsquares[Left_Lower] =  new CSquare(leftx, centery, centerx, righty);
                    break;
            case Right_Lower:
                    subsquares[Right_Lower] = new CSquare(centerx, centery, rightx, righty);
                    break;
    }
}


void CSquare::add_room_by_mode(CRoom *room, int mode)
{
    mode = get_mode(room);
    if (subsquares[mode] == NULL) 
        this->add_subsquare_by_mode(mode);
        
    subsquares[ mode ]->add(room);
}


bool CSquare::to_be_passed()
{
    if (!rooms.empty())
        return false;
    
    /* if we have ANY children, the node has to be passed */
    if (subsquares[0] || subsquares[1] || subsquares[2] || subsquares[3] )
        return true;
    
    return false;
}

void CSquare::add(CRoom *room)
{
    CRoom *r;
    unsigned int i;
    
    if (to_be_passed() ) 
    {
        add_room_by_mode(room, get_mode(room) );
        return;
    }
    
    if (( rooms.size() < MAX_SQUARE_ROOMS) && ( (rightx - leftx) < MAX_SQUARE_SIZE) ) 
    {
        rooms.push_back(room);
        return;
    } else {
        
        for (i=0; i < rooms.size(); i++) {
            r = rooms[i] ;
            add_room_by_mode(r, get_mode(r) );
        }
        rooms.clear();
        rooms.resize(0);
        add_room_by_mode(room, get_mode(room) );
    }
}

void roommanager::remove_from_plane(CRoom *room)
{
    CPlane *p;
    
    p = planes;
    while (p->z != room->z) {
        if (!p) {
            printf(" FATAL ERROR. remove_fromplane() the given has impossible Z coordinate!\r\n");
            return;     /* no idea what happens next ... */
        }
        p = p->next;
    }

    p->squares->remove(room);
}


void CSquare::remove(CRoom *room)
{
    CSquare *p;
    vector<CRoom *>::iterator i;
    
    p = this;
    while (p) {
        if (!p->to_be_passed()) {       
            /* just for check */
            for (i=p->rooms.begin(); i != p->rooms.end(); ++i) {
                if (room->id == ((*i)->id)) {
                    i = p->rooms.erase(i);
                    return;
                }
            }
        }
        p = p->subsquares[ p->get_mode(room) ];
    }
}

int CSquare::get_mode(CRoom *room)
{
    return get_mode(room->x, room->y);
}

int CSquare::get_mode(int x, int y)
{
    if (this->centerx > x) {
        if (this->centery > y) {
            return Left_Lower;
        } else {
            return Left_Upper;
        }
    } else {
        if (this->centery > y) {
            return Right_Lower;
        } else {
            return Right_Upper;
        }
    }
}

bool CSquare::is_inside(CRoom *room)
{
    /* note : right and lower borders are inclusive */
    
    if ((leftx <  room->x) && (rightx >= room->x) &&  
        (lefty >  room->y) && (righty <= room->y)    )
        return true;    /* yes the room is inside this square then */
    
    return false; /* else its not */
}

/* CPlane classes implementation */

CPlane::CPlane()
{
    z = 0;
    next = NULL;
    squares = NULL;
}

CPlane::~CPlane()
{
    delete squares;
}

CPlane::CPlane(CRoom *room)
{
    next = NULL;

    z = room->z;

    
    squares = new CSquare(  room->x - ( MAX_SQUARE_SIZE - 1) / 2,  
                            room->y + ( MAX_SQUARE_SIZE - 1 ) / 2,
                            room->x + ( MAX_SQUARE_SIZE - 1 ) / 2,
                            room->y - ( MAX_SQUARE_SIZE - 1 ) / 2);

/*    printf("Created a new square lx ly: %i %i, rx ry: %i %i, cx cy: %i %i, for room x y: %i %i\r\n",
            squares->leftx, squares->lefty, squares->rightx, squares->righty, 
            squares->centerx, squares->centery, room->x, room->y);
*/
    
    squares->rooms.push_back(room);
}

void  roommanager::add_to_plane(CRoom *room)
{
    CPlane *p, *prev, *tmp;

    
    
    if (planes == NULL) {
        planes = new CPlane(room);
        return;
    }
    
    p = planes;
    prev = NULL;
    while (p) {
        if (room->z < p->z) {
            tmp = new CPlane(room);
            tmp->next = p;
            if (prev)
                prev->next = tmp;
            else 
                planes = tmp;
            return;
        }
        /* existing plane with already set borders */
        if (room->z == p->z) {
            expand_plane(p, room);
            return;
        }
        prev = p;
        p = p->next;
    }
    
    /* else .. this is a plane with highest yet found Z coordinate */
    /* we add it to the end of the list */
    prev->next = new CPlane(room);
}


void roommanager::expand_plane(CPlane *plane, CRoom *room)
{
    CSquare *p, *new_root = NULL;
    int size;
    
    p = plane->squares;
    
/*    printf("Preparing to expand the plane lx ly: %i %i, rx ry: %i %i, cx cy: %i %i, for room x y: %i %i\r\n",
            p->leftx, p->lefty, p->rightx, p->righty, p->centerx, p->centery, room->x, room->y);
*/    
    while ( p->is_inside(room) != true ) {
        /* plane fork/expanding cycle */
        
        size = p->rightx - p->leftx;
        
        switch ( p->get_mode(room) )
        {
            case  Left_Upper:
                new_root = new CSquare(p->leftx - size, p->lefty + size, p->rightx, p->righty);
                new_root->subsquares[ Right_Lower ] = p;
                break;
            case  Right_Upper:
                new_root = new CSquare(p->leftx,  p->lefty + size, p->rightx + size, p->righty);
                new_root->subsquares[ Left_Lower ] = p;
                break;
            case  Right_Lower:
                new_root = new CSquare(p->leftx,  p->lefty, p->rightx + size, p->righty - size);
                new_root->subsquares[ Left_Upper ] = p;
                break;
            case  Left_Lower:
                new_root = new CSquare(p->leftx - size,  p->lefty, p->rightx , p->righty - size);
                new_root->subsquares[ Right_Upper ] = p;
                break;
        }
        
        p = new_root;
    }    

/*    printf("Ok, it fits. Adding!\r\n");
*/    
    p->add(room);
    plane->squares = p;
}
