#include <Wire.h>
#include <Zumo32U4.h>
#pragma once

class blockPusher {
  public:
  void turnRight();
  void turnLeft();
  void stop();
  void twin();
  void attack();
  void duwBlok();
  
  private:
  bool ibne = true;
  bool himar = false;
};