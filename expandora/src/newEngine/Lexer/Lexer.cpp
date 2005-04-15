#include "Lexer.h"		
#include <string>
#include <sstream>
#include <iostream>


void Lexer::pushUserInput (char * input) {
  inputLock.lock();
  userInput.push(input);
  inputLock.unlock();
  inputSync.wakeOne();
}

void Lexer::pushMudInput(char * input) {
  inputLock.lock();
  mudInput.push(input);
  inputLock.unlock();
  inputSync.wakeOne();
}

void Lexer::run() {
  init();
  istringstream * buf = 0;
  while(1) {
    if (!userInput.empty()) {
      inputLock.lock();
      string temp = string(userInput.front());
      userInput.pop();
      inputLock.unlock();
      playerLexer.switch_streams(buf = new istringstream(temp), 0); 
      playerLexer.yylex();
      delete(buf);
    }
    else if(!mudInput.empty()) {
      inputLock.lock();
      string temp = string(mudInput.front());
      mudInput.pop();
      inputLock.unlock();
      mudLexer.switch_streams(buf = new istringstream(temp), 0); 
      mudLexer.yylex();
      delete(buf);
    }
    else inputSync.wait();
  }    
}

GenericLexer::GenericLexer() {
  event = pemm.activate();
  property = pmm.activate();
}


void GenericLexer::pushEvent(char _type) {
  if (event->timestamp < 1) event->timestamp = m_timestamp();
  event->type = _type;
  emit eventFound(event);
  event = pemm.activate();
}
	
void GenericLexer::pushProperty() {
  if (event->timestamp < 1) event->timestamp = m_timestamp();
  event->push(property);
  property = pmm.activate();
}
		
void GenericLexer::pushOptional() {
  if (event->timestamp < 1) event->timestamp = m_timestamp();
  event->pushOptional(property);
  property = pmm.activate();
}
		
void GenericLexer::skipProperty() {
  if (event->timestamp < 1) event->timestamp = m_timestamp();
  property->skip();
  event->push(property);
  property = pmm.activate();
}
		
void GenericLexer::skipSomeProperties() {
  if (event->timestamp < 1) event->timestamp = m_timestamp();
  property->skipMany();
  event->push(property);
  property = pmm.activate();
}
		

long GenericLexer::m_timestamp()
{
#ifdef Q_OS_LINUX || Q_OS_MACX
  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);
  return tv.tv_sec*1000000 + tv_usec);
#else
  return 0;
#endif
}
		

