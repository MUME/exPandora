
%option prefix="Mud"
  
/* we will define all the actions in the MudLexer class, so flex should know that it needs to produce MudLexer::yylex() */

/* the codes are octal, so \33 is \27 in decimal and <ESC> in literal*/
OTHERCOL	\33\[([234][013-9]m)|(22m)|(42m)|([1-9]m)
ENDCOL		\33\[0m
ROOMCOL		\33\[32m

SPECIAL_MOB	"Nob"

%x DESC
%x EXITS
%x ROOMNAME
%x PROMPT

/* we should also find things like "You skillfully discover a xy" or "The xy seems to be closed" in the initial state and drop a note on these*/
%%

{ROOMCOL}	BEGIN{ROOMNAME};

<*>[\0]						return; /* end of the string we are parsing*/
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

<ROOMNAME>.|\n			append(YYText()[0]);
<ROOMNAME>{ENDCOL}		pushProperty(); BEGIN(DESC);
<ROOMNAME>{ENDCOL}"\nExits:"	pushProperty(); skipProperty(); BEGIN(EXITS);

<DESC>.|\n						append(YYText()[0]);
<DESC>"Exits:"						pushProperty(); BEGIN(EXITS);
<DESC>^("The "|"A "|"An "|{SPECIAL_MOB}).*"\n"		|
<DESC>^.*"is standing here".*"\n"			|
<DESC>^.*"is resting here".*"\n"			|
<DESC>^.*"is sleeping here".*"\n"			; /* throw a way any mobs, players or objects (and possibly consistent parts of the desc) in this room*/
<DESC,PROMPT>{ROOMCOL}					skipSomeProperties(); pushEvent(ROOM); BEGIN(ROOMNAME);

<EXITS>\[[^\] ,.]+\][,.]	append(YYText()[1]); pushOptional();	/* we don't know if the door is secret, */
<EXITS>"("[^) .,]+")"=?[,.]	append(YYText()[1]); pushOptional();	/* especially when it's open*/
<EXITS>\*[^* ,.]+\*=?[,.]	append(YYText()[1]); pushProperty();	/* *'s around an exit indicate light it seems...*/
<EXITS>[^ ,.]+[,.]		append(YYText()[0]); pushProperty();
<EXITS>=			append(YYText()[0]); 
<EXITS>\n			BEGIN(PROMPT);

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
<PROMPT>"="			| 
<PROMPT>"O"			append(YYText()); pushProperty(); markTerrain(); 
<PROMPT>[\t> ]			pushEvent(ROOM);  BEGIN(INITIAL);


%%

/*TODO: define append(), if possible use flex's own buffer and just move pointers around, append(int) appends only the i'th character*/
/*	pushProperty and pushOptional flush the buffer.*/
/*	matchCompleteRoom matches one room we believe to be fully specified, matchIncompleteRoom matches a description where the last parts are missing*/
/*	jumpProperty indicates that we left out one property, jumpLastProperty indicates that we left out all following but the last property */
/*		- this has to be represented in our search tree somehow so that we can match rooms with title and exits*/
/*	perhaps we should also drop a note in the current room if the user types "exits" or if he searches and finds a hidden exit*/
/*	markTerrain should tell the Room-Algorithm which property determines the terrain type for the rendering*/
