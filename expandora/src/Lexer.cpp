#include "Lexer.h"		
#include <string>
#include <sstream>
#include <iostream>
#include "utils.h"

void Lexer::pushUserInput (char * input) {
  inputLock.lock();
  userInput.push(input);
  inputLock.unlock();
  inputSync.wakeAll(); // there is only one thread waiting to be woken ...
}

void Lexer::pushMudInput(char * input) {
  inputLock.lock();
  mudInput.push(input);
  inputLock.unlock();
  inputSync.wakeAll(); // there is only one thread waiting to be woken ...
}

void Lexer::run() {
  char * buf = 0;
  while(1) {
    if (!userInput.empty()) {
      inputLock.lock();
      buf = userInput.front();
      userInput.pop();
      inputLock.unlock();
      playerLexer.switch_streams(new istringstream(string(buf)), 0);
      playerLexer.yylex(); 
    }
    else if(!mudInput.empty()) {
      inputLock.lock();
      buf = mudInput.front();
      mudInput.pop();
      inputLock.unlock();
      mudLexer.switch_streams(new istringstream(string(buf)), 0);
      mudLexer.yylex(); 
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
  parser->event(event);
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
		

		

