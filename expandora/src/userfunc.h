#ifndef USERLAND_H 
#define USERLAND_H 


#define USERCMD(name)  int name(int cmd, int subcmd, char *line)
#include <deque>
#include <qmutex.h>
using namespace std;


struct user_command_type {
  const char *name;
  int (*command_pointer) (int cmd, int subcmd, char *line);
  int subcmd;           /* subcommand*/
  unsigned int flags;
  const char *desc;     /* short command description */
  const char *help;     /* long description/helpfile */
};

extern const struct user_command_type user_commands[];

struct queued_command_type {
  int  id;
  char arg[MAX_STR_LEN];  
};

class Userland {
  QMutex queue_mutex;
  deque<struct queued_command_type> commands_queue;
public:
    
  void parse_command();
  void add_command(int id, char *arg);
  int is_empty() { return commands_queue.empty(); }

};

extern class Userland userland_parser;

#define USER_PARSE_NONE 0 /* 0 - not my area - send the line futher */
#define USER_PARSE_DONE 1 /* 1 - got the line, and taken care of it - gag it */
int parse_user_input_line(char *line);

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
