#include "defines.h"
#include "ParseEvent.h"
#include "Property.h"
#include "ObjectRecycler.h"

#undef yyFlexLexer
#define yyFlexLexer PlayerFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#define yyFlexLexer MudFlexLexer
#include <FlexLexer.h>

class Lexer : private MudFlexLexer, private PlayerFlexLexer {
	public:
		void MudLex(char * in) {MudFlexLexer::LexerInput(in, MAX_DATA_LEN); MudFlexLexer::yylex();}
		void PlayerLex(char * in) {PlayerFlexLexer::LexerInput(in, MAX_DATA_LEN); PlayerFlexLexer::yylex();}
	private: // only used by yylex()	
		void pushEvent();
		void pushProperty();
		void pushOptional();
		void skipProperty();
		void skipSomeProperties();
		void append(char ap); // append a single char
		void append(char * begin, char * end); // append some substring
		void append(char * text); // append a 0-terminated string

		ObjectRecycler<ParseEvent> events;
		ObjectRecycler<Property> props;
		ParseEvent * event;
		Property * property;
}
		
