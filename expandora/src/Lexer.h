#include "defines.h"

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

	private:
		void pushEvent();
		void pushProperty();
		void pushOptionalProperty();
		void skipProperty();
		void skipSomeProperties();
		void append();
		void append(char ap);
		void append(char * begin, char * end);
		void append(char * text);
}
		
