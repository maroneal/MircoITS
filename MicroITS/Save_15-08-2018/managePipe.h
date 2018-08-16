#ifndef MANAGEPIPE_H_INCLUDED
#define MANAGEPIPE_H_INCLUDED

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>

void createPipe(int *fd1, int *fd2);
void configPipeClient(int *fd1, int *fd2);
void configPipeServer(int *fd1, int *fd2);
void writePipe(int *fd, char *buf, int tailleBuf);
int readPipe(int *fd, char *buf, int tailleBuf);
int isPipeAvailable(int *fd);
void closeAllPipe(int *fd1, int *fd2);

#endif // MANAGEPIPE_H_INCLUDED
