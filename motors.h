#include <Zumo32U4.h>
#include "motors.h"
#pragma once

class motors {

    public:
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
}