%{
#define MUD
#include "Lexer.h"
#undef MUD

%}

%option yyclass="MudLexer"
%option prefix="Mud"
%option c++
%option noyywrap
%option outfile="lex.mud.c"
  

/* the codes are octal, so \33 is \27 in decimal and <ESC> in literal*/
OTHERCOL	\33\[([234][013-9]m)|(22m)|(42m)|([1-9]m)
ENDCOL		\33\[0m
ROOMCOL		\33\[32m



%x DESC
%x EXITS
%x ROOMNAME
%x PROMPT

%%


{ROOMCOL}				BEGIN(ROOMNAME);
"You flee head over heels"  pushEvent(UNKNOWN); /* drop the current room and flee in UNKOWN dir */

<*>{OTHERCOL}[^\33]*{ENDCOL}			/* throw away some message in other colors*/
<*>"You just see a dense fog around you..."                     |
<*>"It is pitch black..."		                        skipSomeProperties(); BEGIN(PROMPT);

<*>"The ".*" seems to be closed."                               |
<*>"Alas, you cannot go that way..."				|
<*>"You need to swim to go there."				|
<*>"You need to swim there."					|
<*>"In your dreams, or what?"					|
<*>"You failed to climb there and fall down, hurting yourself."	|
<*>"You unsccessfully try to break through the ice."		|
<*>"Oops! You cannot go there riding!"				|
<*>"Your mount refuses to follow your orders!"			|
<*>"You can't go into deep water!"				|
<*>"Nah... You feel too relaxed to do that.."			|
<*>"No way! You are fighting for your life!"			|
<*>"You are too exhausted."					|
<*>"You are too exhausted to ride."				|
<*>"You don't control your mount!"				|
<*>"Your mount is too sensible to attempt such a feat."		|
<*>"You need to climb to go there."				|
<*>"The ascent is too steep, you need to climb to go there."	|
<*>"The descent is too steep, you need to climb to go there."	|
<*>"You failed swimming there."					|
<*>"Maybe you should get on your feet first?"			|
<*>"Your boat cannot enter this place."				pushEvent(MOVE_FAIL); BEGIN(INITIAL);

<ROOMNAME>.			        append(YYText()[0]);
<ROOMNAME>{ENDCOL}"\r\n"		pushProperty(); BEGIN(DESC);
<ROOMNAME>{ENDCOL}.{0,3}"Exits:"	pushProperty(); skipProperty(); BEGIN(EXITS);

<DESC>.                 append(YYText()[0]);
<DESC>"\r\n"            clearProperty();
<DESC>"\n\r"	        pushProperty();
<DESC>"Exits:"		clearProperty(); BEGIN(EXITS);


<DESC,EXITS>{ROOMCOL}					skipSomeProperties(); pushEvent(ROOM); BEGIN(ROOMNAME);


<EXITS>"south"  |
<EXITS>"north"  |
<EXITS>"west"   |
<EXITS>"east"   |
<EXITS>"up"     |
<EXITS>"down"   append(YYText()[0]);
<EXITS>")"[.,]  |
<EXITS>"]"[.,]  pushOptional();
<EXITS>")="[.,] |
<EXITS>"]="[.,] append(YYText()[1]); pushOptional();
<EXITS>"="[.,]  append(YYText()[0]); pushProperty();
<EXITS>[.,]     pushProperty();
<EXITS>"\n\r"			|
<EXITS>"\r\n"			BEGIN(PROMPT);

<PROMPT>"["[^\r\n]*">"    		|
<PROMPT>"#"[^\r\n]*">"    		| 
<PROMPT>"."[^\r\n]*">"    		| 
<PROMPT>"f"[^\r\n]*">"    		| 
<PROMPT>"("[^\r\n]*">"    		| 
<PROMPT>"<"[^\r\n]*">"    		| 
<PROMPT>"%"[^\r\n]*">"    		| 
<PROMPT>"~"[^\r\n]*">"    		| 
<PROMPT>"W"[^\r\n]*">"    		| 
<PROMPT>"U"[^\r\n]*">"    		| 
<PROMPT>"+"[^\r\n]*">"    		| 
<PROMPT>":"[^\r\n]*">"    		| 
<PROMPT>"O"[^\r\n]*">"    		|
<PROMPT>"="[^\r\n]*">"    		append(YYText()[0]); markTerrain(); pushProperty(); pushEvent(ROOM); BEGIN(INITIAL);
<PROMPT>{ROOMCOL}               skipProperty(); pushEvent(ROOM); BEGIN(ROOMNAME);
<PROMPT>">"			skipProperty(); pushEvent(ROOM); BEGIN(INITIAL);

%%

/*TODO:
- we should also find things like "You skillfully discover a hidden xy" or "The xy seems to be closed" in the initial state and drop a note on these.
- perhaps we should also drop a note in the current room if the user types "exits" or if he searches and finds a hidden exit.
- leader recogition: "You start following xy" should set some variable to xy. 
  "xy leaves z" should then be recognized and z should be remembered in another variable until "You follow xy". 
  Then a move event like "pushEvent(Coordinate::moveCodes.find(z)->second)" should be generated.
*/
