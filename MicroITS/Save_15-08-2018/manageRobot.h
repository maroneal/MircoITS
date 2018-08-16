#ifndef MANAGEROBOT_H_INCLUDED
#define MANAGEROBOT_H_INCLUDED

#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "manageNetwork.h"

#define COMMAND_SIGNATURE			0x81
#define COMMAND_RAW_SENSORS			0x86
#define COMMAND_CAL_SENSORS			0x87
#define COMMAND_BATTERY				0xB1
#define COMMAND_CALIBRATE			0xB4
#define COMMAND_RES_CAL				0xB5
#define COMMAND_LINE_POSITION		0xB6
#define COMMAND_ENCODERS			0xB7
#define COMMAND_RES_ENC				0xB8
#define COMMAND_AUTO_CAL			0xBA
#define COMMAND_START_PID			0xBB
#define COMMAND_STOP_PID			0xBC
#define COMMAND_M1_FORWARD			0xC6
#define COMMAND_M1_BACKWARD			0xC5
#define COMMAND_M2_FORWARD			0xC1
#define COMMAND_M2_BACKWARD			0xC2
#define COMMAND_FORWARD_PRESSED		0xA0
#define COMMAND_RIGHT_PRESSED		0xA1
#define COMMAND_BACKWARD_PRESSED	0xA2
#define COMMAND_LEFT_PRESSED		0xA3
#define COMMAND_SPACE_PRESSED		0xA4
#define COMMAND_FORWARD_RELEASED	0xA5
#define COMMAND_RIGHT_RELEASED		0xA6
#define COMMAND_BACKWARD_RELEASED	0xA7
#define COMMAND_LEFT_RELEASED		0xA8
#define COMMAND_SPACE_RELEASED		0xA9
#define CAMERA_ON_LINE				0xAC
#define CAMERA_TURN_LEFT			0xAD
#define CAMERA_TURN_RIGHT			0xAE
#define CAMERA_NO_LINE				0xAF
#define COMMAND_NOTHING				0x80

#define MOTOR_STEP_MAX				10
#define MOTOR_STEP_TURNING			2
#define MAX_SPEED					127
#define MIN_SPEED					-127
#define ANGLE_TURN					2
#define SIZE_WHEEL					32
#define MOTOR_REDUCTION				50
#define COUNT_PER_TOUR				12
#define STEP_TURN					20

#define PI 3.14159265359
#define TWO_PI 6.2831853070
#define RAD2DEG 57.2958 /* radians to degrees conversion */

#define TMAX                        10
#define TMIN                        -10

int speedMotLeft;
int speedMotRight;
unsigned char motorStep;
int count;
int globalSpeed;
int t;

double heading;
double posX;
double posY;

// Setup serial
int openSerial(char name[], int speed);
void closeSerial(int fd);

// Initialization
void initializeVariables();
void initRobot();

// Send command
void configRobot(int fd);
void sendCommand(int fd, char command);

// Read returning response from command
int readSerial(int fd, char command);
int read_signature(int fd);
int read_line_position(int fd);
int read_res_enc(int fd);
int read_auto_cal(int fd);
int read_raw_sensors(int fd);
int read_battery(int fd);
int read_encoders_value(int fd, int32_t *encM1, int32_t *encM2);

// Manage motors
void incrementMotorStep();
void setMotorStep(unsigned char step);
int antiWindUp(int enter);
void setM1Speed(int fd, int speed);
void setM2Speed(int fd, int speed);
void manageMotorPressed(int fd, char c);
void manageMotorReleased(int fd, char c);
void turnLeft(int fd);
void turnRight(int fd);
void goForward(int fd);
void stop(int fd);

// Encoders
char resetEncoders(int fd);
void resetOrigin(int fd, point *actual);

#endif // MANAGEROBOT_H_INCLUDED
