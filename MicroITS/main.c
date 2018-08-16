#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <stdint.h>
#include <sys/types.h>
#include <wait.h>
#include <time.h>
#include "managePipe.h"
#include "manageNetwork.h"
#include "manageRobot.h"
#include "manageFile.h"

// TCP
#define PORT_TCP				4200
#define IP_DIST					"192.168.1.2"
#define IP_LOCAL				"192.168.1.1"
#define IP_LOOPBACK				"127.0.0.1"
#define MAX_VEHICLE				10
// Buffer
#define SIZE_BUF				100
// Serial
#define SERIAL_ARDUINO NAME		"/dev/serial0"
#define SERIAL_ARDUINO_SPEED	115200
// Time
#define TIME_PERIOD_UPDATE		1

char end;
//

// Handler to get the signal SIGINT. It allows to have a clean exit by closing the sockets and the pipes and the serial communication
void handler(int numSignal) {
	if (numSignal == SIGINT) // If signal CTRL+C is sent, we stop the program properly
	{
        end = 1;
	}
}

int getTime()
{
	return (int) clock() * 1000 / CLOCKS_PER_SEC;
	//return (int)time(NULL);
}

int main(int argc, char * argv[])
{
	end = 0;    // Reset the ending variable

	// Socket
	int socketMain, clientSocket;
	struct sockaddr_in sa;

	// Signal handler
	struct sigaction sigHandler;
	sigHandler.sa_handler = handler;    // Associate the "handler" function to the handler
	sigaction(SIGINT, &sigHandler, NULL);   // Attach the signal "SIGINT" to the handler

	// Pipe
	int fd1[2]; // Pipe Server -> Client
    int fd2[2]; // Pipe Client -> Server
	createPipe(fd1, fd2);

	// Message
	message mes;
	resetMessage(&mes);

	// Serial communication
	int serial;

	// GSM communication
	//int gsm;

	// Buffer
	char buffer[SIZE_BUF];  // Buffer to manipulate strings in the program

	// Encoders
	//point origin = {0, 0};
	//point position = {0, 0};

	// Others
	//int n = 0;

	// Launch the python script
	//system("lxterminal -e \"python3 LocalizeAndFollow.py\"");

	// Fork to get two parallel processes
	pid_t childPid = -2;
	childPid = fork();

	switch (childPid) {
		case -1:
			// Close pipeline before exit
			closeAllPipe(fd1, fd2);

			perror("Fork :");
			exit(EXIT_FAILURE);
			break;

		case 0: // Child process - (TCP Client)
		    ;
		    /******* Begin Process *******/
		    // Variables
            clock_t time;

		    // Initialization process
			configPipeClient(fd1, fd2); // Config pipeline for the client

			//printf("Adresse IP de la source : %s \n", inet_ntoa(sa_S_diff.sin_addr));

			strcat(mes.wifiIp, IP_DIST);
			strcat(mes.gsmIp, "");
			mes.speed=42;
			mes.position.x = 0;
			mes.position.y = 1;
			mes.object = 2;
			mes.distance = 27;
			mes.acceleration = 3;
			mes.direction = 4;
			mes.command = 5;

			configClientSocket(&sa, IP_DIST, PORT_TCP); // Configure the socket to communicate with a distant or local server

			serializeStructMessage(mes, buffer);    // Serialize the structure in a string to be able to send it on the socket

			sleep(1);
			readPipe(fd1, buffer, SIZE_BUF);    // Read the pipe to see if the father is either ready or not
			if(strcmp(buffer, "OK")) printf("PID : %d : FATHER IS NOT OK\n", getpid());	// If the father says he is not ready, we print it in the terminal

			/******* Main loop *******/

			while (!end)
			{
                if(!isPipeAvailable(fd1))
				{
					readPipe(fd1, buffer, SIZE_BUF);
					unserializeStructMessage(&mes, buffer, SIZE_BUF);

					printf("COMMAND : %x\n", mes.command);

					if(mes.command == COMMAND_GET_TIME)	// If the server wants to get the time of a round trip of a message between the two robots
					{
						printf("TIME ASKED\n");

						// Create the socket

						socketMain = createSocket();

						time = getTimeofTravel(socketMain, IP_LOCAL, &sa);	// Get the time of the round trip
						closeSocket(socketMain);	// Close the socket


						strcpy(buffer, "");
						sprintf(buffer, "%d", (int)time);	// Print the time of the round trip in the buffer
						strcat(buffer, "\n");
						// Close the socket



						writePipe(fd2, buffer, SIZE_BUF);	// Write the time into the pipe
					}
					else if (mes.command == COMMAND_GET_POSITION)	// If the server wants to get the position of the other robot
					{
						printf("POSITION ASKED\n");
						resetMessage(&mes);	// Reset the message

						strcpy(mes.wifiIp, IP_LOCAL);
						mes.command = COMMAND_GET_POSITION;	// Send the command "position" to the other robot for that he can send his position
						serializeStructMessage(mes, buffer);	// Serialize the message to be able to send it through the socket

						// Create the socket
						socketMain = createSocket();

						writeClientSocket(socketMain, &sa, buffer, sizeof(buffer));	// Send the message through the socket
						readSocket(socketMain, buffer, sizeof(buffer));	// Wait and read the socket for the message from the other robot

						unserializeStructMessage(&mes, buffer, SIZE_BUF);	// Unserialize the string to get a message

						strcpy(buffer, "");
						sprintf(buffer, "%.1f,%.1f\n", mes.position.x, mes.position.y);	// Write the position of the other robot in the buffer
						closeSocket(socketMain);

						writePipe(fd2, buffer, SIZE_BUF);	// Write the position of the other robot into the pipe
					}
				}
			}

				//turnLeft90deg(serial, origin, &position);

				//getPosition(serial, origin, &position);
				//printf("X : %.2f, Y : %.2f\n", position.x, position.y);
				/*n = readPipe(fd1, buffer, SIZE_BUF);

				system("/home/pi/Desktop/MicroITS/wifi_monitoring_oneshot.sh");
				writeFile("../log_oneshot.txt", buffer, SIZE_BUF);
				printf("OK\n");*/


			/******* Finishing Process *******/

			close(fd1[0]);
            close(fd2[1]);
            //close(clientSocket);

			printf("PID : %d : Child finished\n", getpid());
			break;

		default: // Father process - (TCP Server)
			;
			/******* Begin Process *******/
			// Variables
			int clientStatus;
			char secondBuf[SIZE_BUF];
			point position;
			position.x = 0;
			position.y = 0;
			int timer = 0;

			strcpy(secondBuf, "OK");

			// Initialization process
			serial = openSerial("/dev/serial0", SERIAL_ARDUINO_SPEED);  // Open the serial communication with the Arduino
			//gsm = openSerial("/dev/ttyUSB0", 9600);   // Open the serial communication with the GSM

			configPipeServer(fd1, fd2);	// Configure pipe between server and client

			//initRobot();

			socketMain = createSocket();

			if(configServerSocket(socketMain, &sa, PORT_TCP) == -1)	// If the server socket can't be set
			{
				kill(childPid, SIGKILL);	// Send signal to child to kill it
				closeSocket(socketMain);	// Close the socket properly
				exit(EXIT_FAILURE);	// Exit the program with an error code
			}

			//resetEncoders(serial);	// Reset the encoders
			//resetOrigin(serial, &origin);
			writePipe(fd1, "OK", sizeof("OK"));
			sleep(1);

			/******* Main loop *******/

			while (!end)
			{
				clientStatus = isClientConnected(socketMain);

                if (clientStatus > 0)    // If something happened on the socket
				{
                    // Accept client to connect
                    clientSocket = acceptClient(socketMain, &sa);  // Get the connection with the client
                    //printf("CONNECTED\n");

                    readSocket(clientSocket, buffer, SIZE_BUF);   // Read the message from the client

                    unserializeStructMessage(&mes, buffer, SIZE_BUF);

                    if(!strcmp(mes.wifiIp, IP_LOOPBACK))	// If the message comes from the python program
					{
						if(mes.command == COMMAND_NOTHING)	// If the command is nothing, then the python program only wants to send the position of the robot
						{
							position.x = mes.position.x;	// Storing of the position
							position.y = mes.position.y;
						}
					}
					else if(!strcmp(mes.wifiIp, IP_DIST))	// If the message comes from the other robot
					{
						if(mes.command == COMMAND_GET_TIME)	// If the other robot wants to know the time of a round trip of a message
						{
							printf("GET TIME\n");
							writeServerSocket(clientSocket, buffer, SIZE_BUF);	// The same message is sent to the other robot
						}
						else if(mes.command == COMMAND_GET_POSITION)	// If the other robot wants to know the position of this robot
						{
							printf("GET POSITION\n");
							resetMessage(&mes);	// Reset of the message
							strcpy(buffer, IP_LOCAL);	// Copy our local IP address
							mes.position.x = position.x;	// Store the position of the robot
							mes.position.y = position.y;
							serializeStructMessage(mes, buffer);	// Serialiaze the message to be able to send it through the socket

							writeServerSocket(clientSocket, buffer, SIZE_BUF);	// Write the message into the socket to send the message to the other robot
						}
					}

					sendCommand(serial, mes.command);	// Send the command of the message to the
					resetMessage(&mes);	// Reset the message to finish it clearly
				}

				//usleep(1000000);	// Wait necessary to be able for the program to work. Don't know why, surely because of the pipes which need latency
				//printf("Time : %d\n", getTime());
				/*if(getTime() >= timer + TIME_PERIOD_UPDATE)	// If it's time to store all the data in the file
				{
					char temp[100];	// Store the current position in the buffer
					strcpy(buffer, "");
					sprintf(temp, "%.1f", position.x);
					strcat(buffer, temp);
					strcat(buffer, ",");
					sprintf(temp, "%.1f", position.y);
					strcat(buffer, temp);
					//strcat(buffer, "\n;\n");
					strcat(buffer, "\n");
					printf("X : %.1f, Y : %.1f\n", position.x, position.y);

					//system("/home/pi/Desktop/MicroITS/wifi_monitoring_oneshot.sh");	// Write the wireless data (signal strength, quality, name, packets loast) into the file
					//writeFile("../log_oneshot.txt", buffer, SIZE_BUF);	// Write the position of this robot in the file

					strcpy(mes.wifiIp, IP_LOCAL);
					mes.command = COMMAND_GET_POSITION;	// Ask the position of the other robot
					serializeStructMessage(mes, buffer);	// Serialize the message
					writePipe(fd1, buffer, SIZE_BUF);	// Write the message in the pipe for the client process
					sleep(1);	// Wait necessary to be able for the program to work. Don't know why, surely because of the pipes which need latency
					//usleep(500000);
					readPipe(fd2, buffer, SIZE_BUF);

					writeFile("../log_oneshot.txt", buffer, SIZE_BUF);	// Write the position of the other robot in the file

					strcpy(mes.wifiIp, IP_LOCAL);
					mes.command = COMMAND_GET_TIME;	// Command to calculate the time of a round trip of a message between the two robots
					serializeStructMessage(mes, buffer);	// Serialize the message

					writePipe(fd1, buffer, SIZE_BUF);	// Write the message in the pipe for the client process
					sleep(1);	// Wait necessary to be able for the program to work. Don't know why, surely because of the pipes which need latency
					readPipe(fd2, buffer, SIZE_BUF);

					writeFile("../log_oneshot.txt", buffer, SIZE_BUF);	// Write the time of a round trip of a message in the file

					writeFile("../log_oneshot.txt", ";\n", sizeof(";\n"));

					//timer = getTime();	// Refresh the timer variable with the current time
				}*/


				// Send information to the client
				//writeServerSocket(clientSocket, buf, tailleBuf);

			}

			/******* Finishing Process *******/

            // Close pipesq
			close(fd1[1]);
            close(fd2[0]);

            // Stop the motors
            stop(serial);

            // Close the serial communication
            closeSerial(serial);

            // Close the socket
            close(socketMain);

            printf("PID : %d : Waiting for child process to finish\n", getpid());
            wait(NULL); // Wait for the child to finish
            printf("PID : %d : Father finished\n", getpid());
			break;
	}

	return 0;
}
