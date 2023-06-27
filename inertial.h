#include <Zumo32U4.h>
#pragma once

class inertial {
private:
  Zumo32U4IMU sensors;
  int gyroOffset[2];


public:
  inertial();
  inertial(const inertial &);
  void setup();
  void calabrateGyro();
  void getGyroPoss(int *);
  bool checkDownwardSpeed();
  void getMegData(int *);
  void getaccData(int *);


  int8_t DebugRegTest(String, uint8_t, uint8_t, int16_t) const;

  void print();

  int check();
};
