#include "motors.h"
#include "encoderStuff.h"
#include "extern.h"
#include <Zumo32U4.h>

Zumo32U4Motors motors;

double speed = 1;                                 //starting speed (no movement)
double speedRight;
double speedLeft;
const int MIN_SPEED = -400;
const int MAX_SPEED = 400;

bool allowDrive = true;                           //determines if the robot should move or pause


// the minimum is 1 because we are going to multiplied 
const int minimumTuriningValue = 1;
const int maximumTurningValue = 6;
double steerRight = minimumTuriningValue;         //default steer value. Value is later on multiplied with so the default is 1
double steerLeft = minimumTuriningValue;
const double steerIntensity = 1.3;                      //intensity of steering changes


// void printCorrectionValues()
// {
//   snprintf_P(correctionValues, sizeof(correctionValues), PSTR("%6d %6d exp L: %4d exp R: %4d"), 
//           countsLeft, countsRight, 
//           expectedLeftEncoderCount, expectedRightEncoderCount);
//   Serial.println((String)correctionValues + "\toffL:" + offsetLeftEncoderCount + "\toffR:" + offsetRightEncoderCount 
//   + "\tcorrectLeft:" + correctLeft + "\tcorrectRight:" + correctRight
//       );
// }


void moveLeft()
{
  resetEncoderCounts();
  if (!(steerRight < maximumTurningValue))
  {
    return;
  }
  if  (steerLeft < minimumTuriningValue)
  {
    steerRight *= steerIntensity;   //increase right wheel speed
  }
  else
  {
    steerLeft /= steerIntensity;    //reduce left wheel speed if condition not met
  }
}
void moveRight()
{
  resetEncoderCounts();
  if (steerLeft < maximumTurningValue)
  {
    if (steerRight < minimumTuriningValue)
    {
        steerLeft *= steerIntensity;  //increase left wheel speed
    }
    else
    {
        steerRight /= steerIntensity; //decrease right wheel speed if condition not met
    }
  }
}
void moveSlower()
{
  resetEncoderCounts();
  if (speed > -10)                  //checks if speed is above mimimum allowed value (NOTE: speed within functions isn't the actual motor speed.
  {                                 //The actual speeds for the motors are calculated at the end using a formula)
    speed -= 1;
    Serial1.println((String)"Speed: " + speed + " --");

  }
}
void moveFaster()                   //does the opposite as moveSlower(), to move faster
{
  resetEncoderCounts();
  if (speed < 10)                   //checks if speed is below maximum allowed value
  {
    speed += 1;
    Serial1.println((String)"Speed: " + speed + " ++");
  }
}
void moveToMaxSpeed()
{
  resetEncoderCounts();
  speed = 8;                        //set speed to the max
  Serial1.println((String)"Speed: " + speed + " ++");

}
void resetSpeed()
{
  resetEncoderCounts();
  speed = 1;
  //This stops the robot because opposite steer is subtracted from speed in the formula that determines the final speed for both motors (1-1=0)
  steerLeft = 1;
  steerRight = 1;
  speedLeft = 0;
  speedRight = 0;

  //sets drive t true so robot can immediately start to drive again when input it given in case the robot was paused before reset was pressed
  allowDrive = true;                     

}
void resetRotationalMovement()
{ 
  resetEncoderCounts();
  //resets steer values but not the overal speed so the robots starts driving straight forward
  steerLeft = 1;
  steerRight = 1;
}

void stopContinue()
{
  resetEncoderCounts();
  allowDrive = !allowDrive;
}
void rotateDeg(int deg)
{
  resetEncoderCounts();
  deg *= 10.6;
  setExpectedLeftEncoderCount(-deg);
  setExpectedRightEncoderCount(deg);
}

void setAndNormalizeMotorValues() {
    speedLeft  = (speed * steerLeft - steerRight) * 50;
    speedRight = (speed * steerRight - steerLeft) * 50;

    if (speedLeft  > MAX_SPEED) speedLeft = MAX_SPEED;
    if (speedRight > MAX_SPEED) speedRight = MAX_SPEED;
    if (speedLeft  < MIN_SPEED) speedLeft = MIN_SPEED;
    if (speedRight < MIN_SPEED) speedRight = MIN_SPEED;

    //prints the left/right motor speed and the two steer values
    Serial1.println((String)"\nLEFT: " + speedLeft + "  steerLeft: " + steerLeft + "\nRIGHT: " + speedRight + "  steerRight: " + steerRight);
    //these two formulas determine the final speeds for the left and right motor
    
    if (!allowDrive)
    {
        speedLeft = 0;      //sets the motor speeds to 0 to stop them. When robot is allowed to drive again, it will continue at the last set speed instead of resetting its speed.
        speedRight = 0;
        Serial1.println("STOPPED");
    }
}

void applyMotorValues() 
{
    motors.setLeftSpeed(speedLeft + getCorrectLeft());///////////////////////////////////////////////////////////
    motors.setRightSpeed(speedRight + getCorrectRight());
}

bool isStandingStill() {
  return (speedLeft == 0 && speedRight == 0);
}

bool isAllowDrive() {
  return (allowDrive);
}

double getSpeedLeft() {
  return speedLeft;
}

double getSpeedRight() {
  return speedRight;
}
