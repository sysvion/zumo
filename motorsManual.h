#include <Zumo32U4.h>
#pragma once

class motorsManual {
public:
  motorsManual();
  void moveLeft();
  void moveRight();
  void moveSlower();
  void moveFaster();

  void moveToMaxSpeed();
  void resetSpeed();
  void resetRotationalMovement();
  void rotateDeg(int);
  void stopContinue();
  void setAndNormalizeMotorValues();
  void applyMotorValues();
  const bool isStandingStill();
  const bool isAllowDrive();
  const double getSpeedLeft();
  const double getSpeedRight();
  void setSpeedLeft(double);
  void setSpeedRight(double);
  
  private:
  double speed;
  double speedRight;
  double speedLeft;
  const int MIN_SPEED = -400;
  const int MAX_SPEED = 400;
  bool allowDrive;
  const int minimumTuriningValue = 1;
  const int maximumTurningValue = 6;
  double steerRight;
  double steerLeft;
  const double steerIntensity = 1.3;

};