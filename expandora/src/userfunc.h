#define USERCMD(name)  int name(int cmd, int subcmd, char *line)



struct user_command_type {
  const char *name;
  int (*command_pointer) (int cmd, int subcmd, char *line);
  int subcmd;           /* subcommand*/
  unsigned int flags;
  const char *desc;     /* short command description */
  const char *help;     /* long description/helpfile */
};

extern const struct user_command_type user_commands[];


#define USER_PARSE_NONE 0 /* 0 - not my area - send the line futher */
#define USER_PARSE_DONE 1 /* 1 - got the line, and taken care of it - gag it */
int parse_user_input_line(char *line);

#ifdef DMALLOC
#include <dmalloc.h>
#endif
