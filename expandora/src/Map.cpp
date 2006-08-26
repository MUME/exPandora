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

#define MAX_SQUARE_SIZE         40
#define MAX_SQUARE_ROOMS        40

class roommanager Map;



/*------------ merge_rooms ------------------------- */
int roommanager::tryMergeRooms(CRoom *r, CRoom *copy, int j)
{
  int i;
  CRoom *p;

  if (j == -1) {
    /* oneway ?! */
    print_debug(DEBUG_ROOMS, "fixing one way in previous room, repointing at merged room");
    
    p = getRoom(oneway_room_id);
    for (i = 0; i <= 5; i++)
        if (p->isExitLeadingTo(i, copy) == true) 
            p->setExit(i, r);
    
    smallDeleteRoom(copy);


    stacker.put(r);
    return 1;
  }
  if ( r->isExitUndefined(j) ) {
    r->setExit(j, copy->exits[j] );
                      
    p = copy->exits[j] ;
    if (p->isExitLeadingTo( reversenum(j), copy) == true)
        p->setExit( reversenum(j), r);
        
    smallDeleteRoom(copy);

    stacker.put(r);
    return 1;
  }
  return 0;
}

/* ------------ fixfree ------------- */
void roommanager::fixFreeRooms()
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
CRoom *roommanager::getRoom(unsigned int id)
{
  return ids[id];
}

/* ----------- getroom ENDS --------- */

QByteArray roommanager::getName(unsigned int id) 
{     
    if (ids[id])
        return (*(ids[id])).getName();
    return "";
}

/* ------------ addroom --------------*/
void roommanager::addRoomNonsorted(CRoom *room)
{
  if (ids[room->id] != NULL) {
      printf
          ("Error while adding new element to database! This id already exists!\n");
      exit(1);
  }


  rooms.push_back(room);
  ids[room->id] = room;	/* add to the first array */
  NameMap.addName(room->getName(), room->id);	/* update name-searhing engine */

  fixFreeRooms();
// FIX ME - ADD TO PLANE
}

void roommanager::addRoom(CRoom *room)
{
    addRoomNonsorted(room);
}
/* ------------ addroom ENDS ---------- */

/*------------- Constructor of the room manager ---------------*/
roommanager::roommanager()
{
    init();
}


void roommanager::init()
{
    printf("Roommanager INIT.\r\n");

    next_free = 1;

    print_debug(DEBUG_ROOMS, "In roomer.init()");
  
    /* adding first (empty) root elements to the lists */
    rooms.clear();
    regions.clear();
    
    
    
    CRegion *region = new CRegion;
    region->setName("default");
    
    regions.push_back(region);
    Engine.set_users_region(region);
    Engine.set_last_region(region);
    
    
    ids[0] = NULL;
}

/*------------- Constructor of the room manager ENDS  ---------------*/

CRegion *roommanager::getRegionByName(QByteArray name)
{
    CRegion    *region;
    for (int i=0; i < regions.size(); i++) {
        region = regions[i];
        if (region->getName() == name) 
            return region;
    }
    return NULL;
}

bool roommanager::addRegion(QByteArray name)
{
    CRegion    *region;
    
    if (getRegionByName(name) == false) {
        region = new CRegion();
        region->setName( name );
        regions.push_back(region);
        return true;
    } else {
        return false;
    }
    
}

void roommanager::addRegion(CRegion *reg)
{
    if (reg != NULL) 
        regions.push_back(reg);
}


void roommanager::sendRegionsList()
{
    CRegion    *region;
    send_to_user( "Present regions: \r\n");
    for (int i=0; i < regions.size(); i++) {
        region = regions[i];
        send_to_user("  %s\r\n", (const char *) region->getName() );
    }
    

}

QList<CRegion *> roommanager::getAllRegions()
{
    return regions;
}


/* -------------- reinit ---------------*/
void roommanager::reinit()
{
    next_free = 1;

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
void roommanager::deleteRoom(CRoom *r, int mode)
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
	    if (rooms[i]->isExitLeadingTo(k, r) == true) {
                if (mode == 0) {
                    rooms[i]->removeExit(k);
                } else if (mode == 1) {
                    rooms[i]->setExitUndefined(k);
                }
	    }

    smallDeleteRoom(r);
}

/* --------- _delete_room ENDS --------- */

/* ------------ small_delete_room --------- */
void roommanager::smallDeleteRoom(CRoom *r)
{
    if (r->id == 1) {
	printf("ERROR (!!): Attempted to delete the base room!\n");
	return;
    }
    
    // TODO - remove the room from the SceneNode
    stacker.removeRoom(r->id);
    
    vector<CRoom *>::iterator i;
    ids[ r->id ] = NULL;
    
    for (i = rooms.begin(); i != rooms.end(); i++)
        if ((*i)->id == r->id ) {
            printf("Deleting the room from rooms vector.\r\n");
            i = rooms.erase(i);
            break;
        }
    
    delete r;    
    fixFreeRooms();
}
/* --------- small_delete_room ENDS --------- */


