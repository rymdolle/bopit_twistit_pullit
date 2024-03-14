#include "twist_it.h"

bool twist_it(uint8_t coverLed, uint8_t photoPin, long timeout)
{
  digitalWrite(twistLed, HIGH);
  long start = millis();
  while(millis() - start < timeout) {
    play_melody(timeout, false);
    int currentValue = analogRead(twistPin);
    Serial.println(currentValue);

    if (currentValue >= maxThreshold && !twistSuccess) {
      twistSuccess = true;
    }
    if (twistSuccess && currentValue <= minThreshold) {
      //Serial.println("Twist-It Success!"); // Just 4 testing

      twistSuccess = false;
      digitalWrite(twistLed, LOW);
      return true;
    }
  }

  digitalWrite(twistLed, LOW);
  return false;
}
