#ifndef TELNETFILTER
#define TELNETFILTER
#include "Lexer.h"
#include "defines.h"

// we should chage that prefix ... perhaps '¤' - or is that only present on german keyboards ;)
#define MAP_COMMAND_PREFIX 'm' 


class TelnetFilter {
 public:
  /** the lexical analyzer to be called when the data is made "clean"*/
  void attachLexer(Lexer * lexer);

  /** these methods purge protocol-specific things from the imput, save a copy 
      in the buffer, then call the analyzer thread via a QWaitCondition; so they
      return before the buffer is analyzed ... */
  int analyzeMudStream(char * input, int length);

  /** this one should probably be handled differently - not everything is to be
      passed on - a hackish way will be modifying the input ... */
  int analyzeUserStream(char * input, int length);
 private:
  Lexer * lexer;
  char * purgeProtocolSequences(char * input, int length);
};

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
