#include <stdio.h>

int main(int argc, char *argv[]) {
	int i;
	FILE * file = fopen("characterTable", "w");
	//for (i = 0; i< 128; i++)
	//	fputc(i, file);
	char test = 200;
	int test2 = 0;	
	unsigned char test3 = 200;
	printf("test: %i\n", test);
	printf("test2: %i\n", test2);
	printf("test3: %i\n", test3);
	fputc(test, file);
	fputc(test2, file);
	fputc(test3, file);
	fputc(138, file);
	fputc(136, file);
	fclose(file);
	return 0;
}