#include "TelnetFilter.h"


/** the lexical analyzer to be called when the data is made "clean"*/
void TelnetFilter::attachLexer(Lexer * _lexer) {
  lexer = _lexer;
}

  /** these methods purge protocol-specific things from the imput, save a copy 
      in the buffer, then call the analyzer thread via a QWaitCondition; so they
      return before the buffer is analyzed ... */
void TelnetFilter::analyzeMudStream(char * input, int * length) {
  lexer->pushMudInput(purgeProtocolSequences(input, *length)); // pushes the input and wakes the parser thread
} 




      
  /** this one should probably be handled differently - not everything is to be
      passed on - a hackish way will be modifying the input ... */
void TelnetFilter::analyzeUserStream(char * input, int * length) {
  lexer->pushUserInput(purgeProtocolSequences(input, *length));
  if(input[0] == MAP_COMMAND_PREFIX) input[0] = 0;
  *length = 0;
}

char * TelnetFilter::purgeProtocolSequences(char * input, int length) {
  char * lexingBuffer = new char[length];
  int lag = 0;
  for (int i = 0; i < length; i++) {
    if ((unsigned char)input[i] == (unsigned char)255) {
      i += 3;
      lag += 3;
    }
    else lexingBuffer[i-lag] = input[i];
  }
  lexingBuffer[length - 1] = 0;
  return lexingBuffer;
}
