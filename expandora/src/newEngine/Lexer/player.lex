%option yyclass="PlayerLexer"
%option prefix="Player"
%option c++
%option noyywrap
%option outfile="lex.player.c"


%{
#define PLAYER
#include "Lexer.h"
#include "LexDefs.h"
#include "Coordinate.h"
#undef PLAYER
%}

%%
^"south" pushEvent(SOUTH);
^"north" pushEvent(NORTH);
^"west"  pushEvent(WEST);
^"east"  pushEvent(EAST);
^"up"    pushEvent(UP);
^"down"  pushEvent(DOWN);
^("look"|"examine")(" "|"\t")*("\r\n"|"\n\r") pushEvent(NONE);
%%

