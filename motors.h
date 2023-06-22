#pragma once
extern double speedLeft;
extern double speedRight;
extern int drivingMode;

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