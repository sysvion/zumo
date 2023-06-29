#include <Wire.h>
#include <Zumo32U4.h>
#pragma once

class blockPusher {
public:
blockPusher();
  void setup();
  void turnRight();
  void turnLeft();
  void stop();
  void twin();
  void attack();
  void duwBlock();

private:
  bool ibne;
  bool himar;
  bool senseDir;
  const double minimum = 1;  //readings must be greater than this to
  const uint16_t turnSpeedMax = 300;
  const uint16_t turnSpeedMin = 100;
  const uint16_t deceleration = 10;
  const uint16_t acceleration = 10;

  bool turningLeft;
  bool turningRight;
  uint16_t turnSpeed;
  uint16_t lastTimeObjectSeen;
  int i;
};