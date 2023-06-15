#include "motors.h"
#include "encoderStuff.h"
#include <Zumo32U4.h>

Zumo32U4Motors motors;
//starting speed (no movement)

const double USER_STANDSTILL = 1;                                 
const double MIN_USER_SPEED = -10; //?
const double MAX_USER_SPEED = 10;
double userSpeed = USER_STANDSTILL;

double speedRight;
double speedLeft;
const int MIN_SPEED = -400;
const int MAX_SPEED = 400;

bool isAllowedToDrive = true;                           //determines if the robot should move or pause


// the minimum is 1 because we are going to multiplied 
const int minimumTuriningValue = 1;
const int maximumTurningValue = 6;
double steerRight = minimumTuriningValue;         //default steer value. Value is later on multiplied with so the default is 1
double steerLeft = minimumTuriningValue;


// void printCorrectionValues()
// {
//   snprintf_P(correctionValues, sizeof(correctionValues), PSTR("%6d %6d exp L: %4d exp R: %4d"), 
//           countsLeft, countsRight, 
//           expectedLeftEncoderCount, expectedRightEncoderCount);
//   Serial.println((String)correctionValues + "\toffL:" + offsetLeftEncoderCount + "\toffR:" + offsetRightEncoderCount 
//   + "\tcorrectLeft:" + correctLeft + "\tcorrectRight:" + correctRight
//       );
// }


void moveLeft(const int &steerIntensity)
{
  resetEncoderCounts();
  if (!(steerRight < maximumTurningValue)) {return; }

  if  (steerLeft < minimumTuriningValue)
  {
    steerRight *= steerIntensity;   //increase right wheel speed
  }
  else
  {
    steerLeft /= steerIntensity;    //reduce left wheel speed if condition not met
  }
}

void moveRight(const int &steerIntensity)
{
  resetEncoderCounts();
  if (steerLeft < maximumTurningValue) { return; }

  if (steerRight < minimumTuriningValue)
  {
      steerLeft *= steerIntensity;  //increase left wheel speed
  }
  else
  {
      steerRight /= steerIntensity; //decrease right wheel speed if condition not met
  }
  
}

void moveSlower()
{
  resetEncoderCounts();
  if (!(userSpeed > MIN_USER_SPEED)) {return;}
  
  userSpeed -= 1;
  Serial1.println((String)"Speed: " + userSpeed + " --");
}

//does the opposite as moveSlower(), to move faster
void moveFaster()
{
  resetEncoderCounts();
  if (userSpeed < MAX_USER_SPEED)
  {
    userSpeed += 1;
    Serial1.println((String)"Speed: " + userSpeed + " ++");
  }
}

void moveToMaxSpeed()
{
  resetEncoderCounts();
  userSpeed = MAX_USER_SPEED;
  Serial1.println((String)"Speed: " + userSpeed + " ++");

}

void resetSpeed()
{
  resetEncoderCounts();
  userSpeed = 1;
  //This stops the robot because opposite steer is subtracted from speed in the formula that determines the final speed for both motors (1-1=0)
  steerLeft = 1;
  steerRight = 1;
  speedLeft = 0;
  speedRight = 0;

  //sets drive t true so robot can immediately start to drive again when input it given in case the robot was paused before reset was pressed
  isAllowedToDrive = true;                     

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
  isAllowedToDrive = !isAllowedToDrive;
}
void rotateDeg(int deg)
{
  resetEncoderCounts();
  deg *= 10.6;
  setExpectedLeftEncoderCount(-deg);
  setExpectedRightEncoderCount(deg);
}

void setAndNormalizeMotorValues() {
    speedLeft  = (userSpeed * steerLeft - steerRight) * 50;
    speedRight = (userSpeed * steerRight - steerLeft) * 50;

    // note: is this allready done in the lib?
    if (speedLeft  > MAX_SPEED) speedLeft = MAX_SPEED;
    if (speedRight > MAX_SPEED) speedRight = MAX_SPEED;
    if (speedLeft  < MIN_SPEED) speedLeft = MIN_SPEED;
    if (speedRight < MIN_SPEED) speedRight = MIN_SPEED;

    //prints the left/right motor speed and the two steer values
    Serial1.println((String)"\nLEFT: " + speedLeft + "  steerLeft: " + steerLeft + "\nRIGHT: " + speedRight + "  steerRight: " + steerRight);
    //these two formulas determine the final speeds for the left and right motor
    
    if (!isAllowedToDrive)
    {
        speedLeft = 0;      //sets the motor speeds to 0 to stop them. When robot is allowed to drive again, it will continue at the last set speed instead of resetting its speed.
        speedRight = 0;
        Serial1.println("STOPPED");
    }
}

void applyMotorValues() 
{
    motors.setLeftSpeed(speedLeft + getCorrectLeft());
    motors.setRightSpeed(speedRight + getCorrectRight());
}

const bool isStandingStill() {
  return (speedLeft == 0 && speedRight == 0);
}