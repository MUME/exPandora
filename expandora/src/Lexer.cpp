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

Lexer::Lexer() {
	event = pemm.activate();
	property = pmm.activate();
	playerLexer.attachLexer(this);
	mudLexer.attachLexer(this);
	playerLexer.init();
}

void Lexer::markTerrain() {
	parser->setTerrain(property);
}

void Lexer::pushEvent(char _type) {
	event->push(0);
	event->type = _type;
	parser->event(event);
	event = pemm.activate();
}
	
void Lexer::pushProperty() {
	event->push(property);
	property = pmm.activate();
}
		
void Lexer::pushOptional() {
	event->pushOptional(property);
	property = pmm.activate();
}
		
void Lexer::skipProperty() {
	property->skip();
	event->push(property);
	property = pmm.activate();
}
		
void Lexer::skipSomeProperties() {
	property->skipMany();
	event->push(property);
	property = pmm.activate();
}
		
void Lexer::append(char ap) {
       property->add(ap);       // append a single char
}
		
void Lexer::append(const char * begin, const char * end) {
       property->add(begin, end);	// append some substring
}		
       
void Lexer::append(char * text) { // append a 0-terminated string
	property->add(text);
}
		

