
#define NEW_ENGINE
#define DEBUG
//#define DMALLOC

#include <qglobal.h>

#if defined Q_OS_LINUX || defined Q_OS_MACX
#define QT_THREAD_SUPPORT
#endif

#define TRUE            1
#define FALSE           0

#define MAX_ROOMS       30000		/* maximal amount of rooms */


#define MAX_STR_LEN     400
#define MAX_DATA_LEN 3072
#define MAX_LINES_DESC  20

#define PROXY_BUFFER_SIZE 4096

#define NORTH           0         
#define EAST            1
#define SOUTH           2         
#define WEST            3
#define UP              4
#define DOWN            5


/* global flags */
extern int glredraw;		/* redraw is needed */
extern int glquit;			/* quiting ... */

extern int modified;

extern int mud_emulation;              /* we are in emulation mode */


extern int     auda_argc;
extern char    **auda_argv;


extern char roomname_start[MAX_STR_LEN];
extern char roomname_end[MAX_STR_LEN];

extern char logfile_name[MAX_STR_LEN];

extern struct Troom *addedroom;	/* new room, contains new data is addinrroom==1 */


extern const char * exitnames[];
extern char exits_pattern[MAX_STR_LEN];
extern struct TFailureData *failure_data;


int numbydir(char dir);
int reversenum(int num);

void engine();
void toggle_renderer_reaction();
void notify_analyzer();

#ifdef DMALLOC
#include <dmalloc.h>
#endif
