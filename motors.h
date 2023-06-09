#pragma once

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
bool isStandingStill();
bool isAllowDrive();

double getSpeedLeft();
double getSpeedRight();