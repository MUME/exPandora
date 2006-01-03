#ifndef C_EVENT_H
#define C_EVENT_H

#include <QMutex>
#include <QByteArray>
#include <vector>

#define EVENTS_NUM   12

enum EventTypes {       C_EMPTY, 
                   R_EMPTY, 
                   C_MOVE, 
                   C_LOOK, 
                   C_SCOUT, 
                   R_ROOM, 
                   R_EXITS, 
                   R_BLIND, 
                   R_PROMPT, 
                   R_FAIL, 
                   R_DESC
            };

enum EventGroups {      E_NONE = 0, 
                        E_CAUSE = 1,
                        E_RESULT = 2
                 };

typedef struct  {
  QByteArray    data;
  char          type;
  char          group;
} TEvent;

extern std::vector<TEvent> Events;

char get_event_type(QByteArray name);
int get_event_group(QByteArray name);
int get_event_group(char type);


class CEventPipe {
      QMutex     pipe_lock;    /* pipe lock */  
      std::vector<TEvent> pipe;
public:
      CEventPipe();
      ~CEventPipe();
       
      void push(TEvent event);
      TEvent pop();    /* removes front element from the pipe and returns it */
      TEvent get();    /* just returns the front element, it stays in pipe */
      bool empty();
      void clear() { pipe.clear(); }
      QByteArray print();
};

#endif

