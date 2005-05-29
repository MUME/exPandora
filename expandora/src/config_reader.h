/* config file reader and parcer - header file */

#define ACMD(name)  void name(char *line)


int parse_config(char * path, char *filename);

#ifdef DMALLOC
#include <dmalloc.h>
#endif
