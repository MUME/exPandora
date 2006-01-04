#include "TelnetFilter.h"


/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
extern "C" MY_EXPORT TelnetFilter * createComponent() {
  return new TelnetFilter;
}

  /** these methods purge protocol-specific things from the input, save a copy 
      in the buffer, then call the analyzer thread asynchronously; so they
      return before the buffer is analyzed ... */
void TelnetFilter::analyzeMudStream(char * input, int length) {
  emit newMudInput(purgeProtocolSequences(input, length)); // pushes the input and wakes the parser thread
  return;
} 




      
  /** this one should probably be handled differently - not everything is to be
      passed on - a hackish way will be modifying the input ... */
void TelnetFilter::analyzeUserStream(char * input, int length) {
  emit newUserInput(purgeProtocolSequences(input, length));
  /*if(input[0] == MAP_COMMAND_PREFIX) {
    input[0] = 0;
    return 0;
  }
  else*/ return;
}


char * TelnetFilter::purgeProtocolSequences(char * input, int length) {
  char * lexingBuffer = new char[length+1];
  int lag = 0;
  for (int i = 0; i < length; ++i) {
    if (input[i] == '\xFF') {
      i += 2;
      lag += 3;
    }
    else lexingBuffer[i-lag] = input[i];
  }
  lexingBuffer[length-lag] = 0;
  return lexingBuffer;
}
