#include "motors.h"
#include "buzzerStuff.h"

bool inputReceivedManual() {

  if (Serial1.available()) {         //if character is received form Serial1
    int inputChar = Serial1.read();  //stores received character into a char varible

    switch (inputChar) {
      case '-':   //if input char is "-", call lowerVolume() function. The same is done for every other key which call other functions
        lowerVolume();
        break;

      case '=':
        increaseVolume();
        break;

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
    return true;
  }
  return false;
}

void inputReceivedAutonomous() {
if (Serial1.available()) {

    switch (Serial1.read()) {
      case 'z':
        drivingMode = 0;
        manualModeSound();
        break;

      case 'x':
        drivingMode = 1;
        autonomousModeSound();
        break;

      case 'c':
        drivingMode = 2;
        startupSound();
        break;
    }
  }
}