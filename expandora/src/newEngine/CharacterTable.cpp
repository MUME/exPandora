#include <stdio.h>
#include <string.h>


char characterTable_file[] = "characterTable";
unsigned char characterTable[128] = ""; 

/*= {
	 0 , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 
	'j', 'k', 'l', 'm', 'm', 'n', 'o', 'p', 'q',
	'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' '};
*/

unsigned char positionTable[128] = "";

void initCharacterTable() {
	memset(characterTable, 0, 128);
	memset(positionTable, 0, 128);
	int tablePos = 0;
	int readChar;
	
	FILE *in = fopen(characterTable_file,"r");
	
	while((readChar = fgetc(in)) != EOF) {
		characterTable[tablePos] = (unsigned char)readChar;
		positionTable[readChar] = tablePos;
	}
	
}

