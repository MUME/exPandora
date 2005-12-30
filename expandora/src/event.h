#ifndef C_EVENT_H
#define C_EVENT_H

#define E_NONE          0
#define E_CAUSE         1
#define E_RESULT        2

enum results_and_causes { C_EMPTY, R_EMPTY, C_MOVE, C_LOOK, C_SCOUT, R_ROOM, R_EXITS, R_BLIND, R_PROMPT, R_FAIL, R_DESC};

struct event_types_type {
  QByteArray    name;
  char          type;
  char          group;
};


extern vector<struct event_types_type> event_types;


struct Tevent {
  char type;
  QByteArray data;			
  struct Tevent *next, *prev;
};

/* Cause Events top and bottom pointers
  new events are added to the bottom,
  and top ones are taken for analyzing */
extern struct Tevent *Ctop, *Cbottom; /* events that cause movements */
extern struct Tevent *Rtop, *Rbottom; /* response on them */

extern struct Tevent *pre_Cstack;
extern struct Tevent *pre_Rstack;

extern QMutex stacks_mutex;


void preRAdd(char type, QByteArray data);
void preCAdd(char type, QByteArray data);


void Rremove(); /* remove an items from Result stack */
void Cremove(); /* remove an items from Result stack */
void addtostack(struct Tevent *stack, char type, QByteArray data);
void printstacks();

char get_cause_type_by_line(QByteArray line);
char get_result_type_by_line(QByteArray line);

void clear_events_stacks();

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
