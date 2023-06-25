#include "motors.h"
#include "buzzerStuff.h"

buzzerStuff buzzer

bool xBee::inputReceivedManual() {
  //if character is received form Serial1
  if (Serial1.available()) {
    //stores received character into a char varible
    int inputChar = Serial1.read();

    switch (inputChar) {
      //if input char is "-", call lowerVolume() function. The same is done for every other key which call other functions
      case '-':
        buzzer.lowerVolume();
        break;

      case '=':
        buzzer.increaseVolume();
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
        buzzer.autonomousModeSound();
        break;
      
      //"default:" is ran if none of other cases were activated. This is needed for the ' ' character (SPACEBAR) because this gives an error in a regular case.
      default:
        if (inputChar == ' ') {
          stopContinue();
          if (isAllowDrive()) {
            buzzer.playSoundId = 8;
          } else {
            buzzer.playSoundId = 9;
          }
        }
        break;
    }
    return true;
  }
  return false;
}

void xBee::inputReceivedAutonomous() {
if (Serial1.available()) {

    switch (Serial1.read()) {
      case 'z':
        drivingMode = 0;
        buzzer.manualModeSound();
        break;

      case 'x':
        drivingMode = 1;
        buzzer.autonomousModeSound();
        break;

      case 'c':
        drivingMode = 2;
        buzzer.startupSound();
        break;
    }
  }
}