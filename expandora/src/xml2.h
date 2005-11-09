
/* buffered reading from file */


void xml_writebase(char *filename);

/* Loads a char, returns char struct or NULL */
void xml_readbase(char *filename);

#ifdef DMALLOC
#include <dmalloc.h>
#endif
