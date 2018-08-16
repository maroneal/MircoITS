#include "managePipe.h"

// Create the pipes
void createPipe(int *fd1, int *fd2)
{
	pipe(fd1);
	pipe(fd2);
}

// Configure the pipes for the client process
void configPipeClient(int *fd1, int *fd2)
{
	close(fd1[1]);	// Close write section of fd1
	close(fd2[0]);	// Close read section of fd2
}

// Configure the pipes for the server process
void configPipeServer(int *fd1, int *fd2)
{
	close(fd1[0]);	// Close read section of fd1
	close(fd2[1]);	// Close write section of fd2
}

// Write the buffer into the given pipe
void writePipe(int *fd, char *buf, int tailleBuf)
{
	if(write(fd[1], buf, tailleBuf) == -1) perror("Write pipe");
}

// Read the given pipe into the buffer
int readPipe(int *fd, char *buf, int tailleBuf)
{
	int n=0;
	memset(buf, 0, tailleBuf);
	if((n = read(fd[0], buf, tailleBuf)) == -1) perror("Read pipe");

	return n;
}

// Give the information if a message is available on the pipe given
int isPipeAvailable(int *fd)
{
	struct timeval timeout;
	fd_set flag;
	int rc = 0;

	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;	// Timeout of 100ms

	FD_ZERO(&flag);	// Clears all set
	FD_SET(fd[0], &flag);	// Add the flag to the socket

	rc = select(fd[0] + 1, &flag, NULL, NULL, &timeout);	// Return 0 if timeout reached and >0 if something happens on the socket

	if(rc < 0) perror("SELECT : ");

	return rc;
}

// Close the pipe given
void closeAllPipe(int *fd1, int *fd2)
{
	close(fd1[1]);	// Close write section of fd1
	close(fd2[0]);	// Close read section of fd2
	close(fd1[0]);	// Close read section of fd1
	close(fd2[1]);	// Close write section of fd2
}
