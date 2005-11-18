
#include <vector>
#include <qstring.h>
using namespace std;

#define IS_PROMPT       1
#define IS_CRLF         2
#define IS_LFCR         3
#define IS_LF           4       /* only linefeed lines */
#define IS_SPLIT        5
#define IS_SKIP         6       
#define IS_TELNET       7       /* telnet protocol commands */
#define IS_NONE         8       /* is not an ascii line */

/* global variables */
extern char    base_file[];
extern int     local_port;
extern char    remote_host[];
extern int     remote_port;
extern char    last_prompt[MAX_STR_LEN];


struct Tincoming_lines {
    char line[MAX_DATA_LEN];
    unsigned short int type;
    unsigned short int len;
};


class Cdispatcher
{
    char *o_buf;
    int o_len;
    int o_pos;
	
    char leader_pattern[MAX_STR_LEN];  /* "You now follow "; */

    struct Tincoming_lines buffer[300];
    int amount;

    char roomdesc[PROXY_BUFFER_SIZE];
    char getting_desc;          /* desc shall be incoming - just got roomname */

    char getting_colour_scheme;
    vector<QString> colour_data;      
    
    char last_leaders_movement;
    char leader[MAX_STR_LEN];
    char following_leader;

    
    int check_roomname(char *line);
    int check_exits(char *line);
    int check_failure(char *nline);
	
public:

    
    void set_leaderpatter(char *line);
    
    void  analyze_mud_stream(char *buf, int *n);
    void analyze_user_stream(char *buf, int *n);
    char *get_leader() {return leader; }
    
    void dispatch_buffer(); 
	
    Cdispatcher();
};

extern class Cdispatcher dispatcher;

#ifdef DMALLOC
#include <dmalloc.h>
#endif
