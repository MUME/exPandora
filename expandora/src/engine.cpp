/* rewritten analyzer engine. */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <QMutex>
#include <QTime>

#include "configurator.h"
#include "defines.h"
#include "event.h"
#include "stacks.h"
#include "dispatch.h"
#include "exits.h"
#include "forwarder.h"
#include "utils.h"
#include "engine.h"
#include "rooms.h"
#include "tree.h"
#include "renderer.h"


/* ------------------------------- defines --------------------------------- */

#define MAX_CODE_LENGTH     20

#define ECMD(name)  void name()

/* ---------------------- structures and variables ------------------------- */

/* general mutex that blocks analyzer */
QMutex analyzer_mutex;

Croom *addedroom;	/* new room, contains new data is addinrroom==1 */

/* inner flags and variables */
struct engine_flags_type engine_flags;


struct engine_command_type {
  char *name;
  void (*function) ();
};


struct engine_program_entry {
  char cause;
  char result;
  int len;                      /* code length */
  int code[MAX_CODE_LENGTH];
  struct engine_program_entry *next;
};

ECMD(engine_command_redraw);
ECMD(engine_command_swap);
ECMD(engine_command_resync);
ECMD(engine_command_rremove);
ECMD(engine_command_cremove);
ECMD(engine_command_try_dir);
ECMD(engine_command_try_all_dirs);
ECMD(engine_command_done);
ECMD(engine_command_apply_roomname);
ECMD(engine_command_apply_desc);
ECMD(engine_command_apply_exits);
ECMD(engine_command_apply_prompt);
ECMD(engine_command_mappingoff);


const struct engine_command_type engine_commands[] = {
  {"redraw",            engine_command_redraw},
  {"swap",              engine_command_swap},
  {"resync",            engine_command_resync},

  {"cremove",           engine_command_cremove},
  {"rremove",           engine_command_rremove},

  {"try_dir",           engine_command_try_dir},
  {"try_all_dirs",      engine_command_try_all_dirs},

  {"done",              engine_command_done},

  {"apply_roomname",    engine_command_apply_roomname},
  {"apply_desc",        engine_command_apply_desc},
  {"apply_exits",       engine_command_apply_exits},
  {"apply_prompt",      engine_command_apply_prompt},
  

  {"mappingoff",        engine_command_mappingoff},
  
  {NULL, NULL}
};

/* empty program by default */
struct engine_program_entry *engine_program = NULL;


/* ----------------------------- Functions --------------------------------- */

void copy_stacks();
void engine_run();

/* -------------------------- implementations ------------------------------ */ 



ECMD(engine_command_mappingoff)
{
  if (engine_flags.mapping) {
    print_debug(DEBUG_ANALYZER, "in mappingoff");
    send_to_user("--[ Mapping is now OFF!\r\n");
    engine_flags.mapping = 0;
    engine_flags.addingroom = 0;
  }    
}


ECMD(engine_command_apply_roomname)
{
  struct Tevent *r;
  Croom *room;
  unsigned int i;
  int match;
  
//  print_debug(DEBUG_ANALYZER, "in apply_roomname");
    
  r = Rtop->next;

  /* set the environment flags and variables */
  engine_flags.redraw  = 1;
  engine_flags.last_roomname = r->data;

  /* clear desc, exits and terrain info in engine state variables */
  engine_flags.last_desc = "";
  engine_flags.last_exits = "";
  engine_flags.last_terrain = 0;
  
  if (engine_flags.addingroom) {
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
    if ( (match = room->roomname_cmp(r->data)) >= 0) {
      stacker.put(room);
    }
  }
  
  if (stacker.next() == 1 && match > 0) {
    send_to_user("--[ not exact room name match: %i errors.\r\n", match);
  }
  
}

ECMD(engine_command_apply_desc)
{
  struct Tevent *r;
  Croom *room;
  unsigned int i;
  int match;
  
//  print_debug(DEBUG_ANALYZER, "in apply_desc");

  r = Rtop->next;


  engine_flags.last_desc = r->data;

  if (engine_flags.addingroom) {
    addedroom->desc = qstrdup((const char *) r->data);
    
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
          if ( (match = room->desc_cmp(r->data)) >= 0) {
//              print_debug(DEBUG_ANALYZER, "found matching room");
              stacker.put(room);
          }
      }
  }
  
  if (stacker.next() == 1 && match > 0) {
    /* this means we have exactly one match */
    if (conf.get_autorefresh()) {
      send_to_user("--[ (AutoRefreshed) not exact room desc match: %i errors.\r\n", match);
      stacker.next_first()->refresh_desc(r->data);  
    } else {
      send_to_user("--[ not exact room desc match: %i errors.\r\n", match);
    }
    
  }
  

}

ECMD(engine_command_apply_exits)
{
  print_debug(DEBUG_ANALYZER, "in apply_exits");

  engine_flags.last_exits = Rtop->next->data;
  
  do_exits((const char *)Rtop->next->data);
}

ECMD(engine_command_apply_prompt)
{
  char terrain;
  unsigned int i;
  struct Tevent *r;
  Croom *room;

  
//  print_debug(DEBUG_ANALYZER, "in apply_prompt");

  r = Rtop->next;
  
  if (engine_flags.redraw) engine_flags.redraw++;
  
  terrain= r->data[1 + conf.get_prompt_col_len()];  /*second charecter is terrain*/
  engine_flags.last_terrain = terrain;

  if (engine_flags.addingroom) {
    engine_flags.addingroom = 0;
      
//    print_debug(DEBUG_ANALYZER, "ADDINGROOM = 0");
      
    addedroom->refresh_terrain(terrain);
    
    if ( check_roomdesc() != 1) 
      angrylinker(addedroom);
    /*stacker.put(addedroom->id);*/  /*check_roomdesc handles putting in stack */
    return;
  }

  
  if (stacker.amount() == 0) {
    return;
  }
  
  for (i = 0; i < stacker.amount(); i++) {
      room = stacker.get(i);
      if (!conf.get_terrain_check()) {
        stacker.put(room);
        continue;
      }

      if (conf.get_pattern_by_room(room) == terrain) 
        stacker.put(room);
  }
  
  
}

ECMD(engine_command_redraw)
{
//  print_debug(DEBUG_ANALYZER, "[ in FORCED REDRAW ]");
  toggle_renderer_reaction();
  engine_flags.redraw  = 0;
}

ECMD(engine_command_swap)
{
//  print_debug(DEBUG_ANALYZER, "in swap");

  stacker.swap();

  if (stacker.amount() == 0 && engine_flags.resync) 
    engine_command_resync();
  
  if (engine_flags.mapping && stacker.amount() != 1)
    engine_command_mappingoff();
  
  
  if (engine_flags.redraw >= 2) { 
//    print_debug(DEBUG_ANALYZER, "[ in REDRAW ]");
    toggle_renderer_reaction();
    engine_flags.redraw  = 0;
  }
}

ECMD(engine_command_done)
{
//  print_debug(DEBUG_ANALYZER, "in done");

  engine_flags.done = 1;
}

ECMD(engine_command_resync)
{
  unsigned int j;
  Ttree *n;
  
  engine_command_mappingoff();
  
  print_debug(DEBUG_ANALYZER, "FULL RESYNC");
  n = NameMap.find_by_name(engine_flags.last_roomname);
  if (n != NULL)
    for (j = 0; j < n->ids.size(); j++) {
      if (strcmp(engine_flags.last_roomname, Map.getname( n->ids[j] )) == 0) {
//        print_debug(DEBUG_ANALYZER, "Adding matches");
        stacker.put( n->ids[j] );
      } 
    }

  stacker.swap();
}

ECMD(engine_command_rremove)
{
//  print_debug(DEBUG_ANALYZER, "in rremove");

  Rremove();
}

ECMD(engine_command_cremove)
{
//  print_debug(DEBUG_ANALYZER, "in cremove");

  Cremove();
}

ECMD(engine_command_try_dir)
{
  int dir;
  unsigned int i;
  struct Tevent *r, *c;
  Croom *room;
  
//  print_debug(DEBUG_ANALYZER, "in try_dir");

  r = Rtop->next;
  c = Ctop->next;
  
  dir = numbydir(c->data[0]);
  
  if (stacker.amount() == 0) {
    return;
  }
  
  for (i = 0; i < stacker.amount(); i++) {
      room = stacker.get(i);
      if (room->is_connected(dir)) {
          stacker.put(room->exits[dir]);
//          print_debug(DEBUG_ANALYZER, "adding match");
      } else {
//        print_debug(DEBUG_ANALYZER, "failed");
        
        if (stacker.amount() == 1 && engine_flags.mapping && r->type == R_ROOM) {
          /* if we got there we have only one element in possibility */
          /* stack - check the if statement above */

          send_to_user("--[ Adding new room!\n");
//          print_debug(DEBUG_ANALYZER, "adding new room!");
          
    
          
          Map.fixfree();	/* making this call just for more safety - might remove */

          addedroom = new Croom;

          addedroom->id = Map.next_free;
          addedroom->name = strdup(r->data);
          room->exits[dir] = addedroom->id;
          addedroom->exits[reversenum(dir)] = room->id;

          addedroom->x = room->x;
          addedroom->y = room->y;
          addedroom->z = room->z;
          if (dir == NORTH)	addedroom->y += 2;
          if (dir == SOUTH)     addedroom->y -= 2;
          if (dir == EAST)      addedroom->x += 2;
          if (dir == WEST)      addedroom->x -= 2;
          if (dir == UP)	addedroom->z += 2;
          if (dir == DOWN)      addedroom->z -= 2;


          engine_flags.addingroom = 1;	/* for exits, decription and prompt, ends in prompt check */
          Map.addroom(addedroom);
          
          stacker.put(addedroom);

          return;
        }	
        
        
      }
  }
  
  
  
}


ECMD(engine_command_try_all_dirs)
{
  Croom *room;
  unsigned int i, j;

//  print_debug(DEBUG_ANALYZER, "in try_all_dirs");
  engine_command_mappingoff();

  if (stacker.amount() == 0) {
    return;
  }
  
  for (i = 0; i < stacker.amount(); i++) {
      room = stacker.get(i);
      for (j = 0; j <=5; j++) 
        if (room->is_connected(j) ) 
          stacker.put(room->exits[j]);
  }
  
}




void engine()
{
  print_debug(DEBUG_ANALYZER, "in main cycle");
  QTime t;
  
  if (mud_emulation)
    return;

  analyzer_mutex.lock();

  t.start();
  
  copy_stacks();

  engine_flags.done = 0;
  while (!engine_flags.done) 
    engine_run();
  
  print_debug(DEBUG_ANALYZER, "done. Time elapsed %d ms", t.elapsed());
  
  analyzer_mutex.unlock();
}

void engine_run()
{
  struct engine_program_entry *p;
  int i;
  char result, cause;

    if (Rtop->next == NULL)
        result = R_EMPTY;
    else 
        result = Rtop->next->type;
        
    if (Ctop->next == NULL)
        cause = C_EMPTY;
    else 
        cause = Ctop->next->type;

  if (cause == C_EMPTY && result == R_EMPTY) {
    engine_flags.done = 1;
    return;
  }

/*
  print_debug(DEBUG_ANALYZER, "current state cause : %c result %c", 
                              (const char *) event_types[cause].name  , 
                              (const char *) event_types[result].name);
*/  
  /* find passing scriplet and execute it one by one */
  
  p = engine_program;
  while (p) {
  
    if ( (p->cause == cause) && (p->result == result)) {
    for (i=0; i< p->len; i++) 
        ( (*engine_commands[ p->code[i] ].function) () );

      return;
    }
    p = p->next;
  }
  
  printf("No fitting scriplet found.\r\n");
  if (p == NULL) 
    engine_flags.done = 1;
  
  return;
}


void copy_stacks()
{
    struct Tevent *p, *n;
    
    stacks_mutex.lock();
    
    p = pre_Cstack;
    while (p) {
        n = p;
        addtostack(Cbottom, n->type, n->data);
        Cbottom = Cbottom->next;
        p = p->next;
        free(n);
    }
    pre_Cstack = NULL;



    p = pre_Rstack;
    while (p) {
        n = p;
        addtostack(Rbottom, n->type, n->data);
        Rbottom = Rbottom->next;
        p = p->next;
        free(n);
    }
    pre_Rstack = NULL;
    
    stacks_mutex.unlock();
}

/* load config and init engine */
void engine_init()
{

/*
struct event_types_type event_types[] = {
  {"EMPTY",           E_EMPTY,        E_CAUSE},         
  {"EMPTY",           E_EMPTY,        E_RESULT},        
  {"C_MOVE",          C_MOVE,         E_CAUSE},         
  {"C_LOOK",          C_LOOK,         E_CAUSE},         
  {"C_SCOUT",         C_SCOUT,        E_CAUSE},         
  {"R_ROOM",          R_ROOM,         E_RESULT},        
  {"R_EXITS",         R_EXITS,        E_RESULT},        
  {"R_BLIND",         R_BLIND,        E_RESULT},        
  {"R_PROMPT",        R_PROMPT,       E_RESULT},        
  {"R_FAIL",          R_FAIL,         E_RESULT},        
  {"R_DESC",          R_DESC,         E_RESULT},        
  {"R_BLIND",         R_BLIND,        E_RESULT},        
  
  {NULL,              E_EMPTY,         E_NONE}
};
*/
  struct event_types_type event;
  
  event.name = "EMPTY";           event.type = C_EMPTY;         event.group = E_CAUSE;
  event_types.push_back(event);  
  event.name = "EMPTY";           event.type = R_EMPTY;         event.group = E_RESULT;
  event_types.push_back(event);  
  event.name = "C_MOVE";          event.type = C_MOVE;         event.group = E_CAUSE;        
  event_types.push_back(event);  
  event.name = "C_LOOK";          event.type = C_LOOK;         event.group = E_CAUSE;        
  event_types.push_back(event);  
  event.name = "C_SCOUT";         event.type = C_SCOUT;        event.group = E_CAUSE;        
  event_types.push_back(event);  
  event.name = "R_ROOM";          event.type = R_ROOM;         event.group = E_RESULT;        
  event_types.push_back(event);  
  event.name = "R_EXITS";         event.type = R_EXITS;        event.group = E_RESULT;        
  event_types.push_back(event);  
  event.name = "R_BLIND";         event.type = R_BLIND;        event.group = E_RESULT;        
  event_types.push_back(event);  
  event.name = "R_PROMPT";        event.type = R_PROMPT;       event.group = E_RESULT;        
  event_types.push_back(event);  
  event.name = "R_FAIL";          event.type = R_FAIL;         event.group = E_RESULT;        
  event_types.push_back(event);  
  event.name = "R_DESC";          event.type = R_DESC;         event.group = E_RESULT;        
  event_types.push_back(event);  
  event.name = "R_BLIND";         event.type = R_BLIND;        event.group = E_RESULT;        
  event_types.push_back(event);  

  
  /* setting defaults */
  engine_flags.done =                   1;            
  engine_flags.addingroom =             0;
  engine_flags.resync =                 1;
  engine_flags.mapping =                0;
  engine_flags.gettingfirstroom =       0;

  engine_flags.redraw = 0;
  
  engine_flags.last_roomname[0] = 0;
  engine_flags.last_desc[0] = 0;
  engine_flags.last_terrain = 0;
}

int engine_parse_command_line(char cause, char result, char *line)
{
  struct engine_program_entry *p;
  char *k;
  int j;
  int len, i;
  char command[MAX_STR_LEN];
  
  print_debug(DEBUG_ANALYZER, "Engine. Parsing command: %s, %s, %s.", 
                    (const char *) event_types[cause].name , 
                    (const char *) event_types[result].name, line);
  
  /* check if scriplet for this combo of cause-result already exists */
  p = engine_program;
  while (p) {
    if ( (p->cause == cause) && (p->result == result)) {
      printf("Combo already exits. %s command line is not used.", line);
      return 1;
    }
    p = p->next;
  }
  
  p = new engine_program_entry;
  p->cause = cause;
  p->result = result;
  
  
  /* start commands parsing cycle */
  k = line;
  len = 0;
  while (*k) {
    /* go till next ';' char or end of line */
    for (j = 0; *k; k++) {
      if (isspace(*k))
        continue;

      if (*k == ';') {
        k++;
        break;
      }
      command[j++] = LOWER(*k);
    }
    command[j] = 0; /* terminate the line */
    
    if (strlen(command) == 0)
      continue;
    
    /* now find fitting command and translate it in interpreter code */
    for (i = 0; engine_commands[i].name; i++) 
      if (strcmp(command, engine_commands[i].name) == 0) {
        p->code[len++] = i;
        break;
      }
    
    /* we've found the match, right ? else ...*/
    if (engine_commands[i].name == NULL) {
      printf("%s is not a command!\r\n", command);
      free(p);
      return 1;
    }
    
  }

  if (len == 0) {
    printf("No commands in line!\r\n");
    free(p);
    return 1;
  }
 
  /* else link the new command */
  p->next = engine_program;
  engine_program = p;

  p->len = len;
  
  return 0;  
}

int check_roomdesc()
{
    Croom *r;
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


void angrylinker(Croom *r)
{
  Croom *p;
  unsigned int i;
  Croom *candidates[6];
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
