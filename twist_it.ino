#include "twist_it.h"

bool twist_it(uint8_t coverLed, uint8_t photoPin, long timeout) {
  digitalWrite(twistLed, HIGH);
  unsigned long start = millis();
  while (millis() - start < timeout) {
    play_melody(timeout, false);
    int currentValue = analogRead(twistPin);

    if (currentValue >= maxThreshold && !twistSuccess) {
      twistSuccess = true;
    }
    if (twistSuccess && currentValue <= minThreshold) {
      twistSuccess = false;
      digitalWrite(twistLed, LOW);
      return true;
    }
  }
  digitalWrite(twistLed, LOW);
  return false;
}
