#include "xBee.h"

extern int drivingMode;

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
        m.moveLeft();
        buzzer.playSoundId = 1;
        break;

      case 'd':
        m.moveRight();
        buzzer.playSoundId = 2;
        break;

      case 's':
        m.moveSlower();
        buzzer.playSoundId = 3;
        break;

      case 'w':
        m.moveFaster();
        buzzer.playSoundId = 4;
        break;

      case 'q':
        m.moveToMaxSpeed();
        buzzer.playSoundId = 5;
        break;

      case 'R':
      case 'r':
        m.resetSpeed();
        buzzer.playSoundId = 6;
        break;

      case 'e':
        m.resetRotationalMovement();
        buzzer.playSoundId = 7;
        break;

      case 'A':
        m.rotateDeg(90);
        break;

      case 'S':
        m.rotateDeg(-90);
        break;

      case 'D':
        m.rotateDeg(-180);
        break;

      case 'F':
        m.rotateDeg(360);
        break;

      case 'x':
        drivingMode = 1;
        buzzer.autonomousModeSound();
        break;
      
      //"default:" is ran if none of other cases were activated. This is needed for the ' ' character (SPACEBAR) because this gives an error in a regular case.
      default:
        if (inputChar == ' ') {
          m.stopContinue();
          if (m.isAllowDrive()) {
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
    m.setAndNormalizeMotorValues();
}
void xBee::manualLoop() {
  m.applyMotorValues();
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
