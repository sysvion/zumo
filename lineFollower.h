#include <Zumo32U4.h>
#pragma once

class lineFollower {

  public:
    void lineSensorsInitFiveSensors();
    void CalibrateGreen();
    void CalibrateGray();
    void CalibrateBrown();
    int CalibrateSensors();
    int getCalibratedCount();
    int readLine(unsigned int *);
    void lineFollow();
    void storeCollor(int);
    int *getStoredColors(int index);
    void readCalibrated(int *sensor_values);
}
