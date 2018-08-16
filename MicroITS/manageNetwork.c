#include "manageNetwork.h"

/********** CREATION AND CONFIGURATION OF SOCKETS **********/

// Create a socket and return it
int createSocket()
{
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);	// Create the socket with IPv4 protocol and TCP
	if(sock == -1) perror("Creation socket");	// Error at creating the socket

	return sock;
}

// Configure the socket for the client process
void configClientSocket(struct sockaddr_in *sa, char *ipDist, int portDist)
{
	sa->sin_family = AF_INET;	// IPv4 Internet Protocol
	sa->sin_port = htons(portDist);	// Port of the distant server
	sa->sin_addr.s_addr = inet_addr(ipDist);	// IP of the distant server
}

// Configure the socket for the server process
int configServerSocket(int sock, struct sockaddr_in *sa, int port)
{
	int response = 0;

	bzero((char*) sa, sizeof(struct sockaddr));
	sa->sin_family = AF_INET;	// IPv4 Internet Protocol
	sa->sin_port = htons(port);	// Any IP can connect to the server
	sa->sin_addr.s_addr = INADDR_ANY;	// Port of the server

	if(bind(sock, (struct sockaddr *) sa, sizeof(struct sockaddr)) == -1)	// Attach socket to configuration
	{
		perror("Bind");
		response = -1;
	}
	if(listen(sock, 5) == -1)	// Set maximum number of simultaneous connections
	{
		perror("listen");
		response = -1;
	}

	return response;
}

/********** MANIPULATION OF SOCKETS **********/

// Give the information if someone attemps to connect to the socket in order to send a message
int isClientConnected(int sock)
{
	struct timeval timeout;
	fd_set flag;
	int rc = 0;

	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;	// Timeout of 100ms

	FD_ZERO(&flag);	// Clears all set
	FD_SET(sock, &flag);	// Add the flag to the socket

	rc = select(sock + 1, &flag, NULL, NULL, &timeout);	// Return 0 if timeout reached and >0 if something happens on the socket

    if(rc < 0) perror("SELECT : ");

	return rc;
}

// Read the socket from the server process
int readSocket(int sock, char *buf, int tailleBuf)
{
	// Reading message on socket
	int n=0;
	bzero(buf, tailleBuf);
	if((n = read(sock, buf, tailleBuf)) == -1) perror("Reading client");

	return n;
}

// Accept the client to connect
int acceptClient(int sock, struct sockaddr_in *sa_C)
{
    socklen_t taille_sa = sizeof(struct sockaddr);
    int new_sock;
    //Acceptation of a new client
    if((new_sock = accept(sock, (struct sockaddr *) sa_C, &taille_sa)) == -1) perror("Accept socket");

	return new_sock;
}

// Write the buffer into the socket from the server process
void writeServerSocket(int sock, char *buf, int tailleBuf)
{
	if(write(sock, buf, tailleBuf) == -1) perror("Writing to client");	// Write buffer to the client
}

// Write the buffer into the socket from the client process
void writeClientSocket(int sock, struct sockaddr_in *sa, char *buf, int tailleBuf)
{
	int taille_sa = sizeof(struct sockaddr);

	if(connect(sock, (struct sockaddr *) sa, taille_sa) == -1) perror("Connection socket");	// Connect to the server
	if(write(sock, buf, tailleBuf) == -1) perror("Write to socket");
}

int getTimeofTravel(int sock, char *IP, struct sockaddr_in *sa)
{
	message m;
	clock_t timeBegin, timeEnd;
	char temp[100];

	resetMessage(&m);
	strcpy(m.wifiIp, IP);
	m.command = COMMAND_GET_TIME;

	serializeStructMessage(m, temp);

	timeBegin = clock();
	writeClientSocket(sock, sa, temp, sizeof(temp));

	readSocket(sock, temp, sizeof(temp));

	timeEnd = clock();

	return (int) timeEnd - timeBegin;
}

// Close the socket given
void closeSocket(int sock)
{
	if(close(sock) == -1)	perror("Closing socket");	// Close the socket
}

/********** MANIPULATION OF MESSAGES **********/

// Find the index of a given caracter in the given string
int findIndex(char *buf, int taille, int begin, char c)
{
	int i = begin;
	int carac = -1;

	while(carac != c && i < taille)
	{
		carac = buf[i++];
	}

	if(i == taille) i = -1;	// If we didn't find the character

	return i;
}

// Reset a message structure
void resetMessage(message *mess)
{
	strcpy(mess->wifiIp, "");	// Reset the Wifi IP field
	strcpy(mess->gsmIp, "");	// Reset the GSM IP field
	mess->speed = 0;
	mess->position.x=0;
	mess->position.y=0;
	mess->object = 0;
	mess->distance = 0;
	mess->acceleration = 0;
	mess->direction = 0;
	mess->command = 0x80;
}

// Serialize a message in a string in order to send in a socket
void serializeStructMessage(message mess, char *buf)	// Create a string buffer with information sperated by a semicolon
{
	char temp[100];

	strcpy(buf, "");	// Reset the buffer

	strcpy(buf, mess.wifiIp);
	strcat(buf, ";");

	strcat(buf, mess.gsmIp);
	strcat(buf, ";");

	sprintf(temp, "%d", mess.speed);
	strcat(buf, temp);
	strcat(buf, ";");

	sprintf(temp, "%.2f", mess.position.x);
	strcat(buf, temp);
	strcat(buf, ";");

	sprintf(temp, "%.2f", mess.position.y);
	strcat(buf, temp);
	strcat(buf, ";");

	sprintf(temp, "%d", mess.object);
	strcat(buf, temp);
	strcat(buf, ";");

	sprintf(temp, "%d", mess.distance);
	strcat(buf, temp);
	strcat(buf, ";");

	sprintf(temp, "%d", mess.acceleration);
	strcat(buf, temp);
	strcat(buf, ";");

	sprintf(temp, "%d", mess.direction);
	strcat(buf, temp);
	strcat(buf, ";");

	sprintf(temp, "%c", mess.command);
	strcat(buf, temp);
	strcat(buf, ";");
}

// Unserialize a message from a string in order to manipulate it in the program
void unserializeStructMessage(message *mess, char *buf, int tailleBuf)
{
	int beginIndex = 0;
	int endIndex = 0;
	char temp[tailleBuf];
	memset(temp, 0, tailleBuf);	// Reset the buffer
	resetMessage(mess);

	// Local IP
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	strcpy(mess->wifiIp, temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	// Public IP
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	memset(temp, 0, tailleBuf);	// Reset the buffer
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	strcpy(mess->gsmIp, temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	// Speed
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	memset(temp, 0, tailleBuf);	// Reset the buffer
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	mess->speed = atoi(temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	// Position
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	memset(temp, 0, tailleBuf);	// Reset the buffer
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	mess->position.x = atof(temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	memset(temp, 0, tailleBuf);	// Reset the buffer
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	mess->position.y = atof(temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	// Object
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	memset(temp, 0, tailleBuf);	// Reset the buffer
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	mess->object = atoi(temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	// Distance
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	memset(temp, 0, tailleBuf);	// Reset the buffer
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	mess->distance = atoi(temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	// Acceleration
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	memset(temp, 0, tailleBuf);	// Reset the buffer
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	mess->acceleration = atoi(temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	// Direction
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	memset(temp, 0, tailleBuf);	// Reset the buffer
	strncpy(temp, buf + beginIndex, endIndex - beginIndex - 1); // Minus 1 to avoid getting the semicolon character
	mess->direction = atoi(temp);
	beginIndex = endIndex;	// We begin after the semicolon character

	// Command
	endIndex = findIndex(buf, tailleBuf, endIndex, ';');
	mess->command = (char) buf[endIndex-2];
}
