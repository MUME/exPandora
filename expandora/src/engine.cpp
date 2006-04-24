/* rewritten analyzer engine. */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <QMutex>
#include <QTime>

#include "configurator.h"
#include "defines.h"
#include "stacks.h"
#include "dispatch.h"
#include "exits.h"
#include "forwarder.h"
#include "utils.h"
#include "engine.h"
#include "Map.h"
#include "tree.h"


class CEngine Engine;


/*---------------- * MAPPING OFF ---------------------------- */
void CEngine::mappingoff()
{
    if (mapping) {
        send_to_user("--[ Mapping is now OFF!\r\n");
        mapping = 0;
        addingroom = 0;
    }    
}
/*---------------- * MAPPING OFF ---------------------------- */

/*---------------- * APPLY_ROOMNAME ---------------------------- */
void CEngine::applyName()
{
  CRoom *room;
  unsigned int i;
  int match;
  
  print_debug(DEBUG_ANALYZER, "in apply_roomname");

  /* set the environment flags and variables */
  setMgoto( false );    /* if we get a new roomname incoming, mgoto has to go away */
//  redraw  = 1;
  
  set_roomname(event.name);

  /* clear desc, exits and terrain info in engine state variables */
  set_desc("");
  set_exits("");
  set_terrain(0);
  
  if (addingroom) {
    // addedroom->name = strdup(r->data); /* doing this in try_dir */
    stacker.put(addedroom);
    return;
  }

  if (stacker.amount() == 0) 
    return;
  
  match = 0;
  
  for (i = 0; i < stacker.amount(); i++) {
    room = stacker.get(i);
    if (room->name == NULL) {
      printf("ERROR - room without a roomname!\r\n");
    }
    if ( (match = room->roomname_cmp(event.name)) >= 0) {
      stacker.put(room);
    }
  }
  
  if (stacker.next() == 1 && match > 0) {
    send_to_user("--[ not exact room match: %i errors.\r\n", match);
  }
}
/*---------------- * APPLY_ROOMNAME ---------------------------- */


/*---------------- * APPLY_DESC ---------------------------- */
void CEngine::applyDesc()
{
  CRoom *room;
  unsigned int i;
  int match;
  
  print_debug(DEBUG_ANALYZER, "in apply_desc");

  set_desc(event.desc);

  if (addingroom) {
    addedroom->desc = qstrdup((const char *) event.desc);
    
    stacker.put(addedroom);
    return;
  }

  match = 0;
  
  if (stacker.amount() == 0) {
      return;
  } else {
      for (i = 0; i < stacker.amount(); i++) {
          room = stacker.get(i);
	  if (room->desc == NULL) {
	    /* this room has no roomdesc, so we just skip it. */
	    continue;
	  }
          if ( (match = room->desc_cmp(event.desc)) >= 0) {
//              print_debug(DEBUG_ANALYZER, "found matching room");
              stacker.put(room);
          }
      }
  }
  
  if (stacker.next() == 1 && match > 0) {
    /* this means we have exactly one match */
    if (conf.get_autorefresh()) {
      send_to_user("--[ (AutoRefreshed) not exact room desc match: %i errors.\r\n", match);
      stacker.next_first()->refresh_desc(event.desc);  
    } else {
      send_to_user("--[ not exact room desc match: %i errors.\r\n", match);
    }
    
  }
}
/*---------------- * APPLY_DESC ---------------------------- */




/*---------------- * APPLY_EXITS ------------------------ */
void CEngine::applyExits()
{
  print_debug(DEBUG_ANALYZER, "in apply_exits");
  set_exits(event.exits);
  do_exits((const char *) event.desc);
}
/*---------------- * APPLY_EXITS --------------------------- */


/*---------------- * SWAP  ------------------------- */
void CEngine::swap()
{
    print_debug(DEBUG_ANALYZER, "in swap");
    stacker.swap();
    
    if (mapping && stacker.amount() != 1)
        mappingoff();
}
/*---------------- * SWAP  ------------------------- */


/*---------------- * RESYNC * ------------------------- */
void CEngine::resync()
{
  unsigned int j;
  TTree *n;
  
  mappingoff();
  
  print_debug(DEBUG_ANALYZER, "FULL RESYNC");
  n = NameMap.find_by_name(last_name);
  if (n != NULL)
    for (j = 0; j < n->ids.size(); j++) {
      if (last_name == Map.getname( n->ids[j] )) {
//        print_debug(DEBUG_ANALYZER, "Adding matches");
        stacker.put( n->ids[j] );
      } 
    }

  stacker.swap();
}

void CEngine::tryDir()
{
    int dir;
    unsigned int i;
    CRoom *room;
    CRoom *candidate;
    
    print_debug(DEBUG_ANALYZER, "in try_dir");
    dir = numbydir(event.dir[0]);
    if (dir == -1) {
        printf("Error in try_dir - faulty DIR given as input!\r\n");
        return;
    }
            
    if (stacker.amount() == 0) 
        return;
  
    for (i = 0; i < stacker.amount(); i++) {
        room = stacker.get(i);
        if (room->is_connected(dir)) {
            candidate = Map.getroom(room->exits[dir]);
            if  (testRoom(candidate) )
                stacker.put(candidate);
                
        
        } /*
        else {
            if (stacker.amount() == 1 && mapping && event.name != "") {
        
                send_to_user("--[ Adding new room!\n");
                
                Map.fixfree();	// making this call just for more safety - might remove 
        
                addedroom = new CRoom;
        
                addedroom->id = Map.next_free;
                addedroom->name = strdup((const char *)event.name);
                room->exits[dir] = addedroom->id;
                addedroom->exits[reversenum(dir)] = room->id;
        
                addedroom->x = room->x;
                addedroom->y = room->y;
                addedroom->z = room->z;
                if (dir == NORTH)	    addedroom->y += 2;
                if (dir == SOUTH)     addedroom->y -= 2;
                if (dir == EAST)      addedroom->x += 2;
                if (dir == WEST)      addedroom->x -= 2;
                if (dir == UP)	    addedroom->z += 2;
                if (dir == DOWN)      addedroom->z -= 2;
        
        
                addingroom = 1;	// for exits, decription and prompt, ends in prompt check 
                Map.addroom(addedroom);
                
                stacker.put(addedroom);
        
                return;
            }	
        
        }
            */
    }
  
  
  
}

bool CEngine::testRoom(CRoom *room) 
{
    if (event.blind)
        return true;
    if  (room->roomname_cmp(event.name) >= 0) 
        if (event.desc == "")
            return true;
        else if ( room->desc_cmp(event.desc) >= 0 ) 
            return true;    
    return false;
}

void CEngine::tryAllDirs()
{
    CRoom *room;
    CRoom *candidate;
    unsigned int i, j;
    
    print_debug(DEBUG_ANALYZER, "in try_all_dirs");
    mappingoff();
    
    if (stacker.amount() == 0) {
        return;
    }
    
    /* exits check if simply ignored for now */
    for (i = 0; i < stacker.amount(); i++) {
        room = stacker.get(i);
        for (j = 0; j <=5; j++) 
            if (room->is_connected(j) ) {
                /* test this room if it fits */
                candidate = Map.getroom(room->exits[j]);
                if (testRoom(candidate))
                    stacker.put(candidate);
            }
        /* check the room itself -> scout/look/examine support  */
        if  (testRoom(room))             
            stacker.put(room);
                            
    }
}


void CEngine::parse_event()
{
    last_name = event.name;
    last_desc = event.desc;
    last_exits = event.exits;
    
    if (event.name.indexOf("It is pitch black...") == 0)
        event.blind = true;
    if (event.dir =="")
        tryAllDirs();
    else 
        tryDir();        
        
    swap();
    
    if (stacker.amount() == 0)
        resync();
        
    toggle_renderer_reaction();
}



CEngine::CEngine()
{
}

CEngine::~CEngine()
{
 /* DESTRUCTOR */                   
}



void CEngine::exec()
{
  print_debug(DEBUG_ANALYZER, "in main cycle");
  QTime t;
  
  if (mud_emulation)
    return;

  t.start();
  
  done = false;
  while (!done) 
    run();
  
  print_debug(DEBUG_ANALYZER, "done. Time elapsed %d ms", t.elapsed());
}

void CEngine::run()
{

    printf("In Engune run().\r\n");
    if (Pipe.empty()) {
        done = true;
        return;
    }

    
    event = Pipe.dequeue();
    printf("----------------------------------\r\n");
    printf("Engine - event in queue (analyzing)\r\n");
    printf("Movement dir: %s\r\n", (const char *) event.dir);
    printf("Name: %s\r\n", (const char *) event.name);
    printf("Desc: %s\r\n", (const char *) event.desc);
    printf("Exits: %s\r\n", (const char *) event.exits);
    printf("----------------------------------\r\n");
    parse_event();
    
    done = true;
    return;        
}

/* load config and init engine */
void CEngine::engine_init()
{
     
  /* setting defaults */
  done =                   1;            
  addingroom =             0;
  mapping =                0;
  gettingfirstroom =       0;
  mgoto             =      0;

  last_name.clear();
  last_desc.clear();
  last_terrain = 0;
  last_prompt.clear();
}

int CEngine::check_roomdesc()
{
    CRoom *r;
    unsigned int i;
    int j;

    print_debug(DEBUG_ANALYZER, "Room-desc check for new room");
    
    j = -1;

    if (addedroom == NULL) {
        printf("analyzer: Failure in check_desc function!\n");
        return 0;
    }

    if (conf.get_automerge() == false) {
        printf("Analyzer: autodesc check if OFF - quiting this routine.\n");
        stacker.put(addedroom);
      
        return 0;
    }
    /* theory - new added room has only one exit dir defined - the one we came from */
    /* so if we find same looking (name, desc) room in base with the same undefined */
    /* exit as the defined exit in current room, we can merge them. */


    if (addedroom->name == NULL) {
        /* now thats sounds bad ... */
        Map.delete_room(addedroom, 0);
        printf("ERROR: in check_description() - empty roomname in new room.\r\n");
        return 0;
    }

        
    if (addedroom->desc == NULL) {
        send_to_user("--[Pandora: Error, empty roomdesc in new added room.\r\n");
        addedroom->desc=strdup("");
    }

    /* find the only defined exit in new room - the one we came from */
    for (i = 0; i <= 5; i++)
      if ( addedroom->is_connected(i) ) {
          j = i;
          break;
      }
    
    
    for (i = 0; i < Map.size(); i++) {
        r = Map.rooms[i];
        if (addedroom->id == r->id || r->desc == NULL || r->name == NULL) {
          continue;
        }
        
        /* in this case we do an exact match for both roomname and description */
        if (strcmp(addedroom->desc, r->desc) == 0) {
            if (strcmp(addedroom->name, r->name) == 0) {
              if (Map.try_merge_rooms(r, addedroom, j)) {
                send_to_user("--[Pandora: Twin rooms merged!\n");
                send_to_user(last_prompt);
                print_debug(DEBUG_ANALYZER, "Twins merged");
                addedroom = NULL;
                return 1;
              }
            }	
        }
    }
        
    /* if we are still here, then we didnt manage to merge the room */
    /* so put addedroom->id in stack */
    stacker.put(addedroom);
    return 0;
}


void CEngine::angrylinker(CRoom *r)
{
  CRoom *p;
  unsigned int i;
  CRoom *candidates[6];
  int distances[6];
  int z;
    
  if (!conf.get_angrylinker()) 
    return; 

  print_debug(DEBUG_ROOMS, "AngryLinker is called");

  
  if (r == NULL) {
    print_debug(DEBUG_ROOMS, "given room is NULL");
    return;
  }
  
  
  
  /* check if we have any undefined exits in thos room at all */
  for (i = 0; i <= 5; i++) 
    if (r->exits[i] == EXIT_UNDEFINED) {
      i = 1000;
      break;
    }
    
  if (i == 5) 
    return;     /* no need to try and link this room - there are no undefined exits */


  /* reset the data */
  for (i=0; i <= 5; i++) {
    distances[i] = 15000;
    candidates[i] = 0;
  }
  z = 0;  
  
  
  /* find the closest neighbours by coordinate */
  for (i = 0; i < Map.size(); i++) {
      p = Map.rooms[i];  
  
    /* z-axis: up and down exits */
    if (p->z != r->z) {
      
      if ((p->x != r->x) || (p->y != r->y))
        continue;
      
      /* up exit */
      if (p->z > r->z) {
        z = p->z - r->z;
        if (z < distances[UP]) {
          /* update */
          distances[UP] = z;
          candidates[UP] = p;
        }
      }

      /* DOWN exit */
      if (r->z > p->z) {
        z = r->z - p->z;
        if (z < distances[DOWN]) {
          /* update */
          distances[DOWN] = z;
          candidates[DOWN] = p;
        }
      }
      
    }
    /* done with z-axis */
      
    /* x-axis. */
    if ((p->y == r->y) && (p->z == r->z)) {
      
      if (p->x == r->x) 
        continue;                       /* all coordinates are the same - skip */
      
      /* EAST exit */
      if (p->x > r->x) {
        z = p->x - r->x;
        if (z < distances[EAST]) {
          /* update */
          distances[EAST] = z;
          candidates[EAST] = p;
        }
      }

      /* WEST exit */
      if (r->x > p->x) {
        z = r->x - p->x;
        if (z < distances[WEST]) {
          /* update */
          distances[WEST] = z;
          candidates[WEST] = p;
        }
      }
      
    }

    /* y-axis.  */
    if ((p->x == r->x) && (p->z == r->z)) {
      
      if (p->y == r->y) 
        continue;                       /* all coordinates are the same - skip */
      
      /* NORTH exit */
      if (p->y > r->y) {
        z = p->y - r->y;
        if (z < distances[NORTH]) {
          /* update */
          distances[NORTH] = z;
          candidates[NORTH] = p;
        }
      }

      /* SOUTH exit */
      if (r->y > p->y) {
        z = r->y - p->y;
        if (z < distances[SOUTH]) {
          /* update */
          distances[SOUTH] = z;
          candidates[SOUTH] = p;
        }
      }
      
    }

  
  }
  
    
  /* ok, now we have candidates for linking - lets check directions and connections*/
  for (i=0; i <= 5; i++) {
    if (r->exits[i] == EXIT_UNDEFINED && candidates[i] != NULL)
      if (candidates[i]->exits[ reversenum(i) ] == EXIT_UNDEFINED) {
        
        if (distances[ i ] <= 2) {
          print_debug(DEBUG_ROOMS, "we have a match for AngryLinker!");
          print_debug(DEBUG_ROOMS, "ID: %i to %i exit %s.", r->id, candidates[i]->id, exits[i] );
          
          /* ok, do the linking */
          candidates[ i ]->exits[ reversenum(i) ] = r->id;
          r->exits[ i ] = candidates[ i ]->id;
          print_debug(DEBUG_ROOMS, "Linked.", r->id, candidates[i]->id, exits[i] );
          
          send_to_user("--[ (AngryLinker) Linked exit %s with %s [%i].\r\n", 
                      exits[ i ], candidates[i]->name, candidates[i]->id);

        }
        
        
        
      }
  }
  
}


void CEngine::printstacks()
{
    char line[MAX_DATA_LEN];
    QByteArray s;

    send_to_user(" -----------------------------\n");

    sprintf(line,
	    "Conf: Mapping %s, AutoChecks [Desc %s, Exits %s, Terrain %s],\r\n"
            "      AutoRefresh settings %s (RName/Desc quotes %i/%i), \r\n"
            "      AngryLinker %s\r\n", 
            ON_OFF(mapping), ON_OFF(conf.get_automerge()), 
            ON_OFF(conf.get_exits_check() ), ON_OFF(conf.get_terrain_check() ),
            ON_OFF(conf.get_autorefresh() ), conf.get_name_quote(), conf.get_desc_quote(),
            ON_OFF(conf.get_angrylinker() )             );
    
    send_to_user(line);
    stacker.printstacks();
}

void CEngine::add_event(Event e)
{
    Pipe.enqueue(e); 
}

bool CEngine::empty()                      /* are pipes empty? */
{
    return Pipe.empty();
}

void CEngine::clear()
{
    Pipe.clear();
}

