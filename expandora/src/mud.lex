// the codes are octal, so \33 is \27 in decimal and <ESC> in literal

OTHERCOL	\33\[[234][013-9]m|22m|42m|[1-9]m
ENDCOL		\33\[0m
ROOMCOL		\33\[32m

%x DESC
%x EXITS
%x ROOMNAME
%x PROMPT

%%

{ROOMCOL}	BEGIN{ROOMNAME};


<*>	{OTHERCOL}[^\33]*{ENDCOL}			;// throw away some message in other colors
<*>	"It's pitch black ...\n"[.]*"\n"		jumpLastProperty; BEGIN(PROMPT);

<*> 	"Alas, you cannot go that way..."				moveFail();
<*> 	"You need to swim to go there."					moveFail();
<*> 	"You need to swim there."					moveFail();
<*> 	"In your dreams, or what?"					moveFail();
<*> 	"You failed to climb there and fall down, hurting yourself."	moveFail();
<*>	"You unsccessfully try to break through the ice."		moveFail();
<*>	"Oops! You cannot go there riding!"				moveFail();
<*>	"Your mount refuses to follow your orders!"			moveFail();
<*>	"You can't go into deep water!"					moveFail();
<*>	"Nah... You feel too relaxed to do that.."			moveFail();
<*>	"No way! You are fighting for your life!"			moveFail();
<*>	"You are too exhausted."					moveFail();
<*>	"You are too exhausted to ride."				moveFail();
<*>	"You don't control your mount!"					moveFail();
<*>	"Your mount is too sensible to attempt such a feat."		moveFail();
<*>	"You need to climb to go there."				moveFail();
<*>	"The ascent is too steep, you need to climb to go there."	moveFail();
<*>	"The descent is too steep, you need to climb to go there."	moveFail();
<*>	"You failed swimming there."					moveFail();
<*>	"Maybe you should get on your feet first?"			moveFail();
<*>	"Your boat cannot enter this place."				moveFail();

<ROOMNAME>	.|\n			append();
<ROOMNAME>	{ENDCOL}		pushProperty(); BEGIN(DESC);
<ROOMNAME>	{ENDCOL}[\n]"Exits:"	pushProperty(); jumpProperty(); BEGIN(EXITS);

<DESC>		.|[\n]			append();
<DESC>		"Exits:"		pushProperty(); BEGIN(EXITS);
<DESC,PROMPT>	{ROOMCOL}		matchIncompleteRoom(); BEGIN(ROOMNAME);

<EXITS> 	\[[^\]]*\]		append(1); pushOptional();	// we don't know if the door is secret, 
<EXITS> 	\([^)]*\)		append(1); pushOptional();	// especially when it's open
<EXITS>		\*[^*]*\*		append(1); pushProperty();	// *'s around an exit indicate light it seems...
<EXITS>		[^ ,]*,			append(0); pushProperty();
<EXITS>		[^ ,]*\n		append(0); pushProperty(); 
<EXITS> 	\n			BEGIN(PROMPT);

<PROMPT> 	"["			| 
<PROMPT> 	"#"			|		 
<PROMPT>	"."			| 
<PROMPT>	"f"			| 
<PROMPT>	"("			| 
<PROMPT>	"<"			| 
<PROMPT>	"%"			| 
<PROMPT>	"~"			| 
<PROMPT>	"W"			| 
<PROMPT>	"U"			| 
<PROMPT>	"+"			| 	
<PROMPT>	":"			| 
<PROMPT>	"="			| 
<PROMPT>	"O"			append(); pushProperty(); markTerrain(); 
<PROMPT>	[\t> ]			matchCompleteRoom();  BEGIN(INITIAL);


%%

//TODO: define append(), if possible use flex's own buffer and just move pointers around, append(int) appends only the i'th character
//	pushProperty and pushOptional flush the buffer.
//	matchCompleteRoom matches one room we believe to be fully specified, matchIncompleteRoom matches a description where the last parts are missing
//	jumpProperty indicates that we left out one property, jumpLastProperty indicates that we left out all following but the last property 
//		- this has to be represented in our search tree somehow so that we can match rooms with title and exits
//	perhaps we should also drop a note in the current room if the user types "exits" or if he searches and finds a hidden exit
//	markTerrain should tell the Room-Algorithm which property determines the terrain type
//	perhaps we should define the functions in a derived class?
