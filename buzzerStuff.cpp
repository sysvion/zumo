#include "buzzerStuff.h"
Zumo32U4Buzzer buzzer;

const int MIN_VOLUME = 6;
const int MAX_VOLUME = 13;
int volume = MIN_VOLUME + 4;

void play(int frequency, int durationMilliseconds) {
  buzzer.playFrequency(frequency, durationMilliseconds, volume);
}

void lowerVolume() {
  if (volume > MIN_VOLUME)  //checks if volume is already at minimum
  {
    volume -= 1;
    Serial1.println((String) "Volume: " + (volume - 6));
    play(460, 300);
  }
}
void increaseVolume() {
  if (volume < MAX_VOLUME)  //checks if volume is already at maximum
  {
    volume += 1;
    Serial1.println((String) "Volume: " + (volume - 6));
    play(460, 300);
  }
}

void startupSound() {
  buzzer.play(">a32>>c32");
}

void scanColorSound() {
  buzzer.play(">a32");
}

void autonomousModeSound() {
  buzzer.play(">a32>>c32>>a32");
}

void manualModeSound() {
  buzzer.play(">a32>>c32>>e32");
}

int playSoundById(int id) {
  switch (id) {
    case 1:
      play(400, 200);
      break;
    case 2:
      play(420, 200);
      break;
    case 3:
      play(400, 100);
      delay(140);
      play(400, 100);
      break;
    case 4:
      play(540, 80);
      delay(120);
      play(540, 80);
      break;
    case 5:
      play(600, 80);
      delay(120);
      play(600, 100);
      break;
    case 6:
      ledYellow(1);
      play(280, 80);
      delay(50);
      ledYellow(0);
      delay(50);
      ledYellow(1);
      play(280, 80);
      delay(100);
      ledYellow(0);
      play(280, 80);
      break;
    case 7:
      ledYellow(1);
      play(280, 80);
      delay(50);
      ledYellow(0);
      delay(50);
      ledYellow(1);
      play(300, 80);
      delay(100);
      ledYellow(0);
      play(280, 80);
      break;
    case 8:
      play(300, 80);
      delay(120);
      play(280, 80);
      break;
    case 9:
      play(280, 80);
      delay(120);
      play(300, 80);
      break;
  }
  return 0;
}