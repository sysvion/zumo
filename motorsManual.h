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
  ///starting speed (no movement)
double speed = 1;
double speedRight;
double speedLeft;
const int MIN_SPEED = -400;
const int MAX_SPEED = 400;

/// determines if the robot should move or pause
bool allowDrive = true;

/// the minimum is 1 because we are going to multiplied
const int minimumTuriningValue = 1;
const int maximumTurningValue = 6;

//default steer value. Value is later on multiplied with so the default is 1
double steerRight = minimumTuriningValue;

//default steer value. Value is later on multiplied with so the default is 1
double steerLeft = minimumTuriningValue;

//intensity of steering changes
const double steerIntensity = 1.3;
};