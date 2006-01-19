#include "Lexer.h"		
#include <string>
#include <sstream>
#include <iostream>
#include <QDateTime>


/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
#ifndef MONOLITHIC
extern "C" MY_EXPORT Component * createComponent() {
  return new Lexer;
}
#else
Initializer<Lexer> lexer("Lexer");
#endif

void Lexer::pushUserInput (char * input) { 
  istringstream stream(input);
  playerLexer.switch_streams(&stream, 0); 
  playerLexer.yylex();
  delete[] input;
}

void Lexer::pushMudInput(char * input) {
  istringstream stream(input);
  mudLexer.switch_streams(&stream, 0); 
  mudLexer.yylex();
  delete[] input;
}

Lexer::Lexer() : Component(true) {
  playerLexer.setParent(this);
  mudLexer.setParent(this);
}

void Lexer::init() {
	setMoves();
	connect(&playerLexer, SIGNAL(eventFound(ParseEvent* )), this, SIGNAL(eventFound(ParseEvent* )));
	connect(&mudLexer, SIGNAL(eventFound(ParseEvent* )), this, SIGNAL(eventFound(ParseEvent* )));
	connect(&playerLexer, SIGNAL(terrainDetected(Property* )), this, SIGNAL(terrainDetected(Property* )));
	connect(&mudLexer, SIGNAL(terrainDetected(Property* )), this, SIGNAL(terrainDetected(Property* )));
}



GenericLexer::GenericLexer() {
  event = pemm.activate();
  property = pmm.activate();
}


void GenericLexer::pushEvent(uint x_type) {
  if (event->timestamp < 1) event->timestamp = m_timestamp();
  event->type = x_type;
  emit eventFound(event);
  event = pemm.activate();
}
	
void GenericLexer::subType(uint type) {
  event->subType = type;
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
		

uint GenericLexer::m_timestamp()
{
return QDateTime::currentDateTime().toTime_t();
}
		

