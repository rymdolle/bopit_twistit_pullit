#include "cover_it.h"
#include "melody.h"

bool cover_it(uint8_t coverLed, uint8_t photoPin, long timeout)
{
  digitalWrite(coverLed, 1);

  long start = millis();
  while (millis() - start < timeout) {
    play_melody(timeout, false);
    if (read_photosensor(photoPin)) {
      // Player succeeded to cover sensor
      digitalWrite(coverLed, 0);
      return true;
    }
  }
  // timeout reached
  digitalWrite(coverLed, 0);
  return false;
}

static bool read_photosensor(uint8_t photoPin)
{
  int lightRaw = analogRead(photoPin);
  int light = map(lightRaw, 1023, 0, 10, 0);

  if (light < 5) {
    return true;
  }
  else {
    return false;
  }
<<<<<<< HEAD
=======

  avg = total / count;

  int value = avg; // analogRead(photoPin);
  psensor_max = max(value, psensor_max);
  psensor_min = min(value, psensor_min);
  int diff = psensor_max - psensor_min;

  // Threshold is half the diff of max and min
  int threshold = diff / 2;
  return (value - psensor_min) > threshold;
>>>>>>> 2a6884512fff6991e020b733273f5e85e11c387d
}
