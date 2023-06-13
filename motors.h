#pragma once
extern double speedLeft;
extern double speedRight;

void moveLeft(const int &steerIntensity = 1.3);
void moveRight(const int &steerIntensity = 1.3);
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