/* Utilities, String functions and stuff */
#ifndef UTILS_H 
#define UTILS_H 

#define log			basic_mud_log

#define LOWER(c)   (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define UPPER(c)   (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c) )

#define ON_OFF(flag) ( (flag) ? "ON" : "OFF" )
#define YES_NO(flag) ( (flag) ? "YES" : "NO" )

#define IS_SET(flag,bit)  ((flag) & (bit))
#define SET_BIT(var,bit)  ((var) |= (bit))
#define REMOVE_BIT(var,bit)  ((var) &= ~(bit))
#define TOGGLE_BIT(var,bit) ((var) = (var) ^ (bit))

#define MAX_INPUT_LENGTH	1024	/* Max length per *line* of input */

struct boolean_struct {
  char *name;
  int   state;
};

#define DEBUG_GENERAL           (1 << 0)
#define DEBUG_ANALYZER          (1 << 1)
#define DEBUG_SYSTEM            (1 << 2)
#define DEBUG_CONFIG            (1 << 3)
#define DEBUG_DISPATCHER        (1 << 4)        
#define DEBUG_FORWARDER         (1 << 5)
#define DEBUG_RENDERER          (1 << 6)
#define DEBUG_ROOMS             (1 << 7)
#define DEBUG_STACKS            (1 << 8)
#define DEBUG_TREE              (1 << 9)
#define DEBUG_USERFUNC          (1 << 10)
#define DEBUG_XML               (1 << 11)
#define DEBUG_INTERFACE         (1 << 12)

/* special flag */
#define DEBUG_TOUSER            (1 << 13)       /* send the message to user also */


extern const char * exitnames[];

struct debug_data_struct {
  char          *name;          
  char          *title;
  char          *desc;
  unsigned int  flag;
  int           state;
};

void print_debug(unsigned int flag, const char *messg, ...);


extern struct debug_data_struct debug_data[];
extern char    timer_ken[MAX_INPUT_LENGTH];
extern double  timer_now;


extern const boolean_struct input_booleans[];
extern const char *exits[];

int is_integer(char *p);
char *skip_spaces(char *str);
char *next_space(char *str);
char *one_argument(char *argument, char *first_arg, int mode);
int is_abbrev(const char *arg1, const char *arg2);
int reversenum(int num);
char dirbynum(int dir);
int numbydir(char dir);
void send_to_user(const char *messg, ...);
void send_to_mud(const char *messg, ...);
int get_input_boolean(char *input);
int parse_dir(char *dir);
void basic_mud_log(const char *format, ...);
int MIN(int a, int b);


#endif


