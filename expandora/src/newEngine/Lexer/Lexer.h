#ifndef LEXER
#define LEXER
#include <iostream>

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

#include <qthread.h>
#include <qwaitcondition.h>
#include <qmutex.h>
#include <queue>
#include <qobject.h>
#include "Component.h"
#include "ParseEvent.h"
#include "Property.h"
#include "ObjectRecycler.h"

using namespace std;

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
class GenericLexer : public QObject {
 private:
  Q_OBJECT

 public:
  GenericLexer();

    void append(char ap) {property->add(ap);} // append a single char
    void append(const char * begin, const char * end) {property->add(begin, end);} // append some substring
    void append(char * text) {property->add(text);} // append a 0-terminated string
    void clearProperty() {property->clear();}

    

    void pushEvent(char type);
    void pushProperty();
    void pushOptional();
    void skipProperty();
    void skipSomeProperties();
    
  signals:
    void terrainDetected(Property *);
    void eventFound(ParseEvent *);
    
protected:
    long m_timestamp();
    void markTerrain() {emit terrainDetected(property);}
    ParseEvent * event;
    Property * property;
};

/**
classes to hold the yylex() methods
as we can't derive the flex generated methods from GenericLexer 
we have to do it this way.
*/
class MudLexer : public GenericLexer, public MudFlexLexer {
 private:
  Q_OBJECT
 public:
  virtual int yylex();
};	
class PlayerLexer : public GenericLexer, public PlayerFlexLexer {
 private:
  Q_OBJECT

 public:
  virtual int yylex();
};


/**
the interface the lexer provides for the proxy; you can attach a parser, and feed it with input
from mud and player. The threading is handled inside this class 
*/
class Lexer : public Component, public QThread {

 public:
  Lexer() {}
  virtual void run();
  void start(Priority priority = InheritPriority) {QThread::start(priority);}

 public slots:
  void pushUserInput(char * input);
  void pushMudInput(char * input); 
 
 private: 
  Q_OBJECT
  
  void init();
  QWaitCondition inputSync;
  QMutex inputLock;
  queue<char *> userInput;
  queue<char *> mudInput;
		
  PlayerLexer playerLexer;
  MudLexer mudLexer;

};


#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
