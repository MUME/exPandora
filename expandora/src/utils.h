/* Utilities, String functions and stuff */
#ifndef UTILS_H 
#define UTILS_H 

#define log			basic_mud_log

#define LOWER(c)   (((c)>='A'  && (c) <= 'Z') ? ((c)+('a'-'A')) : (c))
#define UPPER(c)   (((c)>='a'  && (c) <= 'z') ? ((c)+('A'-'a')) : (c) )

#define ON_OFF(flag) ( (flag) ? "ON" : "OFF" )

#define IS_SET(flag,bit)  ((flag) & (bit))
#define SET_BIT(var,bit)  ((var) |= (bit))
#define REMOVE_BIT(var,bit)  ((var) &= ~(bit))
#define TOGGLE_BIT(var,bit) ((var) = (var) ^ (bit))

#define MAX_INPUT_LENGTH	1024	/* Max length per *line* of input */
#define TIMER_START(ken) {timer_now = m_timestamp(); strcpy(timer_ken, ken);}
#define TIMER_STOP(ms) if ((m_timestamp() - timer_now) * 1000 > (ms)) printf("[timer] %s: %.3fms\r\n", timer_ken, (m_timestamp() - timer_now) * 1000);

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
#ifdef NEW_ENGINE
long m_timestamp();
#else
double m_timestamp(); /* ms */
#endif
void basic_mud_log(const char *format, ...);
int MIN(int a, int b);

#define MAX_N   ( MAX_LINES_DESC * 80 )
#define MAX_M   ( MAX_LINES_DESC * 80 )


class Strings_Comparator {
    private:
        int D[ MAX_N ] [MAX_M ];
    public:
        int compare(QByteArray pattern, QByteArray text);
        int compare_with_quote(QByteArray str, QByteArray text, int quote);
        int strcmp_roomname(QByteArray name, QByteArray text);
        int strcmp_desc(QByteArray name, QByteArray text);

};

extern Strings_Comparator comparator;

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
