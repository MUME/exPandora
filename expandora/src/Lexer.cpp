#include "Lexer.h"		
#include <string>
#include <sstream>
#include <iostream>

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
	event->push(0);
	event->type = _type;
	parser->event(event);
	event = pemm.activate();
}
	
void GenericLexer::pushProperty() {
	event->push(property);
	property = pmm.activate();
}
		
void GenericLexer::pushOptional() {
	event->pushOptional(property);
	property = pmm.activate();
}
		
void GenericLexer::skipProperty() {
	property->skip();
	event->push(property);
	property = pmm.activate();
}
		
void GenericLexer::skipSomeProperties() {
	property->skipMany();
	event->push(property);
	property = pmm.activate();
}
		

		

