#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
#include "inertial.h"
#include "motors.h"
#include "buzzerStuff.h"

Zumo32U4ButtonC buttonC;
char inputChar;

bool isDebuging = false;
uint8_t whatToDebug = 0;
int count = 0;

int playSoundId = 0;

inertial inu = inertial();

//input keys manual
void sendManualToPc() {
  Serial1.println("\n\nZUMO MANUAL MODE\n  WASD to move\n  SPACE to stop/continue\n  Q to mote at max speed\n  E to reset rotational movement\n  R to reset movement\n -/+ keys to change volume\n");
}

void setup() {
  Serial.begin(9600);   //start serial connection with the Arduino serial
  Serial1.begin(9600);  //start serial connection with the XCTU application (Xbee serial)

  //inu.setup();

  while (!Serial1) {}
  sendManualToPc();
}
//function to simplify/shorten the use of buzzer in the main loop

int playSoundById(int id) {
  switch (id) {
    case 1:
      play(400, 200);
      break;
    case 2:
      play(420, 200);
      break;
    case 3:
      play(400, 100);
      delay(140);
      play(400, 100);
      break;
    case 4:
      play(540, 80);
      delay(120);
      play(540, 80);
      break;
    case 5:
      play(600, 80);
      delay(120);
      play(600, 100);
      break;
    case 6:
      ledYellow(1);
      play(280, 80);
      delay(50);
      ledYellow(0);
      delay(50);
      ledYellow(1);
      play(280, 80);
      delay(100);
      ledYellow(0);
      play(280, 80);
      break;
    case 7:
      ledYellow(1);
      play(280, 80);
      delay(50);
      ledYellow(0);
      delay(50);
      ledYellow(1);
      play(300, 80);
      delay(100);
      ledYellow(0);
      play(280, 80);
      break;
    case 8:
      play(300, 80);
      delay(120);
      play(280, 80);
      break;
    case 9:
      play(280, 80);
      delay(120);
      play(300, 80);
      break;
  }
  return 0;
}

void loop() {  //when button C is pressed, message how to use the control keys is printed into Serial1 again
  if (buttonC.isPressed()) {
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

      case 'k':
        isDebuging = !isDebuging;
        break;

      case 'j':
        whatToDebug = whatToDebug + 1 % 4;
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

      case 'D':
        rotateDeg(-90);
        break;

      case 'S':
        rotateDeg(-180);
        break;

      case 'X':
        rotateDeg(360);
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


  if (isDebuging) {
    if (whatToDebug == 0) {
      if (whatToDebug == 0) {
        int gyroinfo[3];
        int *value(gyroinfo);
        inu.getGyroPoss(value);
        Serial1.print(value[0]);
        Serial1.print("\t");
        Serial1.print(value[1]);
        Serial1.print("\t");
        Serial1.print(value[2]);
        Serial1.println();
      }
      if (whatToDebug == 1) {
        int gyroinfo[3];
        int *value(gyroinfo);
        inu.getMegData(value);
        Serial1.print(value[0]);
        Serial1.print("\t");
        Serial1.print(value[1]);
        Serial1.print("\t");
        Serial1.print(value[2]);
        Serial1.println();
      }

      if (whatToDebug == 2) {
        int gyroinfo[3];
        int *value(gyroinfo);
        inu.getaccData(value);
        Serial1.print(value[0]);
        Serial1.print("\t");
        Serial1.print(value[1]);
        Serial1.print("\t");
        Serial1.print(value[2]);
        Serial1.println();
      }
    }
  }

  correctOffsetAndApplyMotorValues();
  if (playSoundId > 0) {
    //play a different sound for each input
    playSoundId = playSoundById(playSoundId);
  }
}
