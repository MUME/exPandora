%option prefix="Player"

%{
#include "LexDefs.h"
#include "Coordinate.h"
%}
%%
/* look and examine will become NONE-moves, forcing the parser to recheck the room we are standing in */
"minit"		init(); // we should call init() when a client connects, but the user can do a re-init
"some command"	pass(); // the player parser is a real proxy while the mud parser is forked in a seperate thread
x			// so we have to pass the player's input to the mud manually when we are done analyzing it

%%

/**
 * this method will be rewritten for every new mud, as people might want to change the standard exits (ie add nw or se)
 * and the default tolerance when comparing rooms
 */
void PlayerFlexLexer::init() {
	stdMoves.resize(7);
	stdMoves[NORTH] = new Coordinate(0,1,0);
	stdMoves[SOUTH] = new Coordinate(0,-1,0);
	stdMoves[EAST] = new Coordinate(1,0,0);
	stdMoves[WEST] = new Coordinate(-1,0,0);
	stdMoves[DOWN] = new Coordinate(0,0,-1);
	stdMoves[UP] = new Coorrdinate(0,0,1);
	stdMoves[NONE] = new Coordinate(0,0,0);
	defaultTolerance = 1;
}

