/**
NORAA (NOn Representational Art Automata) server software

NORAA FIRMWARE ON ARDUINO MEGA 2560 BOARD + LADYADA MOTOR SHIELD V 1.2
2 X NEMA17 STEPPER MOTOR (200 STEP - 2mm x 20teeth PULLEY) + 2 SERVO MOTOR

first created: 01.AUG.2014
last modified: 13.JAN.2017

Copyright (C) 2015  Patrick Tabarelli
patrick.tabarelli@gmail.com - www.patricktabarelli.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/
*/

#include <AccelStepper.h>
#include <AFMotor.h>
#include <Servo.h>

// ##############################################################################################################
// ##############################################################################################################
// SETTINGS - VARIABLES DECLARATION
// ##############################################################################################################
//set width(X)/height(Y) max (step base) considering portrait size (X shortest axis)

String DEVICE_NAME = "noraa8060";
String DEVICE_FIRMWARE = "v08_161112";

//microstepping
#define MICROSTEPS 1

//GT2 pulley - 20 teeth - 40mm per 200 step revolution -->0,2mm per step--------------------------------> PULLEYS
//how many steps to make one mm? 200steps/20teeth/2mm*tooth || 200steps/2pi*radius(mm)
#define PULLEY_X (200/20/2*MICROSTEPS)
#define PULLEY_Y (200/20/2*MICROSTEPS)


// surface max dimension in step------------------------------------------------------------------------> SURFACE
#define DRAWING_SURFACE_WIDTH_MM 425
#define DRAWING_SURFACE_HEIGHT_MM 655

#define SURFACE_WIDTH (DRAWING_SURFACE_WIDTH_MM * PULLEY_X)
#define SURFACE_HEIGHT (DRAWING_SURFACE_HEIGHT_MM * PULLEY_Y)

//set angle for servo Up/Down----------------------------------------------------------------------------> SERVOS
#define SERVO_1_UP_DOWN_PIN 9 
int ANGLE_PEN_UP = 170;
int ANGLE_PEN_DOWN = 10;

//set angle for servo Open/Close--grab angle at 125 ca.
#define SERVO_2_OPEN_CLOSE_PIN 10
int ANGLE_PEN_OPEN = 10;
int ANGLE_PEN_CLOSE = 170;

//set servo motors delay
#define PEN_DELAY 250

//set stepper speed-------------------------------------------------------------> STEPPERS VELOCITY & ACCELERATION
int SPEED_MOTOR_X_AXIS = 200;
int SPEED_MOTOR_Y_AXIS = 200;

//set stepper accelleration
int ACCELERATION_MOTOR_X_AXIS = 700;
int ACCELERATION_MOTOR_Y_AXIS = 700;

// define limit switch X and Y---------------------------------------------------------------------> LIMIT SWITCH
#define ENDSTOP_PIN_X_AXIS 22
#define ENDSTOP_PIN_Y_AXIS 24

//serial variables-----------------------------------------------------------------------------> SERIAL VARIABLES
#define BAUDRATE 115200
char INCOMING_BYTE;
String BUFFER;
#define MAX_BUFFER_LENGTH 256

//comment and DEBUG
//if false there is no comments and it is sensibly faster-------------------------------------------------> DEBUG
boolean DEBUG = false;
//more is less. this works as a filter. with zero(0) VERBOSITY you have all the DEBUG feedback, increase to sort it 
int VERBOSITY = 0;

//time variables-------------------------------------------------------------------------,-----------------> TIME
unsigned long LAST_COMMAND_TIME;
int timeOut = 1000;

// ##############################################################################################################
// SETTINGS - MOVEMENT FUNCTIONS
// ##############################################################################################################

// DC hobby servo
Servo servo1, servo2;

// Stepper motor on M1+M2 200 steps per revolution
// Motor for X axis
AF_Stepper motorX(200, 1);
// Stepper motor on M3+M4 200 steps per revolution
// Motor for Y axis
AF_Stepper motorY(200, 2);

// you can change these to SINGLE or DOUBLE or INTERLEAVE or MICROSTEP!
//Forward-Backward swapped for layout reason
void forwardstepX() {
  motorX.onestep(FORWARD, SINGLE);
}
void backwardstepX() {
  motorX.onestep(BACKWARD, SINGLE);
}

AccelStepper stepperX(forwardstepX, backwardstepX); // use functions to step

// you can change these to SINGLE or DOUBLE or INTERLEAVE or MICROSTEP!
void forwardstepY() {
  motorY.onestep(FORWARD, SINGLE);
}
void backwardstepY() {
  motorY.onestep(BACKWARD, SINGLE);
}

AccelStepper stepperY(forwardstepY, backwardstepY); // use functions to step

// ###############################################################################################################
// VOID SETUP BLOCK
// ###############################################################################################################

void setup(){
  loadConfig();

  // set up Serial communication at specific baudrate
  Serial.begin(BAUDRATE);
  clearBuffer();

  // take motors to origin
  homing();

  stepperX.setMinPulseWidth(20);
  stepperY.setMinPulseWidth(20);

  Serial.println("::::::");
  Serial.print("device: ");
  Serial.println(DEVICE_NAME);
  Serial.print("firmware: ");
  Serial.println(DEVICE_FIRMWARE);
  Serial.println("::::::");

  //init variable
  LAST_COMMAND_TIME = millis();
}

// ###############################################################################################################
// VOID LOOP BLOCK
// ###############################################################################################################

void loop() {
  getCommand();
  checkTimeOut();
}
