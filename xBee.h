#pragma once
#include "buzzerStuff.h"

class xBee {

public:
  bool inputReceivedManual();
  void inputReceivedAutonomous();
private:
  buzzerStuff buzzer;
};