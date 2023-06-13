#include "ProximZumo.h"

Zumo32U4Motors motors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4ButtonA buttonA;

const uint8_t minimum = 1; //readings must be greater than this to 
const uint16_t turnSpeedMax = 300;
const uint16_t turnSpeedMin = 100;
const uint16_t deceleration = 10;
const uint16_t acceleration = 10;

#define LEFT 0
#define RIGHT 1

bool senseDir = RIGHT;
ibne = true;
himar = false;
bool turningLeft = false;
bool turningRight = false;
uint16_t turnSpeed = 200;
uint16_t lastTimeObjectSeen = 0;
int i = 0;

void turnRight(){ //turns right towards block
  motors.setSpeeds(turnSpeed, -turnSpeed);
  turningLeft = false;
  turningRight = true;
}

void turnLeft(){ //turns left towards block
  motors.setSpeeds(-turnSpeed, turnSpeed);
  turningLeft = true;
  turningRight = false;
}

void stop(){ //stops moving
  motors.setSpeeds(0, 0);
  turningLeft = false;
  turningRight = false;
}
void twin(){ //it goes forward 20 cm
  motors.setSpeeds(270, 270);
  delay(1000);
  motors.setSpeeds(0, 0);
  ibne = false;
}

void attack(){ //named this attack because it attacks the block
  if (i <= 10) {
  motors.setSpeeds(400, 400);
  //delay(20);
  //i++
  }
}

void duwBlok(){ //the code that pushes the block out of the circle
  if (ibne) {
    twin();
  }
  //if (himar){
  proxSensors.read();
  uint8_t leftValue = proxSensors.countsFrontWithLeftLeds() ;
  uint8_t rightValue = proxSensors.countsFrontWithRightLeds();

  // Determine if an object is visible or not.
  bool objectSeen = leftValue >= minimum || rightValue >= minimum;
  
  printReadingsToSerial();

  if (objectSeen)
  {
    
    // An object is visible, so we will start decelerating in
    // order to help the robot find the object without
    // overshooting or oscillating.
    turnSpeed = 200;
  }
  else
  {
    // An object is not visible, so we will accelerate in order
    // to help find the object sooner.
    turnSpeed += acceleration;
  }

  // Constrain the turn speed so it is between turnSpeedMin and
  // turnSpeedMax.
  turnSpeed = constrain(turnSpeed, turnSpeedMin, turnSpeedMax);

  if (objectSeen)
  {
    // An object seen.
    ledYellow(1);


    lastTimeObjectSeen = millis();

    if (leftValue < rightValue)
    {
      if (rightValue-leftValue >minimum){ //minimum was 1 first

      // The right value is greater, so the object is probably
      // closer to the robot's right LEDs, which means the robot
      // is not facing it directly.  Turn to the right to try to
      // make it more even.
      turnRight();
      senseDir = RIGHT;
      }
      else{
        stop();
      }
    }
    else if (leftValue > rightValue)
    {
      if (leftValue-rightValue > minimum){ //minimum was 1 first

      // The left value is greater, so turn to the left.
      turnLeft();
      senseDir = LEFT;
    }
  }

    else
    {
      // The values are equal, so stop the motors.
      //stop();
      attack();
    }
  }
  else
  {
    // No object is seen, so just keep turning in the direction
    // that we last sensed the object.
    ledYellow(0);

    if (senseDir == RIGHT)
    {
      turnRight();
    }
    else
    {
      turnLeft();
    }
  }
  
}