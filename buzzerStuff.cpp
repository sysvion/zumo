#include "buzzerStuff.h"
Zumo32U4Buzzer buzzer;

const int MIN_VOLUME = 6;
const int MAX_VOLUME = 13;
int volume = MIN_VOLUME + 4;

void play(int frequency, int durationMilliseconds)
{
  buzzer.playFrequency(frequency, durationMilliseconds, volume);
}

void lowerVolume()
{
  if (volume > MIN_VOLUME)          //checks if volume is already at minimum
  {
    volume -= 1;
    Serial1.println((String)"Volume: " + (volume-6));
    play(460, 300);
  }
}
void increaseVolume()
{
  if (volume < MAX_VOLUME)          //checks if volume is already at maximum
  {
    volume += 1;
    Serial1.println((String)"Volume: " + (volume-6));
    play(460, 300);
  }
}

void startupSound()
{
  buzzer.play(">a32>>c32");
}

void scanColorSound()
{
  buzzer.play(">a32");
}

void autonomousModeSound()
{
  buzzer.play(">a32>>c32>>a32");
}

void manualModeSound()
{
  buzzer.play(">a32>>c32>>e32");
}