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

class GenericLexer {
 public:
    void attachLexer(Lexer * _lexer) {lexer = _lexer;}
 protected:
    Lexer * lexer;
};

class MudLexer : public MudFlexLexer, public GenericLexer {
 public:
  virtual int yylex();
};
	
class PlayerLexer : public PlayerFlexLexer, public GenericLexer {
 public:
  virtual int yylex();
  void init();
};


class Lexer : public QThread {
	public:

		Lexer();
		
		void attachParser(Parser * _parser) {parser = _parser;}
		virtual void run();

		void markTerrain();
		void pushEvent(char type);
		void pushProperty();
		void pushOptional();
		void skipProperty();
		void skipSomeProperties();
		
		// append creates (via Property.add) a new string and doesn't use yytext. So the tree doesn't have to wonder if
		// it has to copy the string when it wants to create a new room - it never has to do that. 
		// If we created a new room we do a shallow clear of the parse event afterwards otherwise we do a deep clear - simple.
		// the memory allocation is handled by the ObjectRecycler which will keep the rate of allocating/destroying strings low.
		
		void append(char ap); // append a single char
		void append(const char * begin, const char * end); // append some substring
		void append(char * text); // append a 0-terminated string
		
		void pushUserInput (char * input);
		void pushMudInput(char * input); 
	private: 
		QWaitCondition inputSync;
		QMutex inputLock;
		queue<char *> userInput;
		queue<char *> mudInput;
		
		PlayerLexer playerLexer;
		MudLexer mudLexer;
		Parser * parser;
		ParseEvent * event;
		Property * property;
};


#endif
