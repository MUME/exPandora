#ifndef DEFINES_H 
#define DEFINES_H 


#include <qglobal.h>
#include <cstdlib>

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

/* coordinate's cap */
#define MAX_X           32000
#define MIN_X           -32000


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


/* resizeable dynamic Id's array */
template <class data_t> class ResizableArray {
    int                 amount;
    int                 size;
    data_t              *array;

public:
    ResizableArray();
    ~ResizableArray() { if (array) delete array; }
    bool is_empty();
    void add(data_t data);
    data_t get(int index) { return array[index]; }
    void remove(data_t data);
    void remove_by_index(int i);
    int  find(data_t data);     /* returns index */
    int  get_amount() {return amount; }
    int  get_size() {return size; }
    void removeall() {if (array) delete array; size = 0; amount = 0;}
};


/* array of ID's class implementation */
template <class data_t> ResizableArray<data_t>::ResizableArray()
{
    array = NULL;
    amount = 0;
    size = 0;
}

template <class data_t> bool ResizableArray<data_t>::is_empty()
{
    if (amount == 0)
        return true;

    return false;
}


template <class data_t> void ResizableArray<data_t>::add(data_t data)
{
  if (array == NULL) {
    array = (data_t *) malloc(sizeof(data_t));
    amount = 0; 
    size = 1;
  }
  /* double the amount of space for ids */
  if (amount == size) {		
    size *= 2;
    array = (data_t *) realloc(array, sizeof(data_t) * size);
  }
  /* and finally add the id to the array */
  array[amount++] = data;
}

template <class data_t> void ResizableArray<data_t>::remove_by_index(int i)
{
  *(array+i) = *(array + (amount-1));
  amount--;
  if (amount == 0) 
    free(array);
}

template <class data_t> int  ResizableArray<data_t>::find(data_t data)     /* returns index */
{
  int i;

  for (i=0; i <= amount; i++) 
    if (*(array+i) == data) 
        return i;
    
  
  return -1;
}

template <class data_t> void ResizableArray<data_t>::remove(data_t data)
{
  int i;

  i = find(data);
  if (i > 0)
      remove_by_index(i);
}


#endif


#ifdef DMALLOC
#include <dmalloc.h>
#endif
