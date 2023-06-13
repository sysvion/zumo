#pragma once
extern double speedLeft;
extern double speedRight;
extern bool   isAllowedToDrive;

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