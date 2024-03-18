#include "melody.h"
#include <stdint.h>

bool play_melody(long interval, bool restart) {
  if (disableBuzzer)
    return;
  static uint8_t note = 0;
  static unsigned long start = millis();
  if (restart) {
    note = 0;
    start = millis();
  }

  interval /= 4;
  if (millis() - start > interval) {
    // interval has passed, play next note
    const int length = sizeof(melody) / sizeof(melody[0]);
    tone(buzzer, melody[note % length], interval / length);
    ++note;
    start = millis();
    if (note % length == 0)  // melody wrapped around
      return true;
  }
  return false;
}
