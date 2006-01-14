#include "TelnetFilter.h"


/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
#ifndef MONOLITHIC
extern "C" MY_EXPORT TelnetFilter * createComponent()
{
  return new TelnetFilter;
}
#else
Initializer<TelnetFilter> telnetFilter("Filter");
#endif


using namespace Qt;

/** these methods are called asynchronously because the connectionType is queued
	like this we get all the benefits of explicit threading for free. */
void TelnetFilter::analyzeMudStream(char * input, int length)
{
  emit newMudInput(purgeProtocolSequences(input, length)); // pushes the input and wakes the parser thread
  return;
}


/** this one should probably be handled differently - not everything is to be
    passed on - a hackish way will be modifying the input ... */
void TelnetFilter::analyzeUserStream(char * input, int length)
{
  emit newUserInput(purgeProtocolSequences(input, length));
  /*if(input[0] == MAP_COMMAND_PREFIX) {
    input[0] = 0;
    return 0;
  }
  else*/ return;
}

ConnectionType TelnetFilter::requiredConnectionType(const QString & str)
{
  
  if (str == SLOT(analyzeUserStream(char *, int)) || str == SLOT(analyzeMudStream(char *, int)))
    return DirectConnection;
  else if (str == SIGNAL(newUserInput(char *)) || str == SIGNAL(newMudInput(char *)))
    return QueuedConnection;
  else
    throw "no such signal or slot";
}

char * TelnetFilter::purgeProtocolSequences(char * input, int length)
{
  char * lexingBuffer = new char[length+1];
  int lag = 0;
  for (int i = 0; i < length; ++i)
  {
    if (input[i] == '\xFF')
    {
      i += 2;
      lag += 3;
    }
    else lexingBuffer[i-lag] = input[i];
  }
  lexingBuffer[length-lag] = 0;
  return lexingBuffer;
}
