%option prefix="Player"

%{
#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3
#define UP 4
#define DOWN 5
#define NONE 6

#include "Coordinate.h"
#include "Lexer.h"
%}
%%

"minit"		init(); // we should call init() when a client connects, but the user can do a re-init
"some command"	pass();

%%

/**
 * this method will be rewritten for every new mud, as people might want to change the standard exits (ie add nw or se)
 * and the default tolerance when comparing rooms
 */
void PlayerFlexLexer::init() {
	stdMoves.resize(6);
	stdMoves[NORTH] = new Coordinate(0,1,0);
	stdMoves[SOUTH] = new Coordinate(0,-1,0);
	stdMoves[EAST] = new Coordinate(1,0,0);
	stdMoves[WEST] = new Coordinate(-1,0,0);
	stdMoves[DOWN] = new Coordinate(0,0,-1);
	stdMoves[UP] = new Coorrdinate(0,0,1);
	defaultTolerance = 1;
}

