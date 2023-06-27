#include "motors.h"
#include "encoderStuff.h"
#include <Zumo32U4.h>

Zumo32U4Motors motors;
encoderStuff encoderObj;

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


/// change the turning left speed by the steerIntensity 
void motors::moveLeft() {
  encoderObj.resetEncoderCounts();
  if (!(steerRight < maximumTurningValue)) {
    return;
  }
  if (steerLeft < minimumTuriningValue) {
    steerRight *= steerIntensity;  //increase right wheel speed
  } else {
    steerLeft /= steerIntensity;  //reduce left wheel speed if condition not met
  }
}

/// change the turning right speed by the steerIntensity 
void motors::moveRight() {
  encoderObj.resetEncoderCounts();
  if (steerLeft < maximumTurningValue) {
    if (steerRight < minimumTuriningValue) {
      steerLeft *= steerIntensity;  //increase left wheel speed
    } else {
      steerRight /= steerIntensity;  //decrease right wheel speed if condition not met
    }
  }
}

// move slower by 10 %
void motors::moveSlower() {
  encoderObj.resetEncoderCounts();
  if (speed > -10)  //checks if speed is above mimimum allowed value (NOTE: speed within functions isn't the actual motor speed.
  {                 //The actual speeds for the motors are calculated at the end using a formula)
    speed -= 1;
    Serial1.println((String) "Speed: " + speed + " --");
  }
}

/// does the opposite as moveSlower(), to move faster
void motors::moveFaster()  
{
  encoderObj.resetEncoderCounts();
  if (speed < 10)  //checks if speed is below maximum allowed value
  {
    speed += 1;
    Serial1.println((String) "Speed: " + speed + " ++");
  }
}

/// set speed to 8
void motors::moveToMaxSpeed() {
  encoderObj.resetEncoderCounts();
  speed = 8;  //set speed to the max
  Serial1.println((String) "Speed: " + speed + " ++");
}

/// set speed to 1 (standing still)
void motors::resetSpeed() {
  encoderObj.resetEncoderCounts();
  speed = 1;
  //This stops the robot because opposite steer is subtracted from speed in the formula that determines the final speed for both motors (1-1=0)
  steerLeft = 1;
  steerRight = 1;
  speedLeft = 0;
  speedRight = 0;

  //sets drive t true so robot can immediately start to drive again when input it given in case the robot was paused before reset was pressed
  allowDrive = true;
}

// reset rotational movement
void motors::resetRotationalMovement() {
  encoderObj.resetEncoderCounts();
  //resets steer values but not the overal speed so the robots starts driving straight forward
  steerLeft = 1;
  steerRight = 1;
}

/// switches the allowDrive flag
void motors::stopContinue() {
  encoderObj.resetEncoderCounts();
  allowDrive = !allowDrive;
}

/// rotate param degrees
void motors::rotateDeg(int deg) {
  encoderObj.resetEncoderCounts();
  deg *= 10.6;
  encoderObj.setExpectedLeftEncoderCount(-deg);
  encoderObj.setExpectedRightEncoderCount(deg);
}

 
void motors::setAndNormalizeMotorValues() {
  speedLeft = (speed * steerLeft - steerRight) * 50;
  speedRight = (speed * steerRight - steerLeft) * 50;

  //prints the left/right motor speed and the two steer values
  Serial1.println((String) "\nLEFT: " + speedLeft + "  steerLeft: " + steerLeft + "\nRIGHT: " + speedRight + "  steerRight: " + steerRight);
  //these two formulas determine the final speeds for the left and right motor

  if (!allowDrive) {
    speedLeft = 0;  //sets the motor speeds to 0 to stop them. When robot is allowed to drive again, it will continue at the last set speed instead of resetting its speed.
    speedRight = 0;
    Serial1.println("STOPPED");
  }
}

/// set the pins of the motor so it changes the speed
void motors::applyMotorValues() {
  motors.setLeftSpeed(speedLeft + encoderObj.getCorrectLeft());
  motors.setRightSpeed(speedRight + encoderObj.getCorrectRight());
}

// if the motor speed is 0
const bool motors::isStandingStill() {
  return (speedLeft == 0 && speedRight == 0);
}

// is the allowDrive flag is set
const bool motors::isAllowDrive() {
  return allowDrive;
}

// get motorSpeedLeft
const double motors::getSpeedLeft() {
  return speedLeft;
}

// get motorSpeedRight
const double motors::getSpeedRight() {
  return speedRight;
}
