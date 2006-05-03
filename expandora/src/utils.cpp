/* utilities, string functions and stuff */
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <ctime>
#include <cstdlib>

#include "defines.h"
#include "configurator.h"

#if defined Q_OS_WIN32
  #define vsnprintf _vsnprintf
#endif

#include "utils.h"
#include "forwarder.h"

FILE *logfile;
const char *exitnames[] = { "north", "east", "south", "west", "up", "down" };

struct debug_data_struct debug_data[] = {
  {"general",  "Debug", "general switch for debug",  DEBUG_GENERAL, 1},
  {"analyzer", "Analyzer",  "analyzer messages", DEBUG_ANALYZER,1},
  {"system", "System", "general messages", DEBUG_SYSTEM, 1},
  {"config", "Config", "configuration file reader/parser messages", DEBUG_CONFIG, 0},
  {"dispatcher", "Dispatcher", "network streams Dispatcher messages",  DEBUG_DISPATCHER,  1},
  {"proxy", "Proxy", "proxy messages", DEBUG_FORWARDER,   0},
  {"renderer", "Renderer", "OpenGL Renderer messages",DEBUG_RENDERER, 0},
  {"roommanager", "Roommanager", "roommanagers messages", DEBUG_ROOMS, 0},
  {"stacks", "Stacker", "stacks messages", DEBUG_STACKS, 0},
  {"tree", "Tree", "tree search engine messages", DEBUG_TREE, 0},
  {"userfunc", "Userfunc", "user interface messages", DEBUG_USERFUNC, 0},
  {"xml", "xml", "XML module messages",  DEBUG_XML, 0},
  {"interface", "Interface", "Qt windowing interface",  DEBUG_INTERFACE, 0},
  
  {NULL, NULL, NULL, 0, 0}
};
  
const char *exits[] = {
      "north",
      "east",
      "south",
      "west",
      "up",
      "down"
};


const boolean_struct input_booleans[] = {
  {"on", TRUE},
  {"off", FALSE},
  {"1", TRUE},
  {"0", FALSE},
  {"yes", TRUE},
  {"no", FALSE},
  {"+", TRUE},
  {"-", FALSE},
  {"true", TRUE},
  {"false", FALSE},
  
  
  {NULL, FALSE}
};

char    timer_ken[MAX_INPUT_LENGTH];
double  timer_now;

size_t write_to_channel(int mode, const char *format, va_list args);


void basic_mud_vlog(const char *format, va_list args);
size_t write_debug(unsigned int flag, const char *format, va_list args);


int MIN(int a, int b)
{
  return (a < b ? a : b);
}


size_t write_debug(unsigned int flag, const char *format, va_list args)
{
  char txt[MAX_STR_LEN*2];
  size_t size;
  int i;
  
  
  size = vsnprintf(txt, sizeof(txt), format, args);
  
  for (i = 0; debug_data[i].name; i++)
    if (IS_SET(flag, debug_data[i].flag) && debug_data[i].state) {
      printf("%s: %s\r\n", debug_data[i].title, txt);
      if (IS_SET(flag, DEBUG_TOUSER) )
        send_to_user("--[ %s: %s\r\n", debug_data[i].title, txt);
    }
  
  return size;
}

void print_debug(unsigned int flag, const char *messg, ...)
{
  va_list args;

  /* check the general switch state */
  if (!debug_data[0].state)
    return;
  
  if (messg == NULL)
    return;

  va_start(args, messg);
  write_debug(flag, messg, args);
  va_end(args);
}


int parse_dir(char *dir)
{
  int i;
  
  for (i = 0; i<= 5; i++)
    if (is_abbrev(dir, exits[i]) )
      return i;
    
  return -1;
}


int is_integer(char *str)
{
  while ((isdigit(*str) || isspace(*str) || (*str == '-') || (*str == '+')) && 
              (*str != 0))
    str++;
  
  if (*str != 0) 
    return 0;

  return 1;
}


int get_input_boolean(char *input)
{
  int i;

  for (i = 0; input_booleans[i].name != NULL; i++)
    if (strcmp(input, input_booleans[i].name) == 0)
      return input_booleans[i].state;
    
  return -1;
}


char *skip_spaces(char *str)
{
  while (isspace(*str) && (*str != 0))
    str++;
  return str;
}

char *next_space(char *str)
{
  while (isspace(*str) && (*str != 0))
    str++;
  return str;
}

/* mode 0 - lower all chars in argument */
/* mode 1 - do not lower all chars in argument */
/* mode 2 - upper all chars in argument */
char *one_argument(char *argument, char *first_arg, int mode)
{

  if (!argument) {
    *first_arg = '\0';
    return (NULL);
  }

  while (*argument && !isspace(*argument)) {
    if (mode == 0) {
      *(first_arg++) = LOWER(*argument);
    } else if (mode == 1) {
      *(first_arg++) = *argument;
    } else if (mode == 2) {
      *(first_arg++) = UPPER(*argument);
    }
    argument++;
  }
  
  *(first_arg) = '\0';

  return (argument);
}


/*
 * determine if a given string is an abbreviation of another
 */
int is_abbrev(const char *arg1, const char *arg2)
{
  if (!*arg1)
    return (0);

  for (; *arg1 && *arg2; arg1++, arg2++)
    if (*arg1 == '*')
      arg2--;
    else
      if (LOWER(*arg1) != LOWER(*arg2)) 
        return (0);
      
  if (!*arg1 || *arg1 == '*') 
    return (1);
  else 
    return (0);
  
}

int numbydir(char dir)
{
    if (LOWER(dir) == 'n')
	return NORTH;
    if (LOWER(dir) == 's')
	return SOUTH;
    if (LOWER(dir) == 'e')
	return EAST;
    if (LOWER(dir) == 'w')
	return WEST;
    if (LOWER(dir) == 'u')
	return UP;
    if (LOWER(dir) == 'd')
	return DOWN;

    return -1;
}

char dirbynum(int dir)
{
  switch (dir) {
	case  NORTH : 
                return 'n';
		break;
	case  SOUTH :
                return 's';
		break;
	case  EAST :
                return 'e';
		break;
	case  WEST :
                return 'w';
		break;
	case  UP :
                return 'u';
		break;
	case  DOWN :
                return 'd';
		break;
  }


  return -1;
}

int reversenum(int num)
{
    if (num == NORTH)
	return SOUTH;
    if (num == SOUTH)
	return NORTH;
    if (num == EAST)
	return WEST;
    if (num == WEST)
	return EAST;
    if (num == UP)
	return DOWN;
    if (num == DOWN)
	return UP;
    return -1;
}

void send_to_user(const char *messg, ...)
{
  va_list args;

  if (messg == NULL)
    return;

  va_start(args, messg);
  write_to_channel(0, messg, args);
  va_end(args);
}

void send_to_mud(const char *messg, ...)
{
  va_list args;

  if (messg == NULL)
    return;

  va_start(args, messg);
  write_to_channel(1, messg, args);
  va_end(args);
}

/* mode 0 - to user, mode 1 to mud */
size_t write_to_channel(int mode, const char *format, va_list args)
{
  char txt[MAX_STR_LEN*2];
  size_t size;

  size = vsnprintf(txt, sizeof(txt), format, args);
  if (mode == 0)
    proxy.send_line_to_user(txt);
  else if (mode == 1)
    proxy.send_line_to_mud(txt);
  
  return size;
}

void basic_mud_log(const char *format, ...)
{
  va_list args;

  va_start(args, format);
  basic_mud_vlog(format, args);
  va_end(args);
}

void basic_mud_vlog(const char *format, va_list args)
{
  time_t ct = time(0);
  char ts[35];

  if (logfile == NULL) {
    puts("SYSERR: Using log() before stream was initialized!");
    return;
  }

  if (format == NULL)
    format = "SYSERR: log() received a NULL format.";

  strftime(ts, 35, "%Y%m%d %H%M%S", gmtime(&ct));
  fprintf(logfile, "%s ", ts);
  vfprintf(logfile, format, args);
  fputc('\n', logfile);
  fflush(logfile);
}

