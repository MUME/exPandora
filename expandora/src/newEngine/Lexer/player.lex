%option yyclass="PlayerLexer"
%option prefix="Player"
%option c++
%option noyywrap
%option outfile="lex.player.c"


%{
#define PLAYER
#include "Lexer.h"
#include "Coordinate.h"
#include <map>
#include <qstring.h>
#undef PLAYER

   // offsets in Coordinate::stdMoves to determine event types
   uint north;
   uint south;
   uint west;
   uint east;
   uint up;
   uint down;
   uint none;
%}

%%

^"south" subType(south);pushEvent(MOVE);
^"north" subType(north);pushEvent(MOVE);
^"west"  subType(west);pushEvent(MOVE);
^"east"  subType(east);pushEvent(MOVE);
^"up"    subType(up);pushEvent(MOVE);
^"down"  subType(down);pushEvent(MOVE);
^("look"|"examine")(" "|"\t")*("\r\n"|"\n\r") subType(none);pushEvent(MOVE);

%%


 /**
  * this method will be rewritten for every new mud, as people might want to change the standard exits (ie add nw or se)
  * when comparing rooms
  * note that you can overwrite previously defined moves with Coordinate::insertMoves(...)
  */
void Lexer::setMoves() {
  map<QString, Coordinate> myMoves;
  myMoves.insert(make_pair("north", Coordinate(0,1,0)));
  myMoves.insert(make_pair("south", Coordinate(0,-1,0)));
  myMoves.insert(make_pair("west", Coordinate(-1,0,0)));
  myMoves.insert(make_pair("east", Coordinate(1,0,0)));
  myMoves.insert(make_pair("up", Coordinate(0,0,1)));
  myMoves.insert(make_pair("down", Coordinate(0,0,-1)));
  myMoves.insert(make_pair("none", Coordinate(0,0,0)));

  Coordinate::insertMoves(myMoves);

  north = Coordinate::moveCodes.find("north")->second;
  south = Coordinate::moveCodes.find("south")->second;
  west = Coordinate::moveCodes.find("west")->second;
  east = Coordinate::moveCodes.find("east")->second;
  up = Coordinate::moveCodes.find("up")->second;
  down = Coordinate::moveCodes.find("down")->second;     
  none = Coordinate::moveCodes.find("none")->second;
}
