#ifndef LEXER
#define LEXER
#include "defines.h"
#include "ParseEvent.h"
#include "Property.h"
#include "ObjectRecycler.h"
#include "Parser.h"
#include <qthread.h>
#include <qwaitcondition.h>
#include <qmutex.h>
#include <queue>
using namespace std;


#ifndef PLAYER
#undef yyFlexLexer
#define yyFlexLexer PlayerFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#define yyFlexLexer MudFlexLexer
#endif



#ifndef MUD
#undef yyFlexLexer
#define yyFlexLexer MudFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#define yyFlexLexer PlayerFlexLexer
#endif

class Lexer;

/** 
The GenericLexer provides some simplifications for the flex lexers - though some are only redundancies ...
append creates (via Property.add) a new string and doesn't use yytext. 
So the tree doesn't have to wonder if it has to copy the string 
when it wants to create a new room - it never has to do that. 
If we created a new room we do a shallow clear of the parse event afterwards 
otherwise we do a deep clear - simple.
The memory allocation is handled by the ObjectRecycler 
which will keep the rate of allocating/destroying strings low.
*/
class GenericLexer {
 public:
  GenericLexer();
    void attachParser(Parser * _parser) {parser = _parser;}
    void append(char ap) {property->add(ap);} // append a single char
    void append(const char * begin, const char * end) {property->add(begin, end);} // append some substring
    void append(char * text) {property->add(text);} // append a 0-terminated string
    void clearProperty() {property->clear();}
    void markTerrain() {parser->setTerrain(property);}
    

    void pushEvent(char type);
    void pushProperty();
    void pushOptional();
    void skipProperty();
    void skipSomeProperties();
    
protected:
    Parser * parser;
    ParseEvent * event;
    Property * property;
};

/**
classes to hold the yylex() methods
as we can't derive the flex generated methods from GenericLexer 
we have to do it this way.
*/
class MudLexer : public MudFlexLexer, public GenericLexer {
 public:
  virtual int yylex();
};	
class PlayerLexer : public PlayerFlexLexer, public GenericLexer {
 public:
  virtual int yylex();
  void init();
};


/**
the interface the lexer provides for the proxy; you can attach a parser, and feed it with input
from mud and player. The threading is handled inside this class 
*/
class Lexer : public QThread {
 public:
  Lexer() {playerLexer.init();}
  void attachParser(Parser * _parser) {mudLexer.attachParser(_parser), playerLexer.attachParser(_parser);}
  
  virtual void run();
  void pushUserInput(char * input);
  void pushMudInput(char * input); 
 
 private: 
  QWaitCondition inputSync;
  QMutex inputLock;
  queue<char *> userInput;
  queue<char *> mudInput;
		
  PlayerLexer playerLexer;
  MudLexer mudLexer;

};


#endif
