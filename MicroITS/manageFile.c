#include "manageFile.h"

FILE* openFile(char filename[])
{
	return fopen(filename, "a+");
}

int closeFile(FILE* file)
{
	return fclose(file);
}

void writeFile(char filename[], char buf[], int sizeBuf)
{
	FILE* file;
	if((file = openFile(filename)) == NULL) printf("ERROR DURING OPENING THE LOG FILE\n");
	//if((fwrite(buf, sizeof(char), sizeBuf,file)) < sizeBuf) printf("ERROR DURING WRITING THE LOG FILE\n");
	fprintf(file, "%s", buf);
	if(closeFile(file) == EOF) printf("ERROR DURING CLOSING THE LOG FILE\n");
}
