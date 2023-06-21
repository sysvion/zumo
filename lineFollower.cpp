#include "Arduino.h"
/* This example uses the line sensors on the Zumo 32U4 to follow
a black line on a white background, using a PID-based algorithm.
It works decently on courses with smooth, 6" radius curves and
has been tested with Zumos using 75:1 HP motors.*/

#include <Wire.h>
#include <Zumo32U4.h>
#include "lineFollower.h"
#include "buzzerStuff.h"
#include "motors.h"

// This is the maximum speedLineFollower the motors will be allowed to turn.
const uint16_t MAX_SPEED = 300;    //300
uint16_t speedLineFollower = 240;  //240
int count_;
int calibratedCount;
int lastSensorDetectedLine = 0;
int lastSensorDetectedLineTimer;
int colors = 0;
const int COLOR_MARGIN = 25;

bool almostOffLine = false;

int scheduleTurn = 0;  //0 is none 1 is left 2 is right
int doScheduledTurn = 0;
int doScheduledTurnTimerLastTime = 0;

Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;

int16_t lastError = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];  // creates an array with length 5
unsigned int lineSensorGreen[NUM_SENSORS] = { 5000, 5000, 5000, 5000, 5000 };
unsigned int lineSensorGray[NUM_SENSORS] = { 5000, 5000, 5000, 5000, 5000 };
unsigned int lineSensorBrown[NUM_SENSORS] = { 5000, 5000, 5000, 5000, 5000 };

void lineSensorsInitFiveSensors() {
  lineSensors.initFiveSensors();
}

int CalibrateSensors() {
  lineSensors.calibrate();
  scanColorSound();
  calibratedCount++;
}

void CalibrateGreen() {
  if (buttonC.getSingleDebouncedPress()) {
    scanColorSound();
    calibratedCount = 3;
    Serial.println(colors);
    for (int i = 0; i < NUM_SENSORS; i++)
      lineSensorGreen[i] = lineSensorValues[i];
  }
}
void CalibrateGray() {
  if (buttonC.getSingleDebouncedPress()) {
    scanColorSound();
    calibratedCount = 4;
    for (int i = 0; i < NUM_SENSORS; i++)
      lineSensorGray[i] = lineSensorValues[i];
  }
}
void CalibrateBrown() {
  if (buttonC.getSingleDebouncedPress()) {
    scanColorSound();
    calibratedCount = 5;
    for (int i = 0; i < NUM_SENSORS; i++)
      lineSensorBrown[i] = lineSensorValues[i];
    delay(2000);
  }
}

int getCalibratedCount() {
  return calibratedCount;
}

////////////////////////COLOR CHECK FUNCTIONS

bool isGreen(int i) {
  return lineSensorValues[i] > lineSensorGreen[i] - COLOR_MARGIN && lineSensorValues[i] < lineSensorGreen[i] + COLOR_MARGIN;
}

bool isGray(int i) {
  return lineSensorValues[i] > lineSensorGray[i] - (COLOR_MARGIN + 10) && lineSensorValues[i] < lineSensorGray[i] + (COLOR_MARGIN + 70);
}

bool isBrown(int i) {
  return lineSensorValues[i] > lineSensorBrown[i] - COLOR_MARGIN + 10 && lineSensorValues[i] < lineSensorBrown[i] + COLOR_MARGIN + 10;
}

bool isBlack(int i) {
  return lineSensorValues[i] > 700;
}

bool isWhite(int i) {
  return lineSensorValues[i] < 50;
}

bool isLine(int i) {
  return isGreen(i) || isBlack(i);
}

int readLine(unsigned int *sensor_values) {

  unsigned char readMode = 1;
  unsigned char white_line = 0;
  int lastValue;
  unsigned char i, on_line = 0;
  unsigned long avg;  // this is for the weighted total, which is long before division
  unsigned int sum;   // this is for the denominator which is <= 64000

  lineSensors.readCalibrated(sensor_values, readMode);

  avg = 0;
  sum = 0;

  for (i = 0; i < NUM_SENSORS; i++) {
    int value = sensor_values[i];
    if (white_line)
      value = 1000 - value;

    // keep track of whether we see the line at all
    if (value > 60) {  //200 default
      on_line = 1;
    }

    // only average in values that are above a noise threshold
    if (value > 50) {
      avg += (long)(value) * (i * 1000);
      sum += value;
    }
  }

  if (!on_line) {
    // If it last read to the left of center, return 0.
    if (lastValue < (NUM_SENSORS - 1) * 1000 / 2)
      return 0;
    // If it last read to the right of center, return the max.
    else
      return (NUM_SENSORS - 1) * 1000;
  }

  lastValue = avg / sum;

  return lastValue;
}

////////////////////////

void lineFollow() {

  int16_t position = readLine(lineSensorValues);

  if (buttonB.getSingleDebouncedPress() && calibratedCount < 2) {
    CalibrateSensors();
  }

  if (calibratedCount == 2) {
    CalibrateGreen();
  } else if (calibratedCount == 3) {
    CalibrateGray();
  } else if (calibratedCount == 4) {
    CalibrateBrown();
  }

  // "error" is how far we are away from the center of the line, which corresponds to position 2000.
  int16_t error = position - 2000;

  // Get motor speedLineFollower difference using proportional and derivative PID terms (the integral term is generally not very
  // useful for line following).  Here we are using a proportional constant of 1/4 and a derivative constant of 6.
  int speedDifference = error / 6 + (0 * (error - lastError));

  lastError = error;

  // Get individual motor speeds. The sign of speedDifference
  // determines if the robot turns left or right.
  speedLeft = speedLineFollower + speedDifference;
  speedRight = speedLineFollower - speedDifference;

  speedLeft = (int)speedLeft;
  speedRight = (int)speedRight;

  speedLeft = constrain(speedLeft, 0, MAX_SPEED);
  speedRight = constrain(speedRight, 0, MAX_SPEED);


  if (isLine(0)) {
    speedLeft = -300;
    speedRight = 50;
    lastSensorDetectedLine = 0;
    almostOffLine = true;
  }
  if (isLine(4)) {
    speedLeft = 50;
    speedRight = -300;
    lastSensorDetectedLine = 4;
    almostOffLine = true;
  }
  if (isWhite(1) && isWhite(4) && lastSensorDetectedLine == 4) {
    speedLeft += 150;
    speedRight = -100;
  }
  if (isWhite(0) && isWhite(3) && lastSensorDetectedLine == 0) {
    speedLeft = -100;
    speedRight += 150;
  }

  if (isWhite(0) && isWhite(1) && isWhite(3) && isWhite(4)) {
    speedLeft = 200;
    speedRight = 200;
  }

  if (isBrown(0) && isBrown(1) && isBrown(3) && isBrown(4)) {
    // Ga naar modus: blok duwen.
    ////////////////////////////////////////////////////////////drivingMode = 2;
    // speedLeft = 0;
    // speedRight = 0;
    play(600, 50);
  }

  if (almostOffLine && lastSensorDetectedLine == 0) {
    speedLeft = -200;
    speedRight = 300;
  }

  if (almostOffLine && lastSensorDetectedLine == 4) {
    speedLeft = 300;
    speedRight = -200;
  }

  if (almostOffLine && isLine(2)) {
    almostOffLine = false;
  }

  /////////////////////////////


  // if (isGray(0) && !isGray(4) && (isBlack(1) || isBlack(2) || isBlack(3))) {
  //   scheduleTurn = 1; //left
  //   startupSound();
  //   speedLeft = 100;
  //   speedRight = 100;
  // }
  // if (isGray(4) && !isGray(0) && (isBlack(1) || isBlack(2) || isBlack(3))) {
  //   scheduleTurn = 2; //right
  //   startupSound();
  //   speedLeft = 100;
  //   speedRight = 100;
  // }

  // if (scheduleTurn == 1 && isBlack(0) && isBlack(4)) {
  //   doScheduledTurn = 1;
  //   scheduleTurn = 0;
  //   doScheduledTurnTimerLastTime = millis();
  //   //manualModeSound();
  // }
  // if (scheduleTurn == 2 && isBlack(0) && isBlack(4)) {
  //   doScheduledTurn = 2;
  //   scheduleTurn = 0;
  //   doScheduledTurnTimerLastTime = millis();
  //   //manualModeSound();
  // }

  // if (isGreen(2)) {
  //   speedLeft /= 2 + 70;
  //   speedRight /= 2 + 70;
  // }

  // else if (scheduleTurn) {
  //   speedLeft /= 2;
  //   speedRight /= 2;
  //   scanColorSound();
  // }


  // if (doScheduledTurn == 1 && (uint8_t)(millis() - doScheduledTurnTimerLastTime) < 500) {
  //   speedLeft = -400;
  //   speedRight = 200;
  //   manualModeSound();
  // } else {
  //   doScheduledTurn = 0;
  // }

  // if (doScheduledTurn == 2 && (uint8_t)(millis() - doScheduledTurnTimerLastTime) < 500) {
  //   speedLeft = 200;
  //   speedRight = -400;
  //   manualModeSound();
  // } else {
  //   doScheduledTurn = 0;
  // }




  // char buffer[80];
  // sprintf(buffer, "%5d %5d %5d %5d %5d error: %5d %5d %5d %5d gray %5d %5d %5d %5d %5d\n", lineSensorValues[0], lineSensorValues[1], lineSensorValues[2], lineSensorValues[3], lineSensorValues[4], error, speedDifference, (int)speedLeft, (int)speedRight, lineSensorGreen[0], lineSensorGreen[1], lineSensorGreen[2], lineSensorGreen[3], lineSensorGreen[4]);
  // if (count_ == 40) {
  //   Serial.print(buffer);
  //   count_ = 0;
  // }
  // count_++;
}