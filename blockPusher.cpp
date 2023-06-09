#include "blockPusher.h"
#include <Zumo32U4.h>

Zumo32U4Motors motorsv2;
Zumo32U4ProximitySensors proxSensors;

const uint8_t minimum = 0;  //readings must be greater than this to
const uint16_t turnSpeedMax = 300;
const uint16_t turnSpeedMin = 100;
const uint16_t deceleration = 10;
const uint16_t acceleration = 10;

#define LEFT 0
#define RIGHT 1

bool senseDir = RIGHT;

bool turningLeft = false;
bool turningRight = false;
uint16_t turnSpeed = 200;
uint16_t lastTimeObjectSeen = 0;
int i = 0;

// turns right towards block
void blockPusher::turnRight() {
  motorsv2.setSpeeds(turnSpeed, -turnSpeed);
  turningLeft = false;
  turningRight = true;
}

void blockPusher::setup() {
  proxSensors.initFrontSensor();
}
//turns left towards block
void blockPusher::turnLeft() {
  motorsv2.setSpeeds(-turnSpeed, turnSpeed);
  turningLeft = true;
  turningRight = false;
}

//stops moving
void blockPusher::stop() {
  motorsv2.setSpeeds(0, 0);
  turningLeft = false;
  turningRight = false;
}

//it goes forward 20 cm
void blockPusher::twin() {
  motorsv2.setSpeeds(270, 270);
  delay(1000);
  motorsv2.setSpeeds(0, 0);
  ibne = false;
}

//named this attack because it attacks the block
void blockPusher::attack() {
  if (i <= 10) {
    motorsv2.setSpeeds(400, 400);
    //delay(20);
    //i++
  }
}

//the code that pushes the block out of the circle
void blockPusher::duwBlock() {
  if (ibne) {
    twin();
  }
  // if (himar){
  proxSensors.read();
  uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
  uint8_t rightValue = proxSensors.countsFrontWithRightLeds();

  // Determine if an object is visible or not.
  bool objectSeen = leftValue >= minimum || rightValue >= minimum;

  // printReadingsToSerial();

  if (objectSeen) {

    // An object is visible, so we will start decelerating in
    // order to help the robot find the object without
    // overshooting or oscillating.
    turnSpeed = 200;
  } else {
    // An object is not visible, so we will accelerate in order
    // to help find the object sooner.
    turnSpeed += acceleration;
  }

  // Constrain the turn speed so it is between turnSpeedMin and
  // turnSpeedMax.
  turnSpeed = constrain(turnSpeed, turnSpeedMin, turnSpeedMax);

  if (objectSeen) {
    // An object seen.
    ledYellow(1);

    lastTimeObjectSeen = millis();

    if (leftValue < rightValue) {
      if (rightValue - leftValue > minimum) {  // minimum was 1 first

        // The right value is greater, so the object is probably
        // closer to the robot's right LEDs, which means the robot
        // is not facing it directly.  Turn to the right to try to
        // make it more even.
        turnRight();
        senseDir = RIGHT;
      } else {
        stop();
      }
    } else if (leftValue > rightValue) {
      if (leftValue - rightValue > minimum) {  // minimum was 1 first

        // The left value is greater, so turn to the left.
        turnLeft();
        senseDir = LEFT;
      }
    } else {
      // The values are equal, so stop the motors.
      // stop();
      attack();
    }
  } else {
    // No object is seen, so just keep turning in the direction
    // that we last sensed the object.
    ledYellow(0);

    if (senseDir == RIGHT) {
      turnRight();
    } else {
      turnLeft();
    }
  }
}