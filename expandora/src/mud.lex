// the codes are octal, so \33 is \27 in decimal and <ESC> in literal

OTHERCOL	\33\[[234][013-9]m|22m|42m|[1-9]m
ENDCOL		\33\[0m	
ROOMCOL		\33\[32m

%x DESC
%x EXITS
%x ROOMNAME

%%


<*>	{OTHERCOL}[^\33]*{ENDCOL}	// throw away some message in other colors
<*>	"It's pitch black ..."		matchIncompleteRoom(); BEGIN(INITIAL);
// all the movement failures will go here (<*>) with moveFail(); as action

<DESC>	.|\n				append();
// problem: what happens with high-perception dark-moving where room names and exits are visible but no descs?
<DESC>	Exits:				pushProperty(); BEGIN(EXITS);
<DESC>	{ROOMCOL}			matchIncompleteRoom(); BEGIN(ROOMNAME);

<EXITS> \[[^\]]*\]			append(1, 1); pushOptional();
<EXITS> \([^)]*\)			append(1, 1); pushOptional();
<EXITS>	\*[^*]*\*			append(1, 1); pushOptional();
<EXITS>	[^ ,],				append(0, 1); pushProperty();
<EXITS>	[^ ,]\n				append(0, 1); pushProperty(); matchCompleteRoom(); BEGIN(INITIAL);
<EXITS> \n				matchCompleteRoom(); BEGIN(INITIAL);

<ROOMNAME>	[.]*			append();
<ROOMNAME>	{ENDCOL}		pushProperty(); BEGIN(DESC);

%%

//TODO: define append(), if possible use flex's own buffer and just move pointers around
//	push* flush the buffer.
//	perhaps we should define the functions in a derived class?
