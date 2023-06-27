#include <Zumo32U4.h>
#include "buzzerStuff.h"
#include "motorsManual.h"
#pragma once

class lineFollower {

public:
  lineFollower();
  void lineSensorsInitFiveSensors();
  int CalibrateSensors();
  int getCalibratedCount();
  void CalibrateGreen();
  void CalibrateGray();
  void CalibrateBrown();
  bool isGreen(int);
  bool isGray(int);
  bool isBrown(int);
  bool isBlack(int);
  bool isWhite(int);
  bool isLine(int);
  int readLine(unsigned int *);
  void lineFollow();
  void storeCollor(int);
  int *getStoredColors(int index);
  void readCalibrated(int *sensor_values);
private:
  buzzerStuff buzzer;
  Zumo32U4ButtonC buttonC;
  motorsManual m;

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

  int16_t lastError = 0;

};
