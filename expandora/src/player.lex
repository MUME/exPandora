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
"minit" init();
"south" pushEvent(SOUTH);
"north" pushEvent(NORTH);
"west"  pushEvent(WEST);
"east"  pushEvent(EAST);
"up"    pushEvent(UP);
"down"  pushEvent(DOWN);
%%

/**
 * this method will be rewritten for every new mud, as people might want to change the standard exits (ie add nw or se)
 * and the default tolerance when comparing rooms
 */
void PlayerLexer::init() {
	stdMoves.resize(7);
	stdMoves[NORTH] = new Coordinate(0,1,0);
	stdMoves[SOUTH] = new Coordinate(0,-1,0);
	stdMoves[EAST] = new Coordinate(1,0,0);
	stdMoves[WEST] = new Coordinate(-1,0,0);
	stdMoves[DOWN] = new Coordinate(0,0,-1);
	stdMoves[UP] = new Coordinate(0,0,1);
	stdMoves[NONE] = new Coordinate(0,0,0);
	defaultTolerance = 1;
}
