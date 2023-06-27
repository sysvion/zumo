#include "Arduino.h"
#include <Wire.h>
#include <Zumo32U4.h>
#include "lineFollower.h"
#include "motors.h"

/// This is the maximum speedLineFollower the motors will be allowed to turn.
const uint16_t MAX_SPEED = 300;
uint16_t speedLineFollower = 240;
int count_;
int calibratedCount;
int lastSensorDetectedLine = 0;
int lastSensorDetectedLineTimer;
int colors = 0;
const int COLOR_MARGIN = 25;

bool almostOffLine = false;

/// 0 is none 1 is left 2 is right
int scheduleTurn = 0;  
int doScheduledTurn = 0;
int doScheduledTurnTimerLastTime = 0;

Zumo32U4LineSensors lineSensors;
lineFollower lineFollower;
Zumo32U4ButtonB buttonB;

int16_t lastError = 0;

#define NUM_SENSORS 5

/// creates an array with length 5
unsigned int lineSensorValues[NUM_SENSORS];  
unsigned int lineSensorGreen[NUM_SENSORS] = { 5000, 5000, 5000, 5000, 5000 };
unsigned int lineSensorGray[NUM_SENSORS] = { 5000, 5000, 5000, 5000, 5000 };
unsigned int lineSensorBrown[NUM_SENSORS] = { 5000, 5000, 5000, 5000, 5000 };

/// init
lineFollower::lineFollower() {

}

void lineFollower::lineSensorsInitFiveSensors() {
  lineSensors.initFiveSensors();
}

int lineFollower::CalibrateSensors() {
  lineSensors.calibrate();
  buzzer.scanColorSound();
  calibratedCount++;
}

/// store the measurement color in the array lineSensorGreen when buttonC is pressed
void lineFollower::CalibrateGreen() {
  if (buttonC.getSingleDebouncedPress()) {
    buzzer.scanColorSound();
    calibratedCount = 3;
    Serial.println(colors);
    for (int i = 0; i < NUM_SENSORS; i++)
      lineSensorGreen[i] = lineSensorValues[i];
  }
}

/// store the measurement color in the array lineSensorGreen when buttonC is pressed
void lineFollower::CalibrateGray() {
  if (buttonC.getSingleDebouncedPress()) {
    buzzer.scanColorSound();
    calibratedCount = 4;
    for (int i = 0; i < NUM_SENSORS; i++)
      lineSensorGray[i] = lineSensorValues[i];
  }
}

/// store the measurement color in the array lineSensorBrown when buttonC is pressed
void lineFollower::CalibrateBrown() {
  if (buttonC.getSingleDebouncedPress()) {
    buzzer.scanColorSound();
    calibratedCount = 5;
    for (int i = 0; i < NUM_SENSORS; i++)
      lineSensorBrown[i] = lineSensorValues[i];
    delay(2000);
  }
}

/// getCalibratedCount
int lineFollower::getCalibratedCount() {
  return calibratedCount;
}

////////////////////////COLOR CHECK FUNCTIONS

/// check if it is sensor is reading a color near the stored color representing green
bool lineFollower::isGreen(int i) {
  return lineSensorValues[i] > lineSensorGreen[i] - COLOR_MARGIN && lineSensorValues[i] < lineSensorGreen[i] + COLOR_MARGIN;
}

/// check if it is sensor is reading a color near the stored color representing gray
bool lineFollower::isGray(int i) {
  return lineSensorValues[i] > lineSensorGray[i] - (COLOR_MARGIN + 10) && lineSensorValues[i] < lineSensorGray[i] + (COLOR_MARGIN + 70);
}

/// check if it is sensor is reading a color near the stored color representing brown 
bool lineFollower::isBrown(int i) {
  return lineSensorValues[i] > lineSensorBrown[i] - COLOR_MARGIN + 10 && lineSensorValues[i] < lineSensorBrown[i] + COLOR_MARGIN + 10;
}

/// check if it is standing on a black line
bool lineFollower::isBlack(int i) {
  return lineSensorValues[i] > 700;
}

/// check if it is standing on a white line 
bool lineFollower::isWhite(int i) {
  return lineSensorValues[i] < 50;
}

/// check if it is standing on a line 
bool lineFollower::isLine(int i) {
  return isGreen(i) || isBlack(i);
}

/// This function operates the same as readCalibrated(), but also returns an
/// estimated position of the robot with respect to a line. The
/// estimate is made using a weighted average of the sensor indices
/// multiplied by 1000, so that a return value of 0 indicates that
/// the line is directly below sensor 0, a return value of 1000
/// indicates that the line is directly below sensor 1, 2000
/// indicates that it's below sensor 2000, etc.  Intermediate
/// values indicate that the line is between two sensors.  The
/// formula is (where \f$v_0\f$ represents the value from the
/// first sensor):
///
/// \f[
/// {(0 \times v_0) + (1000 \times v_1) + (2000 \times v_2) + \cdots
/// \over
/// v_0 + v_1 + v_2 + \cdots}
/// \f]
int lineFollower::readLine(unsigned int *sensor_values) {

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

// this is the main loop where all the logic for the 
void lineFollower::lineFollow() {

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
    drivingMode = 1;
    speedLeft = 0;
    speedRight = 0;
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

///////////////////////////this part of the code handles the gray tape to take turns at intersections but was not stable enough.

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

  if (isGreen(2)) {
    speedLeft /= 1.7;
    speedRight /= 1.7;
  }

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

///////////////////////////print sensor values, used for debugging only
  // char buffer[80];
  // sprintf(buffer, "%5d %5d %5d %5d %5d error: %5d %5d %5d %5d gray %5d %5d %5d %5d %5d\n", lineSensorValues[0], lineSensorValues[1], lineSensorValues[2], lineSensorValues[3], lineSensorValues[4], error, speedDifference, (int)speedLeft, (int)speedRight, lineSensorGreen[0], lineSensorGreen[1], lineSensorGreen[2], lineSensorGreen[3], lineSensorGreen[4]);
  // if (count_ == 40) {
  //   Serial.print(buffer);
  //   count_ = 0;
  // }
  // count_++;
}
