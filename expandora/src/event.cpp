#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <QMutex>

#include "defines.h"
#include "configurator.h"

#include "struct.h"

#include "event.h"
#include "dispatch.h"
#include "stacks.h"
#include "forwarder.h"
#include "utils.h"
#include "engine.h"

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


struct Tevent *Ctop, *Cbottom;
struct Tevent *Rtop, *Rbottom;

struct Tevent *pre_Cstack;
struct Tevent *pre_Rstack;

QMutex stacks_mutex; 

void clear_events_stacks()
{
    struct Tevent *p;
    
    while (Ctop != Cbottom) 
        Cremove();
    
    while (Rtop != Rbottom) 
        Rremove();
    
    while (pre_Cstack!=NULL) {
        p = pre_Cstack->next;
        free(pre_Cstack);
        pre_Cstack = p;
    }

    while (pre_Rstack!=NULL) {
        p = pre_Rstack->next;
        free(pre_Rstack);
        pre_Rstack = p;
    }
}


void Cremove()
{				
    struct Tevent *p;

    p = Ctop->next;
    if (p == NULL)
	return;

    if (Cbottom == Ctop->next)
	Cbottom = Ctop;
    Ctop->next = p->next;
    if (p->next != NULL)
	p->next->prev = Ctop;
    free(p);
}

void Rremove()
{				/* remove an items from Result stack */
    struct Tevent *p;

    p = Rtop->next;
    if (p == NULL)
	return;

    if (Rbottom == Rtop->next)
	Rbottom = Rtop;

    Rtop->next = p->next;
    if (p->next != NULL)
	p->next->prev = Rtop;
    free(p);
}


void addtostack(struct Tevent *stack, char type, QByteArray data)
{
    struct Tevent *n;

    n = new Tevent;
    n->next = NULL;
    n->prev = stack;

    stack->next = n;

    n->type = type;
    n->data = data;
}

void preRAdd(char type, QByteArray data)
{
    struct Tevent *n, *p;

    stacks_mutex.lock();
    
    /* create and put data */
    n = new Tevent;
    n->next = NULL;
    n->prev = NULL;
    
    n->type = type;
    n->data = data;

    
    /* link */
    if (pre_Rstack == NULL) {
        pre_Rstack = n;

        stacks_mutex.unlock();
        return;
    }
    
    p = pre_Rstack;
    while (p->next != NULL) 
        p = p->next;
        
    if (p->next == NULL) {
        n ->prev = p;
        p->next = n;
    }

    stacks_mutex.unlock();
}

void preCAdd(char type, QByteArray data)
{
    struct Tevent *n, *p;

    stacks_mutex.lock();
    
    /* create and put data */
    n = new Tevent;
    n->next = NULL;
    n->prev = NULL;
    
    n->type = type;
    n->data = data;

    
    /* link */
    if (pre_Cstack == NULL) {
        pre_Cstack = n;

        stacks_mutex.unlock();
        return;
    }
    
    p = pre_Cstack;
    while (p->next != NULL) 
        p = p->next;
        
    if (p->next == NULL) {
        n ->prev = p;
        p->next = n;
    }

    stacks_mutex.unlock();
}




void printstacks()
{
    char line[MAX_DATA_LEN];
    char type[2];

    struct Tevent *p;
    send_to_user(" -----------------------------\n");

    sprintf(line,
	    "Conf: Mapping %s, AutoChecks [Desc %s, Exits %s, Terrain %s],\r\n"
            "      AutoRefresh settings %s (RName/Desc quotes %i/%i), \r\n"
            "      AngryLinker %s\r\n"
            " Current leader: %s\r\n", 
            ON_OFF(engine_flags.mapping), ON_OFF(conf.get_automerge()), 
            ON_OFF(conf.get_exits_check() ), ON_OFF(conf.get_terrain_check() ),
            ON_OFF(conf.get_autorefresh() ), conf.get_name_quote(), conf.get_desc_quote(),
            ON_OFF(conf.get_angrylinker() ), dispatcher.get_leader()
            );
    
    send_to_user(line);
    send_to_user(" Cause events stack:\n");
    line[0] = 0;
    strcat(line, " Element ");
    type[1] = 0;
    p = Ctop->next;
    if (p == NULL)
	strcat(line, "(null)");
    while (p != NULL) {
	strcat(line, "[type ");
	type[0] = p->type;
	strcat(line, type);
	strcat(line, ", data ");
	strcat(line, p->data);
	strcat(line, "] ");
      
        if (strlen(line) > MAX_DATA_LEN/2) 
          break;
	p = p->next;
    }
    send_to_user(line);
    send_to_user("\n");

    send_to_user(" Effect events stack:\n");
    line[0] = 0;
    strcat(line, " Element ");
    type[1] = 0;
    p = Rtop->next;
    if (p == NULL)
	strcat(line, "(null)");
    while (p != NULL) {
	strcat(line, "[type ");
	type[0] = p->type;
	strcat(line, type);
	strcat(line, ", data ");
	strcat(line, p->data);
	strcat(line, "] ");
        if (strlen(line) > MAX_DATA_LEN/2) 
          break;
	p = p->next;
    }
    send_to_user(line);
    send_to_user("\n");

    stacker.printstacks();
}

char get_cause_type_by_line(char *line)
{
  int i;
    
  for (i = 0; event_types[i].name; i++) 
    if ( (event_types[i].group == E_CAUSE) && (strcmp(line, event_types[i].name) == 0) ) 
      return event_types[i].type;
    
  return -1;     
}

char get_result_type_by_line(char *line)
{
  int i;
    
  for (i = 0; event_types[i].name; i++) 
    if ( (event_types[i].group == E_RESULT) && (strcmp(line, event_types[i].name) == 0) ) 
      return event_types[i].type;
    
  return -1;  
}
