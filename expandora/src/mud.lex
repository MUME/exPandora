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

<*>[\0]						return 1; /* end of the string we are parsing*/
<*>{OTHERCOL}[^\33]*{ENDCOL}			/* throw away some message in other colors*/
<*>"It's pitch black ...\n"[^\n]*"\n"		skipSomeProperties(); BEGIN(PROMPT);

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
<DESC>"\n\r"		pushProperty();
<DESC>"Exits:"		BEGIN(EXITS);


<DESC,PROMPT>{ROOMCOL}					skipSomeProperties(); pushEvent(ROOM); BEGIN(ROOMNAME);

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

		       %{
		       //<EXITS>\[[^\] ,.]+\][,.]	append(YYText()[1]); pushOptional();	/* we don't know if the door is secret, */
		       //<EXITS>"("[^) .,]+")"=?[,.]	append(YYText()[1]); pushOptional();	/* especially when it's open*/
		       //<EXITS>\*[^* ,.]+\*=?[,.]	append(YYText()[1]); pushProperty();	/* *'s around an exit indicate light it seems...*/
		       //<EXITS>[^ ,.]+[,.]		append(YYText()[0]); pushProperty();
		       //<EXITS>=			append(YYText()[0]); 
			 %}

<EXITS>"\n\r"			|
<EXITS>"\r\n"			BEGIN(PROMPT);

<PROMPT>"["			|
<PROMPT>"#"			| 
<PROMPT>"."			| 
<PROMPT>"f"			| 
<PROMPT>"("			| 
<PROMPT>"<"			| 
<PROMPT>"%"			| 
<PROMPT>"~"			| 
<PROMPT>"W"			| 
<PROMPT>"U"			| 
<PROMPT>"+"			| 
<PROMPT>":"			| 
<PROMPT>"="			append(YYText()[0]); pushProperty(); pushEvent(ROOM); BEGIN(INITIAL);
<PROMPT>">"			skipProperty(); pushEvent(ROOM);  BEGIN(INITIAL);


%%

/* we should also find things like "You skillfully discover a xy" or "The xy seems to be closed" in the initial state and drop a note on these*/
/* "You flee head over heels" should be transformed into a MOVE event without direction (which will be queued in the user queue by the parser), 
"flee" shouldn't generate an event on the user side*/
/*TODO: define mudLexer.append(), if possible use flex's own buffer and just move pointers around, append(int) appends only the i'th character*/
/*	mudLexer.pushProperty and mudLexer.pushOptional flush the buffer.*/
/*	matchCompleteRoom matches one room we believe to be fully specified, matchIncompleteRoom matches a description where the last parts are missing*/
/*	jumpProperty indicates that we left out one property, jumpLastProperty indicates that we left out all following but the last property */
/*		- this has to be represented in our search tree somehow so that we can match rooms with title and exits*/
/*	perhaps we should also drop a note in the current room if the user types "exits" or if he searches and finds a hidden exit*/
/*	markTerrain should tell the Room-Algorithm which property determines the terrain type for the rendering*/
