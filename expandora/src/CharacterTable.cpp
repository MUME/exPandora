#include <stdio.h>
#include <strings.h>
#include "defines.h"

char characterTable_file[MAX_STR_LEN] = "characterTable";
unsigned char characterTable[128] = ""; 

/*= {
	 0 , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 
	'j', 'k', 'l', 'm', 'm', 'n', 'o', 'p', 'q',
	'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' '};
*/

unsigned char positionTable[128] = "";

void initCharacterTable() {
	bzero(characterTable, 128);
	bzero(positionTable, 128);
	int tablePos = 0;
	int readChar;
	
	FILE *in = fopen(characterTable_file,"r");
	
	while((readChar = fgetc(in)) != EOF) {
		characterTable[tablePos] = (unsigned char)readChar;
		positionTable[readChar] = tablePos;
	}
	
}

