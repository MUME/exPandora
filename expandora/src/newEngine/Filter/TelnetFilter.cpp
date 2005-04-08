#include "TelnetFilter.h"


/** the lexical analyzer to be called when the data is made "clean"*/
void TelnetFilter::attachLexer(Lexer * _lexer) {
  lexer = _lexer;
}

  /** these methods purge protocol-specific things from the imput, save a copy 
      in the buffer, then call the analyzer thread via a QWaitCondition; so they
      return before the buffer is analyzed ... */
int TelnetFilter::analyzeMudStream(char * input, int length) {
  lexer->pushMudInput(purgeProtocolSequences(input, length)); // pushes the input and wakes the parser thread
  return length;
} 




      
  /** this one should probably be handled differently - not everything is to be
      passed on - a hackish way will be modifying the input ... */
int TelnetFilter::analyzeUserStream(char * input, int length) {
  lexer->pushUserInput(purgeProtocolSequences(input, length));
  if(input[0] == MAP_COMMAND_PREFIX) {
    input[0] = 0;
    return 0;
  }
  else return length;
}

char * TelnetFilter::purgeProtocolSequences(char * input, int length) {
  char * lexingBuffer = new char[length+1];
  int lag = 0;
  for (int i = 0; i < length; i++) {
    if ((unsigned char)input[i] == (unsigned char)255) {
      i += 2;
      lag += 3;
    }
    else lexingBuffer[i-lag] = input[i];
  }
  lexingBuffer[length-lag] = 0;
  return lexingBuffer;
}