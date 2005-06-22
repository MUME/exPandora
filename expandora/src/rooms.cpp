/* Implementation of room manager of Pandora Project (c) Azazello 2003 */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <qdatetime.h>

#include "defines.h"
#include "struct.h"

#include "rooms.h"
#include "utils.h"
#include "tree.h"
#include "stacks.h"
#include "dispatch.h"
#include "renderer.h"

class roommanager roomer;
struct room_sectors_data *room_sectors = NULL;
struct room_sectors_data *death_terrain = NULL;


const struct room_flag_data room_flags[] = {
  {"undefined", "UNDEFINED", EXIT_UNDEFINED},
  {"death", "DEATH", EXIT_DEATH},
    
  {NULL, NULL, EXIT_UNDEFINED}
};


void reset_room(struct Troom *r)
{				/* Only for new rooms */
    int i;

    r->id = 0;
    r->name = NULL;
    r->note = NULL;
    r->desc = NULL;
    r->x = 0;
    r->y = 0;
    r->z = 0;
    r->sector = 0;
  
    memset(r->timestamp, 0, TIMESTAMP_LEN);
  
  
    r->next = NULL;
    r->prev = NULL;
    for (i = 0; i <= 5; i++) {
	r->exits[i] = 0;
	r->doors[i] = NULL;
    }

}

void roommanager::add_terrain(struct Troom *r, char terrain)
{
  struct room_sectors_data *p;

  
  p = room_sectors;
  while (p) {
    if (terrain == p->pattern) {
      r->sector = p;
      print_debug(DEBUG_ANALYZER, "Terrain %s matches", p->desc);
      
      return;
    }
    p = p->next;
  }
}


/* database integrity check */
void roommanager::database_integrity_check(char *entry_point)
{
  struct Troom *r;
 
  printf("%s: Database integrity check ...", entry_point);
  
  r = roomer.getroom(0);
  r = r->next;
  while (r != NULL) {
    if (getroom(r->id) == NULL) {
      printf("\r\nDATABASE CHECK: ID %i, is no identified.\r\n", r->id);
      exit(1);
    }
    
    if (r->name == NULL) {
      printf("\r\nDATABASE CHECK: Room %i has no room name.\r\n", r->id);
      exit(1);
    }

    if (r->desc == NULL) {
      printf("\r\nDATABASE CHECK: Room %i has no room description.\r\n", r->id);
    }
    
    
    r = r->next;
  }
  
  printf("Ok.\r\n");
  
}


void roommanager::room_modified(struct Troom *r)
{
  modified = 1;
  if (r != NULL)
    update_timestamp(r);
}

/* implementation of desc comparison - simple strcmp at this moment */
void roommanager::refresh_desc(unsigned int id, char *newdesc)
{
  struct Troom *r;
      
  r = roomer.getroom(id);
  free(r->desc);
  r->desc = strdup(newdesc);
}

void roommanager::refresh_roomname(unsigned int id, char *newname)
{
  struct Troom *r;

  r = roomer.getroom(id);

  namer.delete_item(r->name, id);
  free(r->name);
  r->name = strdup(newname);
  namer.addname(newname, id);
}


void roommanager::refresh_terrain(unsigned int id, char terrain)
{
  struct Troom *r;
      
  r = roomer.getroom(id);
  add_terrain(r, terrain);
  
  print_debug(DEBUG_ROOMS, "terrain refresh. new terrain %c", terrain);
  /* hrmm */
      
}


int roommanager::desc_cmp(struct Troom *r, char *desc)
{ 
  return comparator.strcmp_desc(desc, r->desc);
}

int roommanager::roomname_cmp(struct Troom *r, char *name)
{ 
  return comparator.strcmp_roomname(name, r->name);
}




/* --------------- check if exit in room is connected --------------- */
int roommanager::is_connected(struct Troom *r, int dir)
{
  if ((r->exits[dir] == EXIT_UNDEFINED) || (r->exits[dir] == EXIT_DEATH))
    return 0;
  if (r->exits[dir] > 0)
    return 1;

  return 0;
}

/* ------------------------ add_door() ------------------------*/
int roommanager::add_door(struct Troom *r, char *door, int dir)
{

  if (r->exits[dir] == 0) {
    r->exits[dir] = EXIT_UNDEFINED;
  }
    
  if (r->doors[dir] != NULL) 
    free(r->doors[dir]);

  r->doors[dir] = strdup(door);
  
  room_modified(r);
  
  return 1;
}

/* ------------------------ remove_door() ------------------------*/
void roommanager::remove_door(struct Troom *r, int dir)
{

  if (r->doors[dir] != NULL) {
    free(r->doors[dir]);
    r->doors[dir] = NULL;
  }
  
  room_modified(r);
}


/*------------ merge_rooms ------------------------- */
int roommanager::try_merge_rooms(struct Troom *r, struct Troom *copy, int j)
{
  int i;
  struct Troom *p;

  if (j == -1) {
    /* oneway ?! */
    print_debug(DEBUG_ROOMS, "fixing one way in previous room, repointing at merged room");
    
    p = roomer.getroom(roomer.oneway_room_id);
    for (i = 0; i <= 5; i++)
        if (p->exits[i] == copy->id) 
            p->exits[i] = r->id;
    
    roomer.small_delete_room(copy);


    stacker.put(r->id);
    return 1;
  }
  if (r->exits[j] == EXIT_UNDEFINED) {
    r->exits[j] = copy->exits[j];
                      
    p = roomer.getroom(copy->exits[j]);
    if (p->exits[reversenum(j)] == copy->id)
        p->exits[reversenum(j)] = r->id;
        
    roomer.small_delete_room(copy);

    stacker.put(r->id);
    return 1;
  }
  return 0;
}



/* ----------- findrooms ---------------------- */
struct Ttree *roommanager::findrooms(char *name)
{
    return namer.find_by_name(name);
}


void roommanager::changenote(char *note, struct Troom *p)
{
  if (p->note != NULL)
    free(p->note);
  p->note = strdup(note);
  
  room_modified(p);
}


/* this is outdated and unused code - needs to be fixed according to */
/* tree search engine, else usage will corrupt the code */
void roommanager::changename(char *name, struct Troom *p)
{
  
  printf ("ERROR - ATTEMPTED TO USE CHANGENAME()");
  return;
  
  if (p->name != NULL)
    free(p->name);
  p->name = strdup(name);
  
  room_modified(p);
}

/* ------------ fixfree ------------- */
void roommanager::fixfree()
{
    unsigned int i;

    for (i = 0; i < MAX_ROOMS; i++)
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
struct Troom *roommanager::getroom(unsigned int id)
{
  return ids[id];
}

/* ----------- getroom ENDS --------- */

/* ------------ addroom --------------*/
void roommanager::addroom_nonsorted(struct Troom *room)
{
  
  if (ids[room->id] != NULL) {
      printf
          ("Error while adding new element to database! This id already exists!\n");
      exit(1);
  }


  room->prev = rooms;		/* add to list of rooms */
  room->next = rooms->next;
  if (rooms->next != NULL)
    rooms->next->prev = room;
  rooms->next = room;

  ids[room->id] = room;	/* add to the first array */

  namer.addname(room->name, room->id);	/* update name-searhing engine */

  if (room->timestamp[0] == 0) 
    update_timestamp(room);
  
  
  amount++;
  fixfree();
  add_to_plane(room);
}

void roommanager::addroom(struct Troom *room)
{
  addroom_nonsorted(room);
}
/* ------------ addroom ENDS ---------- */



void roommanager::update_timestamp(struct Troom *r)
{
  QDateTime t;
  QString s;

  if (r == NULL)
    return;
  
  t = t.currentDateTime();
  s = t.toString(TIMESTAMP_FORMAT);
  
  
  strncpy(r->timestamp, (const char *) s.ascii(), TIMESTAMP_LEN);
  r->timestamp[TIMESTAMP_LEN] = 0;
  
}

/*------------- Constructor of the room manager ---------------*/
roommanager::roommanager()
{
    init();
}


void roommanager::init()
{

    amount = 0;
    next_free = 1;

    print_debug(DEBUG_ROOMS, "In roomer.init()");
  
    /* adding first (empty) root elements to the lists */
    rooms = new Troom;
    reset_room(rooms);

    ids[0] = rooms;
    planes = NULL;
    
}

/*------------- Constructor of the room manager ENDS  ---------------*/

/* -------------- reinit ---------------*/
void roommanager::reinit()
{
    struct Troom *p, *p2;
    unsigned int i;
    
    amount = 0;
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

        
    for (i = 0; i < MAX_ROOMS; i++)
      ids[i] = NULL;
    ids[0] = rooms;

    
    p = rooms->next;
    while (p != NULL) {
	p2 = p->next;
      
	if (p->note != NULL) 
          free(p->note);
	if (p->name != NULL)
	    free(p->name);
	if (p->desc != NULL)
	    free(p->desc);
	for (i = 0; i <= 5; i++)
	    if (p->doors[i] != NULL)
		free(p->doors[i]);
            
	delete p;

            
	p = p2;
    }
    rooms->next = NULL;
 
    namer.reinit();
}

/* -------------- reinit ENDS --------- */

/* ------------ delete_room --------- */
/* mode 0 - remove all links in other rooms together with exits and doors */
/* mode 1 - keeps the doors and exits in other rooms, but mark them as undefined */
void roommanager::delete_room(struct Troom *r, int mode)
{
    int k;
    struct Troom *p;

    if (r->id == 1) {
	printf("Cant delete base room!\n");
	return;
    }

    /* have to do it this way because of one-ways */
    p = getroom(0);		/* get base room */
    p = p->next;

    while (p != NULL) {
	for (k = 0; k <= 5; k++)
	    if (p->exits[k] == r->id) {
              if (mode == 0) {
		p->exits[k] = 0;
                if (p->doors[k] != NULL) {
                  free(p->doors[k]);
                  p->doors[k] = NULL;
                }
              }
              
              if (mode == 1) {
                p->exits[k] = EXIT_UNDEFINED;
              }
              
              
	    }
	p = p->next;
    }

    small_delete_room(r);
}

/* --------- _delete_room ENDS --------- */



/* ------------ small_delete_room --------- */
void roommanager::small_delete_room(struct Troom *r)
{
    int k;

    if (r->id == 1) {
	printf("ERROR (!!): Attempted to delete the base room!\n");
	return;
    }

    namer.delete_item(r->name, r->id);
    remove_from_plane(r);

    free(r->name);
    r->name = NULL;


    ids[r->id] = NULL;		/* THIS virtualy means the room is deleted in base */

    if (r->desc != NULL)
	free(r->desc);
    if (r->note != NULL)
	free(r->note);
    for (k = 0; k <= 5; k++)
	if (r->doors != NULL)
	    free(r->doors[k]);

    r->prev->next = r->next;	/* fix links */
    if (r->next != NULL)
	r->next->prev = r->prev;
    free(r);			/* now its gone */
    r = NULL;
    
    amount--;
    fixfree();
    room_modified(NULL);
}

/* --------- small_delete_room ENDS --------- */


/* --------- getname --------------*/
char *roommanager::getname(unsigned int id)
{
    Troom *r;

    r = getroom(id);
    if (r != NULL)
	return r->name;

    return NULL;
}

/* --------- getname --------------*/


void roommanager::setx(unsigned int id, int x)
{
  struct Troom *p;
    
  p = getroom(id);
  p->x = x;

  modified = 1;
}

void roommanager::sety(unsigned int id, int y)
{
  struct Troom *p;
    
  p = getroom(id);
  p->y = y;
  modified = 1;
  
}


void roommanager::setz(unsigned int id, int z)
{
  struct Troom *p;
    
  p = getroom(id);
  p->z = z;
  
  modified = 1;
}

/* ------------------------------ prints the given room --------------------*/
void roommanager::send_room(struct Troom *r)
{
    unsigned int i, pos;
    char line[MAX_STR_LEN];

    send_to_user(" Id: %i, Flags: %s, Coord: %i,%i,%i, Last updated: %s\r\n", r->id,
	    r->sector ? r->sector->desc : "NONE", r->x, r->y, r->z,
            r->timestamp[0] == 0 ? "No Info" : r->timestamp);
    send_to_user(" %s%s%s\n", roomname_start, r->name, roomname_end);

    line[0] = 0;
    pos = 0;
    if (!(mud_emulation && dispatcher.is_brief() ) ) {
      for (i = 0; i <= strlen(r->desc); i++)
	if (r->desc[i] == '|') {
	    line[pos] = 0;
	    send_to_user("%s\r\n", line);
	    line[0] = 0;
	    pos = 0;
	} else {
	    line[pos++] = r->desc[i];
	}
    }
    send_to_user(" note: %s\n", r->note);

    
    sprintf(line, "Doors:");
    for (i = 0; i <= 5; i++) {
      if (r->doors[i]) {
        sprintf(line + strlen(line), " %c: %s", dirbynum(i), r->doors[i]);
      }
    
    }
    send_to_user("%s\r\n", line);

    
    if (dispatcher.get_brief_mode() && mud_emulation) {
      sprintf(line, "Exits: ");
      for (i = 0; i <= 5; i++)
          if (r->exits[i] > 0) {
              if (r->exits[i] == EXIT_UNDEFINED) {
                  sprintf(line + strlen(line), " #%s#", exitnames[i]);
                  continue;
              }
              if (r->exits[i] == EXIT_DEATH) {
                  sprintf(line + strlen(line), " !%s!", exitnames[i]);
                  continue;
              }
              if (r->doors[i] != NULL) {
                  if (strcmp("exit", r->doors[i]) == 0) {
                      sprintf(line + strlen(line), " (%s)", exitnames[i]);
                  } else {
                      sprintf(line + strlen(line), " +%s+", exitnames[i]);
                  }
              } else {
                  sprintf(line + strlen(line), " %s", exitnames[i]);
              }
          }
    
      
      
    } else {
      
      line[0] = 0;
      sprintf(line, " exits:");
  
      for (i = 0; i <= 5; i++)
          if (r->exits[i] > 0) {
              if (r->exits[i] == EXIT_UNDEFINED) {
                  sprintf(line + strlen(line), " #%s#", exitnames[i]);
                  continue;
              }
              if (r->exits[i] == EXIT_DEATH) {
                  sprintf(line + strlen(line), " !%s!", exitnames[i]);
                  continue;
              }
              if (r->doors[i] != NULL) {
                  if (strcmp("exit", r->doors[i]) == 0) {
                      sprintf(line + strlen(line), " (%s)", exitnames[i]);
                  } else {
                      sprintf(line + strlen(line), " +%s+", exitnames[i]);
                  }
              } else {
                  sprintf(line + strlen(line), " %s", exitnames[i]);
              }
              sprintf(line + strlen(line), " -[to %i]-", r->exits[i]);
          }
      
      
    }

    send_to_user("%s\r\n", line);
}


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


void CSquare::add_room_by_mode(struct Troom *room, int mode)
{
    mode = get_mode(room);
    if (subsquares[mode] == NULL) 
        this->add_subsquare_by_mode(mode);
        
    subsquares[ mode ]->addroom(room);
}


bool CSquare::to_be_passed()
{
    if (ids.get_amount() > 0)
        return false;
    
    /* if we have ANY children, the node has to be passed */
    if (subsquares[0] || subsquares[1] || subsquares[2] || subsquares[3] )
        return true;
    
    return false;
}

void CSquare::addroom(struct Troom *room)
{
    struct Troom *r;
    int i;
        
    if (to_be_passed() ) {
        add_room_by_mode(room, get_mode(room) );
        return;
    }
    
    if (( ids.get_amount() < MAX_SQUARE_ROOMS) && ( (rightx - leftx) < MAX_SQUARE_SIZE) ) {
        ids.add(room->id);
        return;
    } else {
        for (i=0; i< ids.get_amount(); i++) {
            r = roomer.getroom( ids.get(i) );
            add_room_by_mode(r, get_mode(r) );
        }
        ids.removeall();

        add_room_by_mode(room, get_mode(room) );
    }
}

void roommanager::remove_from_plane(struct Troom *room)
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

    p->squares->removeroom(room);
}


void CSquare::removeroom(struct Troom *room)
{
    CSquare *p;
    int i;
    
    p = this;
    while (p) {
        if (!p->to_be_passed()) {
            /* just for check */
            i = p->ids.find(room->id);
            if (i == -1) {
                printf("FATAL ERROR! CSquare::removeroom given room is not in the supposed CSquare!\r\n");
                return;
            }
            p->ids.remove(room->id);
        }
        
        p = p->subsquares[ p->get_mode(room) ];
    }
}

int CSquare::get_mode(struct Troom *room)
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

bool CSquare::is_inside(struct Troom *room)
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

CPlane::CPlane(struct Troom *room)
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
    
    squares->ids.add(room->id);
}

void  roommanager::add_to_plane(struct Troom *room)
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


void roommanager::expand_plane(CPlane *plane, struct Troom *room)
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
    p->addroom(room);
    plane->squares = p;
}
