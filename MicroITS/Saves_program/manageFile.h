#ifndef MANAGEFILE_H_INCLUDED
#define MANAGEFILE_H_INCLUDED

#include <stdio.h>

FILE* openFile(char filename[]);
int closeFile(FILE* file);
void writeFile(char filename[], char buf[], int sizeBuf);

#endif // MANAGEFILE_H_INCLUDED
