#include "Lexer.h"		

Lexer::Lexer() {
	event = pemm.activate();
	property = pmm.activate();
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
		
void Lexer::append(char * begin, char * end) {
       property->add(begin, end);	// append some substring
}		
       
void Lexer::append(char * text) { // append a 0-terminated string
	property->add(text);
}
		

