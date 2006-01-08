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
#include "Map.h"
#include "tree.h"


class CEngine Engine;


/*---------------- * REDRAW ---------------------------- */
void CEngine::command_redraw()
{
    toggle_renderer_reaction();
    redraw = 0;
}
/*---------------- * REDRAW ---------------------------- */

/*---------------- * MAPPING OFF ---------------------------- */
void CEngine::command_mappingoff()
{
  if (mapping) {
//    print_debug(DEBUG_ANALYZER, "in mappingoff");
    send_to_user("--[ Mapping is now OFF!\r\n");
    mapping = 0;
    addingroom = 0;
  }    
}
/*---------------- * MAPPING OFF ---------------------------- */

/*---------------- * APPLY_ROOMNAME ---------------------------- */
void CEngine::command_applyroomname()
{
  CRoom *room;
  unsigned int i;
  int match;
  
//  print_debug(DEBUG_ANALYZER, "in apply_roomname");

  /* set the environment flags and variables */
  setMgoto( false );    /* if we get a new roomname incoming, mgoto has to go away */
  redraw  = 1;
  set_roomname(r_event.data);

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
    if ( (match = room->roomname_cmp(r_event.data)) >= 0) {
      stacker.put(room);
    }
  }
  
  if (stacker.next() == 1 && match > 0) {
    send_to_user("--[ not exact room match: %i errors.\r\n", match);
  }
}
/*---------------- * APPLY_ROOMNAME ---------------------------- */


/*---------------- * APPLY_DESC ---------------------------- */
void CEngine::command_applydesc()
{
  CRoom *room;
  unsigned int i;
  int match;
  
//  print_debug(DEBUG_ANALYZER, "in apply_desc");

  set_desc(r_event.data);

  if (addingroom) {
    addedroom->desc = qstrdup((const char *) r_event.data);
    
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
          if ( (match = room->desc_cmp(r_event.data)) >= 0) {
//              print_debug(DEBUG_ANALYZER, "found matching room");
              stacker.put(room);
          }
      }
  }
  
  if (stacker.next() == 1 && match > 0) {
    /* this means we have exactly one match */
    if (conf.get_autorefresh()) {
      send_to_user("--[ (AutoRefreshed) not exact room desc match: %i errors.\r\n", match);
      stacker.next_first()->refresh_desc(r_event.data);  
    } else {
      send_to_user("--[ not exact room desc match: %i errors.\r\n", match);
    }
    
  }
}
/*---------------- * APPLY_DESC ---------------------------- */

/*---------------- * APPLY_EXITS ------------------------ */
void CEngine::command_applyexits()
{
//  print_debug(DEBUG_ANALYZER, "in apply_exits");

  set_exits(r_event.data);
  
  do_exits((const char *)r_event.data);
}
/*---------------- * APPLY_EXITS --------------------------- */

/*---------------- * APPLY_PROMPT  ------------------------- */
void CEngine::command_applyprompt()
{
  char terrain;
  unsigned int i;
  int col_pos;
  CRoom *room;

  if (mgoto) 
      return;
  
//  print_debug(DEBUG_ANALYZER, "in apply_prompt");
  if (redraw) redraw++;
  
  
  
  col_pos = r_event.data.indexOf(conf.get_prompt_col());
  if (col_pos == 0) {
    terrain = r_event.data[1 + conf.get_prompt_col_len()];  /*second charecter is terrain*/
  } else if (col_pos == -1) {
    terrain = r_event.data[1];  /* no prompt colour in there !*/
  } else if (col_pos > 0) { /* shouldn't be possible ? */
    terrain = r_event.data[col_pos + 1 + conf.get_prompt_col_len()];
  }
  
  if (conf.get_sector_by_pattern(terrain) == 0) 
     terrain = 0;

  set_terrain(terrain);
  if (addingroom) {
    addingroom = 0;
      
    addedroom->refresh_terrain(terrain);
    
    if ( check_roomdesc() != 1) 
      angrylinker(addedroom);

    return;
  }

  
  if (stacker.amount() == 0 || terrain == 0) {
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



/*---------------- * APPLY_PROMPT  ------------------------- */


/*---------------- * SWAP  ------------------------- */
void CEngine::command_swap()
{
//  print_debug(DEBUG_ANALYZER, "in swap");
  if (!mgoto)
      stacker.swap();

  if (stacker.amount() == 0 && resync) 
    command_resync();
  
  if (mapping && stacker.amount() != 1)
    command_mappingoff();
  
  
  if (redraw >= 2) { 
//    print_debug(DEBUG_ANALYZER, "[ in REDRAW ]");
    toggle_renderer_reaction();
    redraw  = 0;
  }
}
/*---------------- * SWAP  ------------------------- */


/*---------------- * DONE  * ------------------------- */
void CEngine::command_done()
{
    done = true;     
}

/*---------------- * DONE * ------------------------- */


/*---------------- * RESYNC * ------------------------- */
void CEngine::command_resync()
{
  unsigned int j;
  TTree *n;
  
  command_mappingoff();
  
  print_debug(DEBUG_ANALYZER, "FULL RESYNC");
  n = NameMap.find_by_name(last_roomname);
  if (n != NULL)
    for (j = 0; j < n->ids.size(); j++) {
      if (last_roomname == Map.getname( n->ids[j] )) {
//        print_debug(DEBUG_ANALYZER, "Adding matches");
        stacker.put( n->ids[j] );
      } 
    }

  stacker.swap();

}

/*---------------- * RESYNC * ------------------------- */
void CEngine::command_rremove()
{
     RPipe.pop();
}

void CEngine::command_cremove()
{
     CPipe.pop();
}

/* ------------------------------------ */

void CEngine::command_trydir()
{
  int dir;
  unsigned int i;
  CRoom *room;
  
//  print_debug(DEBUG_ANALYZER, "in try_dir");
  dir = numbydir(c_event.data[0]);
  
  if (stacker.amount() == 0) 
    return;
  
  for (i = 0; i < stacker.amount(); i++) {
      room = stacker.get(i);
      if (room->is_connected(dir)) {
          stacker.put(room->exits[dir]);
      } else {
        if (stacker.amount() == 1 && mapping && r_event.type == R_ROOM) {
          /* if we got there we have only one element in possibility */
          /* stack - check the if statement above */

          send_to_user("--[ Adding new room!\n");
          
          Map.fixfree();	/* making this call just for more safety - might remove */

          addedroom = new CRoom;

          addedroom->id = Map.next_free;
          addedroom->name = strdup(r_event.data);
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


          addingroom = 1;	/* for exits, decription and prompt, ends in prompt check */
          Map.addroom(addedroom);
          
          stacker.put(addedroom);

          return;
        }	
        
        
      }
  }
  
  
  
}

void CEngine::command_tryalldirs()
{
  CRoom *room;
  unsigned int i, j;

//  print_debug(DEBUG_ANALYZER, "in try_all_dirs");
  command_mappingoff();

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



CEngine::CEngine()
{
    unsigned int i, j;

    engine_init();
    populate_events();    
    TCode code;
    
    code.name = "redraw"; code.func = &CEngine::command_redraw;
    codes.push_back(code);

    code.name = "mappingoff"; code.func = &CEngine::command_mappingoff;
    codes.push_back(code);

    code.name = "resync"; code.func = &CEngine::command_resync;
    codes.push_back(code);

    code.name = "swap"; code.func = &CEngine::command_swap;
    codes.push_back(code);

    code.name = "rremove"; code.func = &CEngine::command_rremove;
    codes.push_back(code);

    code.name = "cremove"; code.func = &CEngine::command_cremove;
    codes.push_back(code);

    code.name = "try_dir"; code.func = &CEngine::command_trydir;
    codes.push_back(code);

    code.name = "try_all_dirs"; code.func = &CEngine::command_tryalldirs;
    codes.push_back(code);

    code.name = "apply_roomname"; code.func = &CEngine::command_applyroomname;
    codes.push_back(code);

    code.name = "apply_desc"; code.func = &CEngine::command_applydesc;
    codes.push_back(code);

    code.name = "apply_exits"; code.func = &CEngine::command_applyexits;
    codes.push_back(code);

    code.name = "apply_prompt"; code.func = &CEngine::command_applyprompt;
    codes.push_back(code);

    code.name = "done"; code.func = &CEngine::command_done;
    codes.push_back(code);


    for (i = 0; i < EVENTS_NUM; i++)
        for (j = 0; j < EVENTS_NUM; j++)
            code_field[i][j] = -1;
            
    printf("Done.\r\n");        
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
    TScript script;
    int code;
    unsigned int i;


//    printf("In Engune run().\r\n");
    if (RPipe.empty() && CPipe.empty()) {
        done = true;
        return;
    }

    
    r_event = RPipe.get();
    c_event = CPipe.get();
/*
    printf("Got events : R: type %s, data %s, C: type %s, data %s.\r\n", 
            (const char *) Events[r_event.type].data, (const char *) r_event.data,
            (const char *) Events[c_event.type].data, (const char *) c_event.data );
*/
    if (code_field[(int)c_event.type][(int)r_event.type] != -1) {
       script = programs[ code_field[(int)c_event.type][(int)r_event.type] ];
/*       printf("Code field is not empty! Script index %i. \r\n", code_field[(int)c_event.type][(int)r_event.type] );
*/
       for (i = 0; i < script.size(); i++) {
           code = script[i];
           (*this.*(codes[code].func))();
       }   
    } else {
        done = true;
        return;        
    }
}

/* load config and init engine */
void CEngine::engine_init()
{
     
  /* setting defaults */
  done =                   1;            
  addingroom =             0;
  resync =                 1;
  mapping =                0;
  gettingfirstroom =       0;
  mgoto             =      0;

  redraw = 0;
  
  last_roomname.clear();
  last_desc.clear();
  last_terrain = 0;
  last_prompt.clear();
}

int CEngine::parse_command_line(char cause, char result, char *line)
{
  char *k;
  int j;
  unsigned int len, i;
  char command[MAX_STR_LEN];
  TScript script;
  
  print_debug(DEBUG_ANALYZER, "Engine. Parsing command: %s, %s, %s.", 
                    (const char *) Events[(int) cause].data , 
                    (const char *) Events[(int)result].data, line);
  
  /* check if scriplet for this combo of cause-result already exists */
  if (code_field[(int)cause][(int)result] != -1 ) {
      printf("Combo already exits. %s command line is not used.", line);
      return 1;
  }
  
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
    for (i = 0; i < codes.size(); i++) 
        if (codes[i].name == command) {
           script.push_back( i );
           len++;
           break;
      }
    
    /* we've found the match, right ? else ...*/
    if (i == codes.size() ) {
      printf("%s is not a command!\r\n", command);
      return 1;
    }
    
  }

  if (len == 0) {
    printf("No commands in line!\r\n");
    return 1;
  }
 
  /* else link the new command */
  code_field[(int)cause][(int)result] = programs.size();
  programs.push_back(script);
  return 0;  
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
            "      AngryLinker %s\r\n"
            " Current leader: %s\r\n", 
            ON_OFF(mapping), ON_OFF(conf.get_automerge()), 
            ON_OFF(conf.get_exits_check() ), ON_OFF(conf.get_terrain_check() ),
            ON_OFF(conf.get_autorefresh() ), conf.get_name_quote(), conf.get_desc_quote(),
            ON_OFF(conf.get_angrylinker() ), dispatcher.get_leader()
            );
    
    send_to_user(line);
    send_to_user(" Cause events queue:\n");
    s = CPipe.print();
    send_to_user((const char *)s);
    send_to_user("\n");

    send_to_user(" Result/Effect events queue:\n");
    s = RPipe.print();
    send_to_user((const char *) s);
    send_to_user("\n");

    stacker.printstacks();
}

void CEngine::add_event(char type, QByteArray data)
{
    TEvent e; 
    e.group = Events[type].group;
    e.type = type;
    e.data = data;

/*    print_debug(DEBUG_ANALYZER, "sending event: type %s, data %s.\r\n", 
            (const char *) Events[type].data, (const char *) data);
*/
    if (Events[type].group == E_CAUSE) 
        CPipe.push(e); 
    else if (Events[type].group == E_RESULT) 
        RPipe.push(e); 
}

bool CEngine::empty()                      /* are pipes empty? */
{
    return RPipe.empty() && CPipe.empty();
}

void CEngine::clear()
{
    RPipe.clear();
    CPipe.clear();     
}

void CEngine::populate_events()
{
  TEvent event;
  
  event.data = "EMPTY";           event.type = C_EMPTY;         
  event.group = E_CAUSE;
  Events.push_back(event);  
  event.data = "EMPTY";           event.type = R_EMPTY;         
  event.group = E_RESULT;
  Events.push_back(event);  
  event.data = "C_MOVE";          event.type = C_MOVE;         
  event.group = E_CAUSE;        
  Events.push_back(event);  
  event.data = "C_LOOK";          event.type = C_LOOK;         
  event.group = E_CAUSE;        
  Events.push_back(event);  
  event.data = "C_SCOUT";         event.type = C_SCOUT;        
  event.group = E_CAUSE;        
  Events.push_back(event);  
  event.data = "R_ROOM";          event.type = R_ROOM;         
  event.group = E_RESULT;        
  Events.push_back(event);  
  event.data = "R_EXITS";         event.type = R_EXITS;        
  event.group = E_RESULT;        
  Events.push_back(event);  
  event.data = "R_BLIND";         event.type = R_BLIND;        
  event.group = E_RESULT;        
  Events.push_back(event);  
  event.data = "R_PROMPT";        event.type = R_PROMPT;       
  event.group = E_RESULT;        
  Events.push_back(event);  
  event.data = "R_FAIL";          event.type = R_FAIL;         
  event.group = E_RESULT;        
  Events.push_back(event);  
  event.data = "R_DESC";          event.type = R_DESC;         
  event.group = E_RESULT;        
  Events.push_back(event);  
  event.data = "R_BLIND";         event.type = R_BLIND;        
  event.group = E_RESULT;        
  Events.push_back(event);  
}
