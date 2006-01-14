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
   char north;
   char south;
   char west;
   char east;
   char up;
   char down;
   char none;
%}

%%

^"south" pushEvent(south);
^"north" pushEvent(north);
^"west"  pushEvent(west);
^"east"  pushEvent(east);
^"up"    pushEvent(up);
^"down"  pushEvent(down);
^("look"|"examine")(" "|"\t")*("\r\n"|"\n\r") pushEvent(none);

%%


 /**
  * this method will be rewritten for every new mud, as people might want to change the standard exits (ie add nw or se)
  * when comparing rooms
  * note that you can overwrite previously defined moves with Coordinate::insertMoves(...)
  */
void Lexer::setMoves() {
  map<QString, Coordinate *> myMoves;
  myMoves.insert(make_pair("north", new Coordinate(0,1,0)));
  myMoves.insert(make_pair("south", new Coordinate(0,-1,0)));
  myMoves.insert(make_pair("west", new Coordinate(-1,0,0)));
  myMoves.insert(make_pair("east", new Coordinate(1,0,0)));
  myMoves.insert(make_pair("up", new Coordinate(0,0,1)));
  myMoves.insert(make_pair("down", new Coordinate(0,0,-1)));
  myMoves.insert(make_pair("none", new Coordinate(0,0,0)));

  Coordinate::insertMoves(myMoves);

  north = Coordinate::moveCodes.find("north")->second;
  south = Coordinate::moveCodes.find("south")->second;
  west = Coordinate::moveCodes.find("west")->second;
  east = Coordinate::moveCodes.find("east")->second;
  up = Coordinate::moveCodes.find("up")->second;
  down = Coordinate::moveCodes.find("down")->second;     
  none = Coordinate::moveCodes.find("none")->second;
}
