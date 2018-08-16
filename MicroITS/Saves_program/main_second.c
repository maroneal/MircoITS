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
#define IP_Dist					"192.168.1.2"
#define MAX_VEHICLE				10
// Buffer
#define SIZE_BUF				100
// Serial
#define SERIAL_ARDUINO NAME		"/dev/serial0"
#define SERIAL_ARDUINO_SPEED	115200

char end;

// Handler to get the signal SIGINT. It allows to have a clean exit by closing the sockets and the pipes and the serial communication
void handler(int numSignal) {
	if (numSignal == SIGINT) // If signal CTRL+C is sent, we stop the program properly
	{
        end = 1;
	}
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
			clock_t timeBegin, timeEnd;

		    // Initialization process
			configPipeClient(fd1, fd2); // Config pipeline for the client

			//printf("Adresse IP de la source : %s \n", inet_ntoa(sa_S_diff.sin_addr));

			strcat(mes.wifiIp, IP_Dist);
			strcat(mes.gsmIp, "");
			mes.speed=42;
			mes.position.x = 0;
			mes.position.y = 1;
			mes.object = 2;
			mes.distance = 27;
			mes.acceleration = 3;
			mes.direction = 4;
			mes.command = 5;

			configClientSocket(&sa, IP_Dist, PORT_TCP); // Configure the socket to communicate with a distant or local server

			serializeStructMessage(mes, buffer);    // Serialize the structure in a string to be able to send it on the socket

			sleep(1);
			readPipe(fd1, buffer, SIZE_BUF);    // Read the pipe to see if the father is either ready or not
			if(strcmp(buffer, "OK")) printf("PID : %d : FATHER IS NOT OK\n", getpid());	// If the father says he is not ready, we print it in the terminal

			/******* Main loop *******/

			while (!end)
			{
				
			}

				//turnLeft90deg(serial, origin, &position);

				//getPosition(serial, origin, &position);
				//printf("X : %.2f, Y : %.2f\n", position.x, position.y);
				/*n = readPipe(fd1, buffer, SIZE_BUF);

				system("/home/pi/Desktop/MicroITS/wifi_monitoring_oneshot.sh");
				writeFile("../log_oneshot.txt", buffer, SIZE_BUF);
				printf("OK\n");*/
			}

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

			/******* Main loop *******/

			while (!end)
			{
				clientStatus = isClientConnected(socketMain);

                if (clientStatus > 0)    // If something happened on the socket
				{
                    // Accept client to connect
                    clientSocket = acceptClient(socketMain, &sa);  // Get the connection with the client
                    readSocket(clientSocket, buffer, SIZE_BUF);   // Read the message from the client
                    
					writeSocket(clientSocket, buffer, SIZE_BUF);
				}

				// Send information to the client
				//writeServerSocket(clientSocket, buf, tailleBuf);
			}

			/******* Finishing Process *******/

            // Close pipes
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
