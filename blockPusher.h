#include <Wire.h>
#include <Zumo32U4.h>
#pragma once

class ProximZumo {
  public:
  void turnRight();
  void turnLeft();
  void stop();
  void twin();
  void attack();
  void duwBlok();

  private:
  bool ibne;
  bool himar;
}