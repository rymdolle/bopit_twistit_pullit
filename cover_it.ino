#include "cover_it.h"
#include "melody.h"

bool read_photosensor(uint8_t photoPin);

bool cover_it(uint8_t coverLed, uint8_t photoPin, long timeout)
{
  digitalWrite(coverLed, 1);

  long start = millis();
  while (millis() - start < timeout) {
    play_melody(timeout, false);
    if (read_photosensor()) {
      // Player succeeded to cover sensor
      digitalWrite(coverLed, 0);
      return true;
    }
  }

  // timeout reached
  digitalWrite(coverLed, 0);
  return false;
}

static bool read_photosensor()
{
  const int count = 10;
  int readings[count];
  int total = 0;
  int avg = 0;

  static int psensor_min = 1 << 14 - 1;
  static int psensor_max = 0;

  analogRead(photoPin); // stabilise sensor reading
  for (int i = 0; i < count; ++i) {
    readings[i] = analogRead(photoPin); // take average reading
    total += readings[i];
  }

  avg = total / count;

  int value = avg; // analogRead(photoPin);
  Serial.print("PhotoValue: ");
  Serial.print(value);
  Serial.print(" (");
  Serial.print(avg);
  Serial.print(") ");
  psensor_max = max(value, psensor_max);
  psensor_min = min(value, psensor_min);
  int diff = psensor_max - psensor_min;
  Serial.print(psensor_max);
  Serial.print(" ");
  Serial.print(psensor_min);
  Serial.print(" ");

  // Threshold is half the diff of max and min
  int threshold = diff / 2;
  Serial.print(threshold);
  Serial.println("");
  return (diff - (value - psensor_min)) > threshold;
}