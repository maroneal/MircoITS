#include "manageRobot.h"

/********** SERIAL **********/

void closeSerial(int fd)
{
    if(fd != -1) serialClose(fd);
}

int openSerial(char name[], int speed)
{
    int res = 0;
    int serial = 0;
    // Opening serial port
    serial = serialOpen(name, speed);

    // Configuring WiringPI
    if(wiringPiSetup() == -1) res = -1;

    if(res == -1)	// If WiringPI didn't setup correctly
    {
        if(serial != -1) closeSerial(serial);	// Close serial before returning the error
        serial = -1;
    }

    initializeVariables();

    return serial;
}

/********** INITIALIZATION OF VARIABLES**********/

void initializeVariables()
{
    heading = 0.0;
    posX = 0.0;
    posY = 0.0;
    speedMotLeft = 0;
    speedMotRight = 0;
    motorStep = 1;
    count = 0;
    globalSpeed = 0;
    t=0;
}

/********** READ SERIAL FROM ARDUINO **********/

// Read returning response from command

int read_signature(int fd)
{
    char nb_bytes = 6;
    char count;
    char c;

    printf("Signature : ");

    for(count=0; count<nb_bytes; count++)
    {
        while(!serialDataAvail(fd));
        c = serialGetchar(fd);
        printf("%c", c);
    }

    printf("\n");

    return (int)c;
}

int read_line_position(int fd)
{
    int c;
    c = (int)serialGetchar(fd);	// Receives the least significant byte
    while(!serialDataAvail(fd));	// Wait to receive the most significant byte

    c = c + (((int)serialGetchar(fd)) << 8);	// Receives the most significant byte and add it to the first reading

    printf("%d", c);

    return (int)c;
}

int read_res_enc(int fd)
{
    char c;
    c = serialGetchar(fd);

    return (int)c;
}

int read_auto_cal(int fd)
{
    char c = 0;
    char nb_enc = 8;	// NB of encoders to read
    char count = 0;	// Index for the "for" loop
    char enc[20] = {};	// Store the string of the encoders
    int index = 0;

    printf("Encoders : \n");

    for(count=0; count<nb_enc; count++)	// Read all the encoders
    {
        c = 0;
        memset(enc, 0, sizeof(enc));	// Reset the string
        //sprintf(enc, NULL);	// Reset the string
        index = 0;

        while(c != '\r')	// While we are not at the end of the string
        {
            while(!serialDataAvail(fd));	// Wait for a byte
            c = serialGetchar(fd);	// Read the byte
            enc[index++] = c;	// Add it to the string
        }

        printf("%s", enc);	// Print the string
    }

    c = serialGetchar(fd);	// Read for the caracter 'c' if no error
    printf("%c", c);

    if(c != 'c') c = -1;	// If there is an error

    return (int)c;
}

int read_raw_sensors(int fd)
{
    int c = 0;
    char nb_bytes = 10/2;
    char count = 0;

    for(count=0; count<nb_bytes; count++)
    {
        c = (int)serialGetchar(fd);	// Receives the least significant byte
        while(!serialDataAvail(fd));	// Wait to receive the most significant byte

        c = c + (((int)serialGetchar(fd)) << 8);	// Receives the most significant byte and add it to the first reading

        printf("Sensor %d : %d\t",count, c);
    }

    return 0;
}

int read_battery(int fd)
{
    int c;

    c = (int)serialGetchar(fd);	// Receives the least significant byte
    while(!serialDataAvail(fd));	// Wait to receive the most significant byte

    c = c + (((int)serialGetchar(fd)) << 8);	// Receives the most significant byte and add it to the first reading

    printf("Battery : %d", c);

    return c;
}

int read_encoders_value(int fd, int32_t *encM1, int32_t *encM2)
{
    char c;
    *encM1 = 0;
    *encM2 = 0;
    int z;

    for(z=0; z<8; z++)
    {
        while(!serialDataAvail(fd));	// Wait for the buffer to have an information
        c = serialGetchar(fd);	// Get the information

        if(z<4) (*encM1) += c << (z * 8);	// Get the first 4 bytes for motor 1
        else (*encM2) += c << ((z % 4) * 8);	// Get the second 4 bytes for motor 2
    }

    return 0;
}

int readSerial(int fd, char command)
{
    int response = 0;
    while(!serialDataAvail(fd));	// Wait for the Arduino to response

    switch(command)
    {
    // receives signature and stops the motors
    case (char)COMMAND_SIGNATURE:
        response = read_signature(fd);
        break;

    // receives raw sensor values
    case (char)COMMAND_RAW_SENSORS:
        response = read_raw_sensors(fd);
        break;

    // receives zero values if no calibration has been performed yet
    case (char)COMMAND_CAL_SENSORS:

        break;

    // TODO: receives trimpot voltage in millivolts
    case (char)0xB0:

        break;

    // TODO: receives battery voltage in millivolts
    case (char)COMMAND_BATTERY:
        response = read_battery(fd);
        break;

    // calibrates the IR sensors and receives values after calibration
    case (char)COMMAND_CALIBRATE:

        break;

    // receives current line position (0:4000)
    case (char)COMMAND_LINE_POSITION:
        response = read_line_position(fd);
        break;

    // receives the current encoder values from both motors
    case (char)COMMAND_ENCODERS:

        break;

    // resets the encoder counters to zero
    case (char)COMMAND_RES_ENC:
        response = read_res_enc(fd);
        break;

    // starts auto-calibration by turning 90° left and right
    case (char)COMMAND_AUTO_CAL:
        response = read_auto_cal(fd);
        break;

    }

    printf("\n");

    return response;
}

/********** SEND SERIAL TO ARDUINO **********/
// Send command
void sendCommand(int fd, char command)
{
    if(command >= COMMAND_FORWARD_PRESSED && command <= COMMAND_SPACE_PRESSED)
    {
        manageMotorPressed(fd, command);
    }
    else if(command >= COMMAND_FORWARD_RELEASED && command <= COMMAND_SPACE_RELEASED)
    {
        manageMotorReleased(fd, command);
    }
    else if (command >= CAMERA_ON_LINE && command <= CAMERA_NO_LINE)
    {
        if(command == CAMERA_TURN_LEFT) turnLeft(fd);
        else if (command == CAMERA_TURN_RIGHT) turnRight(fd);
        else if (command == CAMERA_ON_LINE) goForward(fd);
        else if (command == CAMERA_NO_LINE) stop(fd);
        else if (command == CAMERA_OBJECT_DETECTED) decreaseSpeed(fd);
    }
    else if (command == COMMAND_NOTHING)
    {

    }
    else //if (command >= COMMAND_SIGNATURE && command <= COMMAND_M2_BACKWARD)
    {
        serialPutchar(fd, command);
    }
}

/********** INITIALIZATION OF THE ROBOT **********/

void configRobot(int fd)
{
    speedMotLeft = 0;
    speedMotRight = 0;
    motorStep = 1;
    count = 0;
    globalSpeed = 0;

    printf("Start of initialization of the robot\n");

    serialPutchar(fd, COMMAND_SIGNATURE);
    readSerial(fd, COMMAND_SIGNATURE);

    serialPutchar(fd, COMMAND_RES_CAL);

    serialPutchar(fd, COMMAND_AUTO_CAL);
    readSerial(fd, COMMAND_AUTO_CAL);

    serialPutchar(fd, COMMAND_BATTERY);
    readSerial(fd, COMMAND_BATTERY);
}

void initRobot()
{
    speedMotLeft = 0;
    speedMotRight = 0;
    motorStep = 1;
    count = 0;
    globalSpeed = 0;
}

/********** MOTOR MANAGER **********/

// Increment the step in case of the user stays on a pressed button (while controlling manually the robot)
void incrementMotorStep()
{
    count = (count + 1) % 5;

    if(count == 0)
    {
        if((motorStep + 1) > MOTOR_STEP_MAX) motorStep = MOTOR_STEP_MAX;	// Anti windup
        else motorStep += 1;
    }
}

// Allow the program to set manually the motor step
void setMotorStep(unsigned char step)
{
    motorStep = step;
}

// Avoid the motor to get a value above the maximum value
int antiWindUp(int enter)
{
    int var = enter;

    if(var > MAX_SPEED) var = MAX_SPEED;
    else if(var < MIN_SPEED) var = MIN_SPEED;

    return var;
}

// Set the motor speed of the left motor
void setM1Speed(int fd, int speed)
{
    unsigned char reverse = 0;

    if (speed < 0)
    {
        speed = -speed;	// make speed a positive quantity
        reverse = 1;	// preserve the direction
    }

    if (speed > 127)	// Anti windup
        speed = 127;

    if (reverse)
    {
        sendCommand(fd, COMMAND_M1_BACKWARD);
        sendCommand(fd, speed);
    }
    else	// forward
    {
        sendCommand(fd, COMMAND_M1_FORWARD);
        sendCommand(fd, speed);
    }
}

// Set the motor speed of the right motor
void setM2Speed(int fd, int speed)
{
    unsigned char reverse = 0;

    if (speed < 0)
    {
        speed = -speed;	// make speed a positive quantity
        reverse = 1;	// preserve the direction
    }

    if (speed > 127)	// Anti windup
        speed = 127;

    if (reverse)
    {
        sendCommand(fd, COMMAND_M2_BACKWARD);
        sendCommand(fd, speed);
    }
    else	// forward
    {
        sendCommand(fd, COMMAND_M2_FORWARD);
        sendCommand(fd, speed);
    }
}

// Update of the speed of the two motors with the turning parameter
void stdUpdate(int tempo)
{
    if(tempo == 0)  // If we go straight forward, the two motors have the same speed
    {
        speedMotLeft = globalSpeed;
        speedMotRight = globalSpeed;
    }
    else if (tempo > 0) // Turn right
    {
        speedMotLeft = globalSpeed;
        speedMotRight = globalSpeed * (TMAX - tempo) / TMAX;
    }
    else if (tempo < 0) // Turn left
    {
        speedMotLeft = globalSpeed * (TMIN - tempo) / TMIN;
    }

    speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup
    speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup
}

// Manage the motors when a user presses a key on the user interface (while controlling manually the robot)
void manageMotorPressed(int fd, char c)
{
    switch(c)
    {
    case COMMAND_FORWARD_PRESSED:	// Forward

        incrementMotorStep();	// Increment the motor step if the key is long pressed
        globalSpeed += motorStep;	// Add the motor step the the speed of the motor
        globalSpeed = antiWindUp(globalSpeed);
        stdUpdate(t);   // Update the speed of the two motors depending of the heading

        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
        break;

    case COMMAND_RIGHT_PRESSED:	// Turn right
        //setMotorStep(MOTOR_STEP_TURNING);
        if(++t > TMAX) t = TMAX;
        printf("T : %d\n", t);

        stdUpdate(t);   // Update the speed of the two motors depending of the heading

        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
        setM1Speed(fd, speedMotLeft);
        break;

    case COMMAND_BACKWARD_PRESSED:	// Backward
        incrementMotorStep();	// Increment the motor step if the key is long pressed
        globalSpeed -= motorStep;
        globalSpeed = antiWindUp(globalSpeed);

        stdUpdate(t);   // Update the speed of the two motors depending of the heading

        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
        break;

    case COMMAND_LEFT_PRESSED:	// Turn left
        if(--t < TMIN) t = TMIN;
        printf("T : %d\n", t);

        stdUpdate(t);   // Update the speed of the two motors depending of the heading

        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
        setM2Speed(fd, speedMotRight);

        break;

    case COMMAND_SPACE_PRESSED:	// Stop
        // Stop all motors by giving them a 0 speed
        speedMotLeft = 0;
        speedMotRight = 0;
        globalSpeed = 0;
        t = 0;

        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
        break;

    }
}

// Manage the motors when a user releases a key on the user interface (while controlling manually the robot)
void manageMotorReleased(int fd, char c)
{
    // All motors take their previous speed
    //speedMotLeft = globalSpeed;
    //speedMotRight = globalSpeed;
    motorStep = 1;

    switch(c)
    {
    case COMMAND_RIGHT_RELEASED:
        //setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
        //setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
        break;

    case COMMAND_LEFT_RELEASED:
        //setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
        //setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
        break;
    }
}

void turnRight(int fd)
{
    //speedMotLeft += STEP_TURN;
    //speedMotRight -= STEP_TURN;
    //globalSpeed = 25;
    if(globalSpeed < 10) globalSpeed = 10;
    t = TMAX;
	stdUpdate(t);

    setM1Speed(fd, speedMotLeft);
    setM2Speed(fd, speedMotRight);
}

void turnLeft(int fd)
{
    //speedMotLeft += STEP_TURN;
    //speedMotRight -= STEP_TURN;
    //globalSpeed = 25;
    if(globalSpeed < 10) globalSpeed = 10;
    t = TMIN;
	stdUpdate(t);

    setM1Speed(fd, speedMotLeft);
    setM2Speed(fd, speedMotRight);
}

void goForward(int fd)
{
    if(++globalSpeed > 27) globalSpeed = 27;
    //globalSpeed = 25;
    t = 0;
    stdUpdate(t);

    setM1Speed(fd, speedMotLeft);
    setM2Speed(fd, speedMotRight);
}

void decreaseSpeed(int fd)
{
    if(--globalSpeed < 0) globalSpeed = 0;
    stdUpdate(t);

    setM1Speed(fd, speedMotLeft);
    setM2Speed(fd, speedMotRight);
}

void stop(int fd)
{
    globalSpeed = 0;
    t = 0;
    stdUpdate(t);

    setM1Speed(fd, speedMotLeft);
    setM2Speed(fd, speedMotRight);
}

// Reset the encoders value of the motors of the robot
char resetEncoders(int fd)
{
    char c;
    setM1Speed(fd, 0);
    sleep(1);
    sendCommand(fd, COMMAND_RES_ENC);
    c = (char) readSerial(fd, COMMAND_RES_ENC);
    sleep(1);

    if(c != 'r' && c!= 'R') printf("Error during the reset of the encoders\n");

    return c;
}

// Convert the values of the encoders into a cartesian plan
void encoderToCartesian(int32_t left_cnt, int32_t right_cnt, point *p)
{
    double DIAMETER = 85.0;
    double DIAMETER_WHEEL = 32.0;
    int COUNTS_REVOLUTION = 600;

    static double last_left_cnt = 0.0;
    static double last_right_cnt = 0.0;

    double distancePerCount = PI * DIAMETER_WHEEL / COUNTS_REVOLUTION;	// Gets the distance made by a wheel when an encoder is incremented
    //double deltaDistance = (left_cnt + right_cnt) / 2.0 * distancePerCount;
    //double countsPerRotation = (DIAMETER / DIAMETER_WHEEL) * COUNTS_REVOLUTION;

    double radiansPerCount = PI * (DIAMETER_WHEEL / DIAMETER) / COUNTS_REVOLUTION;	// Gets the angle (in radians) done when an encoder is incremented

    double deltaLeft = left_cnt - last_left_cnt;
    double deltaRight = right_cnt - last_right_cnt;
    double deltaDistance = 0.5 * (double) (deltaLeft + deltaRight) * distancePerCount;

    //double deltaHeading = (double) (right_cnt - left_cnt) * radiansPerCount;
    double deltaHeading = (double) (deltaRight - deltaLeft) * radiansPerCount;

    heading += deltaHeading;	// Add the angle made on the angle of the robot

    // Limits heading to -Pi <= heading < Pi
    if (heading > PI)
        heading -= TWO_PI;
    else if (heading <= -PI)
        heading += TWO_PI;

    double deltaX = deltaDistance * (double)cos(heading);	// Calcules the distance made on X axis whith projection in cartesian plane
    double deltaY = deltaDistance * (double)sin(heading);	// Calcules the distance made on Y axis whith projection in cartesian plane

    posX += deltaX;	// Add the X distance made on the X distance already made by the robot
    posY += deltaY;	// Add the Y distance made on the Y distance already made by the robot

    last_left_cnt = left_cnt;	// Stores the encoders values
    last_right_cnt = right_cnt;

    p->x = posX;	// Stores the position in the point received in argument of the function
    p->y = posY;
}

// Get the position of the robot on the platoon with the encoders values
void getPosition(int fd, point origin, point *actual)
{
    int32_t e1=0, e2=0;
    sendCommand(fd, COMMAND_ENCODERS);
    read_encoders_value(fd, &e1, &e2);

    encoderToCartesian(e1, -e2, actual);
    actual->x = (actual->x) - (origin.x);
    actual->y = (actual->y) - (origin.y);
}

// Reset the origin of the cartesian plan
void resetOrigin(int fd, point *p)
{
    int32_t e1=0, e2=0;

    sendCommand(fd, COMMAND_ENCODERS);
    read_encoders_value(fd, &e1, &e2);

    encoderToCartesian(e1, e2, p);
}

/****************************************/
/****** OTHER METHOD OF TURNING ******/
/****************************************/
/*void manageMotorReleased(int fd, char c)
{
	// All motors take their previous speed
	speedMotLeft = globalSpeed;
	speedMotRight = globalSpeed;
	motorStep = 1;

	switch(c)
	{
	case COMMAND_RIGHT_RELEASED:
		setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
		setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
		break;

	case COMMAND_LEFT_RELEASED:
		setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
		setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
		break;
	}
}*/

/****************************************/
/****** OTHER METHOD OF TURNING ******/
/****************************************/

/*void manageMotorPressed(int fd, char c)
{
	switch(c)
	{
	case COMMAND_FORWARD_PRESSED:	// Forward

		incrementMotorStep();	// Increment the motor step if the key is long pressed

		speedMotLeft += motorStep;	// Add the motor step the the speed of the motor
		speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup

		setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor

		speedMotRight += motorStep;	// Add the motor step the the speed of the motor
		speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup

		setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor

		globalSpeed = speedMotRight;	// Set the global speed to the current speed
		break;

	case COMMAND_RIGHT_PRESSED:	// Turn right

		setMotorStep(MOTOR_STEP_TURNING);	// Set the motor step to a constant
		//speedMotLeft += motorStep;	// Add the motor step the the speed of the motor
		speedMotLeft = (int) (globalSpeed + globalSpeed/ANGLE_TURN);
		speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup

		setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor

		//speedMotRight -= motorStep;	// Add the motor step the the speed of the motor
		speedMotRight = (int) (globalSpeed - globalSpeed/ANGLE_TURN);
		speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup

		setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
		break;

	case COMMAND_BACKWARD_PRESSED:	// Backward

		incrementMotorStep();	// Increment the motor step if the key is long pressed

		speedMotLeft -= motorStep;	// Add the motor step the the speed of the motor
		speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup

		setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor

		speedMotRight -= motorStep;	// Add the motor step the the speed of the motor
		speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup

		setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor

		globalSpeed = speedMotLeft;	// Set the global speed to the current speed
		break;

	case COMMAND_LEFT_PRESSED:	// Turn left

		setMotorStep(MOTOR_STEP_TURNING);
		//speedMotLeft -= motorStep;	// Add the motor step the the speed of the motor
		speedMotLeft = (int) (globalSpeed - globalSpeed/ANGLE_TURN);
		speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup

		setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor

		//speedMotRight += motorStep;	// Add the motor step the the speed of the motor

		speedMotRight = (int) (globalSpeed + globalSpeed/ANGLE_TURN);
		speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup

		setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
		break;

	case COMMAND_SPACE_PRESSED:	// Stop
		// Stop all motors by giving them a 0 speed
		speedMotLeft = 0;
		speedMotRight = 0;
		globalSpeed = 0;

		setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
		setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
		break;

	}
}*/

/*****************************/
/****** SAVED FUNCTION ******/
/*****************************/

//void manageMotorPressed(int fd, char c)
//{
//    switch(c)
//    {
//    case COMMAND_FORWARD_PRESSED:	// Forward
//
//        incrementMotorStep();	// Increment the motor step if the key is long pressed
//
//        speedMotLeft += motorStep;	// Add the motor step the the speed of the motor
//        speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup
//
//        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
//
//        speedMotRight += motorStep;	// Add the motor step the the speed of the motor
//        speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup
//
//        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
//
//        globalSpeed = speedMotRight;	// Set the global speed to the current speed
//        break;
//
//    case COMMAND_RIGHT_PRESSED:	// Turn right
//        setMotorStep(MOTOR_STEP_TURNING);
//        speedMotLeft += motorStep;	// Add the motor step the the speed of the motor
//        //speedMotLeft = (int) (globalSpeed + globalSpeed/ANGLE_TURN);
//        speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup
//
//        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
//
//        speedMotRight -= motorStep;	// Add the motor step the the speed of the motor
//        //speedMotRight = (int) (globalSpeed - globalSpeed/ANGLE_TURN);
//        speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup
//
//        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
//        break;
//
//    case COMMAND_BACKWARD_PRESSED:	// Backward
//        incrementMotorStep();	// Increment the motor step if the key is long pressed
//
//        speedMotLeft -= motorStep;	// Add the motor step the the speed of the motor
//        speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup
//
//        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
//
//        speedMotRight -= motorStep;	// Add the motor step the the speed of the motor
//        speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup
//
//        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
//
//        globalSpeed = speedMotLeft;	// Set the global speed to the current speed
//        break;
//
//    case COMMAND_LEFT_PRESSED:	// Turn left
//        setMotorStep(MOTOR_STEP_TURNING);
//        speedMotLeft -= motorStep;	// Add the motor step the the speed of the motor
//        //speedMotLeft = (int) (globalSpeed - globalSpeed/ANGLE_TURN);
//        speedMotLeft = antiWindUp(speedMotLeft);	// Check the anti windup
//
//        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
//
//        speedMotRight += motorStep;	// Add the motor step the the speed of the motor
//
//        //speedMotRight = (int) (globalSpeed + globalSpeed/ANGLE_TURN);
//        speedMotRight = antiWindUp(speedMotRight);	// Check the anti windup
//
//        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
//        break;
//
//    case COMMAND_SPACE_PRESSED:	// Stop
//        // Stop all motors by giving them a 0 speed
//        speedMotLeft = 0;
//        speedMotRight = 0;
//        globalSpeed = 0;
//
//        setM1Speed(fd, speedMotLeft);	// Set the new speed to the configuration of the motor
//        setM2Speed(fd, speedMotRight);	// Set the new speed to the configuration of the motor
//        break;
//
//    }
//}

/********** NOT WORKING **********/
/*void turnLeft90deg(int fd, point origin, point *position)
{
    double h = 0;

    stop(fd);

    h = heading;

    speedMotLeft = -20;
    speedMotRight = 20;
    setM1Speed(fd, speedMotLeft);
    setM2Speed(fd, speedMotRight);

    while(h+heading < PI/2)
    {
        printf("H : %.2f \t Heading : %.2f\n", h, heading);
        printf("JACK\n");
        getPosition(fd, origin, position);
    }

    printf("SORTIE\n");

    stop(fd);
}*/
