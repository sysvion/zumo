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
  motorsManual motorsManual;
};
