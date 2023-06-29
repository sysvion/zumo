#include "buzzerStuff.h"
#include "motorsManual.h"
#pragma once

class xBee {

public:
  bool inputReceivedManual();
  void inputReceivedAutonomous();
  void manualLoop();
private:
  buzzerStuff buzzer;
  motorsManual m;
};
