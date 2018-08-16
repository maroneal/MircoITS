#ifndef MANAGENETWORK_H_INCLUDED
#define MANAGENETWORK_H_INCLUDED

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define SIZE_IP  20
#define COMMAND_GET_POSITION	0xD0
#define COMMAND_GET_TIME		0xD1

typedef struct point {
    double x;
    double y;
} point;

typedef struct message {
    char wifiIp[SIZE_IP];
	char gsmIp[SIZE_IP];
    int speed;
    point position;
    int object;
    int distance;
    int acceleration;
    int direction;
	char command;
} message;

/********** CREATION AND CONFIGURATION OF SOCKETS **********/
int createSocket();
void configClientSocket(struct sockaddr_in *sa, char *ipDist, int portDist);
int configServerSocket(int sock, struct sockaddr_in *sa, int port);
/********** MANIPULATION OF SOCKETS **********/
int isClientConnected(int sock);
int readSocket(int sock, char *buf, int tailleBuf);
int acceptClient(int sock, struct sockaddr_in *sa_C);
void writeServerSocket(int sock, char *buf, int tailleBuf);
void writeClientSocket(int sock, struct sockaddr_in *sa, char *buf, int tailleBuf);
int getTimeofTravel(int sock, char *IP, struct sockaddr_in *sa);
void closeSocket(int sock);
/********** MANIPULATION OF MESSAGES **********/
void serializeStructMessage(message mess, char *buf);
void unserializeStructMessage(message *mess, char *buf, int tailleBuf);
void resetMessage(message *mess);
int findIndex(char *buf, int taille, int begin, char c);

#endif // MANAGENETWORK_H_INCLUDED
