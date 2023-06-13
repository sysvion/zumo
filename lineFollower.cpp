/* This example uses the line sensors on the Zumo 32U4 to follow
a black line on a white background, using a PID-based algorithm.
It works decently on courses with smooth, 6" radius curves and
has been tested with Zumos using 75:1 HP motors.*/

#include <Wire.h>
#include <Zumo32U4.h>
#include "lineFollower.h"
#include "extern.h"
#include "buzzerStuff.h"
#include "motors.h"

// This is the maximum speedLineFollower the motors will be allowed to turn.
const uint16_t MAX_SPEED = 300;
uint16_t speedLineFollower = 150;
int count_;
int calibratedCount;
int lastSensorDetectedLine = 0;
int colors = 0;
const int COLOR_MARGIN = 25;

Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;

int16_t lastError = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];  // creates an array with length 5
unsigned int lineSensorGreen[NUM_SENSORS] = {5000,5000,5000,5000,5000};
unsigned int lineSensorGray[NUM_SENSORS] = {5000,5000,5000,5000,5000};
unsigned int lineSensorBrown[NUM_SENSORS] = {5000,5000,5000,5000,5000};

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
    for (int i = 0; i<NUM_SENSORS; i++)
      lineSensorGreen[i] = lineSensorValues[i];
  }
}
void CalibrateGray() {
  if (buttonC.getSingleDebouncedPress()) {
    scanColorSound();
    calibratedCount = 4;
    for (int i = 0; i<NUM_SENSORS; i++)
      lineSensorGray[i] = lineSensorValues[i];
  }
}
void CalibrateBrown() {
  if (buttonC.getSingleDebouncedPress()) {
    scanColorSound();
    calibratedCount = 5;
    for (int i = 0; i<NUM_SENSORS; i++)
      lineSensorBrown[i] = lineSensorValues[i];
  }
}

int getCalibratedCount() {
  return calibratedCount;
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
    if (value > 10) { //200 default
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

////////////////////////COLOR CHECK FUNCTIONS

bool lineSensorIsGreen(int i) {
  return lineSensorValues[i] > lineSensorGreen[i] - COLOR_MARGIN && lineSensorValues[i] < lineSensorGreen[i] + COLOR_MARGIN;
}

bool lineSensorIsGray(int i) {
  return lineSensorValues[i] > lineSensorGray[i] - COLOR_MARGIN && lineSensorValues[i] < lineSensorGray[i] + COLOR_MARGIN;
}

bool lineSensorIsBrown(int i) {
  return lineSensorValues[i] > lineSensorBrown[i] - COLOR_MARGIN && lineSensorValues[i] < lineSensorBrown[i] + COLOR_MARGIN + 15;
}

bool lineSensorIsBlack(int i) {
  return lineSensorValues[i] > 800;
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
  int16_t speedDifference = error / 4 + 3 * (error - lastError);
  if (count_ == 40) {
    Serial.println(speedDifference);
  }

  lastError = error;

  // Get individual motor speeds. The sign of speedDifference
  // determines if the robot turns left or right.
  speedLeft = speedLineFollower + speedDifference;
  speedRight = speedLineFollower - speedDifference;

  // Constrain motor speeds to be between 0 and MAX_SPEED.
  // One motor will always be turning at MAX_SPEED, and the other
  // will be at MAX_SPEED-|speedDifference| if that is positive,
  // else it will be stationary.
  speedLeft = constrain((int)speedLeft, 0, MAX_SPEED);
  speedRight = constrain((int)speedRight, 0, MAX_SPEED);


  if (lineSensorIsBlack(0)) {
    speedLeft = -100;
    speedRight = 200;
    lastSensorDetectedLine = 0;
  }
  if (lineSensorValues[4] > 800) {
    speedLeft = 200;
    speedRight = -100;
    lastSensorDetectedLine = 4;
  }
  if (lineSensorValues[1] < 40 && lastSensorDetectedLine == 4 && lineSensorValues[4] < 40) {
    speedLeft += 200;
    speedRight -= 400;
  }
  if (lineSensorValues[3] < 40 && lastSensorDetectedLine == 0 && lineSensorValues[0] < 40) {
    speedLeft -= 400;
    speedRight += 200;
  }

  if (lineSensorValues[1] < 40 && lineSensorValues[3] < 40 && lineSensorValues[0] < 40 && lineSensorValues[4] < 40) {
    speedLeft = 150;
    speedRight = 150;
  }

  if (lineSensorIsGreen(2)) { // If the color is green
    scanColorSound();
  }

  if (lineSensorIsGray(0) || lineSensorIsGray(4)) { // If the color is gray.
    play(500, 50);
  }

   if (lineSensorIsBrown(0) || lineSensorIsBrown(4)) { // If the color is brown.
      // Ga naar modus: blok duwen.
      speedLeft = 0;
      speedRight = 0;
      play(600, 50);
  }

  char buffer[80];
  sprintf(buffer, "%5d %5d %5d %5d %5d error: %5d %5d %5d %5d green %5d %5d %5d %5d %5d\n", lineSensorValues[0], lineSensorValues[1], lineSensorValues[2], lineSensorValues[3], lineSensorValues[4], error, speedDifference, (int)speedLeft, (int)speedRight, lineSensorGreen[0], lineSensorGreen[1], lineSensorGreen[2], lineSensorGreen[3], lineSensorGreen[4]);
  if (count_ == 40) {
    Serial.print(buffer);
    count_ = 0;
  }
  count_++;
}
