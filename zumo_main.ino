#include <Wire.h>
#include <Zumo32U4.h>
#include <math.h>
#include "inertial.h"
#include "motors.h"
#include "buzzerStuff.h"
#include "encoderStuff.h"
#include "lineFollower.h"
#include "blockPusher.h"
#include "xBee.h"

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonC buttonC;
blockPusher bp;
lineFollower lineFollowerObj;
buzzerStuff buzzerObj;
xBee xBees;
encoderStuff encoder;

char inputChar;
bool isDebuging = false;
uint8_t whatToDebug = 0;
int count = 0;
int drivingMode = 0;
int playSoundId = 0;
inertial inu = inertial();

void sendManualToPc() {
  Serial1.println("\n\nZUMO MANUAL MODE\n  WASD to move\n  SPACE to stop/continue\n  Q to mote at max speed\n  E to reset rotational movement\n  R to reset movement\n -/+ keys to change volume\n");
}

void setup() {
  //start serial connection with the Arduino serial
  Serial.begin(9600);
  //start serial connection with the XCTU application (Xbee serial)
  Serial1.begin(9600);
  bp.setup();
  while (!Serial1) {}
  sendManualToPc();
  lineFollowerObj.lineSensorsInitFiveSensors();
  buzzerObj.startupSound();
}

void manualMode() {
  if (buttonC.getSingleDebouncedPress()) {
    //when button C is pressed, message how to use the control keys is printed into Serial1 again
    sendManualToPc();
  }
  if (xBees.inputReceivedManual()) {
    setAndNormalizeMotorValues();
  }

  if (!isAllowDrive()) {
    count++;
    if (count > 5) {
      buzzerObj.play(300, 40);
      count = 0;
    }
    ledRed(1); delay(50); ledRed(0); delay(100);
  }
  if (isStandingStill()) {
    ledGreen(0);
  } else {
    ledGreen(1);
  }
  encoder.correctOffset();
  applyMotorValues();
  encoder.printCorrectionValues();
}

void blockMode() {
  bp.duwBlock();
  xBees.inputReceivedAutonomous();
}

void autonomousMode() {
  lineFollowerObj.lineFollow();
  if (lineFollowerObj.getCalibratedCount() != 5) {
    resetSpeed();
  }
  applyMotorValues();
  xBees.inputReceivedAutonomous();
}
void loop() {
  if (buttonA.getSingleDebouncedPress()) {
    drivingMode = (drivingMode + 1) % 3;
    if (drivingMode == 1) {
      buzzerObj.autonomousModeSound();
    } else if (drivingMode == 2) {
      buzzerObj.autonomousModeSound();
    } else {
      buzzerObj.manualModeSound();
    }
    resetSpeed();
  }
  if (drivingMode == 1) {
    autonomousMode();
  } else if (drivingMode == 2) {
    blockMode();
  } else {
    manualMode();
  }
  if (playSoundId > 0) {
    //play a different sound for each id
    playSoundId = buzzerObj.playSoundById(playSoundId);
  }
}