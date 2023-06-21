#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
#include "inertial.h"
#include "motors.h"
#include "buzzerStuff.h"
#include "encoderStuff.h"
#include "lineFollower.h"
#include "blockPusher.h"

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonC buttonC_2;
blockPusher bp;

char inputChar;

bool isDebuging = false;
uint8_t whatToDebug = 0;
int count = 0;
int drivingMode = 0;
int playSoundId = 0;

inertial inu = inertial();

//input keys manual
void sendManualToPc() {
  Serial1.println("\n\nZUMO MANUAL MODE\n  WASD to move\n  SPACE to stop/continue\n  Q to mote at max speed\n  E to reset rotational movement\n  R to reset movement\n -/+ keys to change volume\n");
}

void setup() {
  Serial.begin(9600);   //start serial connection with the Arduino serial
  Serial1.begin(9600);  //start serial connection with the XCTU application (Xbee serial)

  bp.setup();


  while (!Serial1) {}
  sendManualToPc();

  lineSensorsInitFiveSensors();

  startupSound();
}
//function to simplify/shorten the use of buzzer in the main loop

void manualMode() {  //when button C is pressed, message how to use the control keys is printed into Serial1 again
  if (buttonC_2.getSingleDebouncedPress()) {
    sendManualToPc();
  }

  if (Serial1.available())  //if character is received form Serial1
  {
    inputChar = Serial1.read();  //stores received character into a char varible

    switch (inputChar)  //a switch is used instead of multiple if-else statements to make code more readable and optimized
    {
      case '-':  //if input char is "-", call lowerVolume() function. The same is done for every other key which call other functions
        lowerVolume();

        break;  //break out of switch

      case '=':
        increaseVolume();
        break;

      // case 'k':
      //   isDebuging = !isDebuging;
      //   break;

      // case 'j':
      //   whatToDebug = whatToDebug + 1 % 4;
      //   break;

      case 'a':
        moveLeft();
        playSoundId = 1;
        break;

      case 'd':
        moveRight();
        playSoundId = 2;
        break;

      case 's':
        moveSlower();
        playSoundId = 3;
        break;

      case 'w':
        moveFaster();
        playSoundId = 4;
        break;

      case 'q':
        moveToMaxSpeed();
        playSoundId = 5;
        break;

      case 'R':
      case 'r':
        resetSpeed();
        playSoundId = 6;
        break;

      case 'e':
        resetRotationalMovement();
        playSoundId = 7;
        break;

      case 'A':
        rotateDeg(90);
        break;

      case 'S':
        rotateDeg(-90);
        break;

      case 'D':
        rotateDeg(-180);
        break;

      case 'F':
        rotateDeg(360);
        break;

      case 'x':
        drivingMode = 1;
        autonomousModeSound();
        break;

      default:  //"default:" is ran if none of other cases were activated. This is needed for the ' ' character (SPACEBAR) because this gives an error in a regular case.
        if (inputChar == ' ') {
          stopContinue();
          if (isAllowDrive()) {
            playSoundId = 8;
          } else {
            playSoundId = 9;
          }
        }
        break;
    }

    //these statements set the motor speeds to the minimum or maximum allowed value if these are above or below allowed vaues
    setAndNormalizeMotorValues();
  }

  if (!isAllowDrive()) {
    count++;
    if (count > 5) {
      play(300, 40);
      count = 0;
    }
    ledRed(1);
    delay(50);
    ledRed(0);
    delay(100);
  }

  if (isStandingStill()) {
    ledGreen(0);
  } else {
    ledGreen(1);
  }

  correctOffset();
  applyMotorValues();
}

void blokMode() {
  bp.duwBlok();
  if (Serial1.available())  //if character is received form Serial1
  {
    inputChar = Serial1.read();  //stores received character into a char varible

    switch (inputChar)  //a switch is used instead of multiple if-else statements to make code more readable and optimized
    {
      case 'z':
        drivingMode = 0;
        manualModeSound();
        resetEncoderCounts();
        break;

      case 'x':
        drivingMode = 1;
        autonomousModeSound();
        break;
    }
  }
}

void autonomousMode() {
  lineFollow();
  if (getCalibratedCount() != 5) {
    resetSpeed();
  }
  applyMotorValues();

  if (Serial1.available())  //if character is received form Serial1
  {
    inputChar = Serial1.read();  //stores received character into a char varible

    switch (inputChar)  //a switch is used instead of multiple if-else statements to make code more readable and optimized
    {
      case 'k':
        isDebuging = !isDebuging;
        break;

      case 'j':
        whatToDebug = whatToDebug + 1 % 4;
        break;

      case 'z':
        drivingMode = 0;
        manualModeSound();
        break;

      case 'c':
        drivingMode = 2;
        startupSound();
        break;
    }
  }
}

void loop() {

  if (buttonA.getSingleDebouncedPress()) {
    drivingMode = (drivingMode + 1) % 3;
    if (drivingMode == 1) {
      autonomousModeSound();
    }
    else if (drivingMode == 2) {
     autonomousModeSound(); 
    }
    else {
      manualModeSound();
    }
    resetSpeed();
  }

  if (drivingMode == 1) {
    autonomousMode();
  } else if (drivingMode == 2) {
    blokMode();
  } else {
    manualMode();
  }

  if (playSoundId > 0) {
    //play a different sound for each id
    playSoundId = playSoundById(playSoundId);
  }
}
