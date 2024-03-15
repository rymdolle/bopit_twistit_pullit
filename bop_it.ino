#include "bop_it.h"
#include "melody.h"

bool bop_it(uint8_t bopLed, uint8_t bopBtn, long timeout)
{
  digitalWrite(bopLed, HIGH);

  long start = millis();
  while (millis() - start < timeout) {
    play_melody(timeout, false);
    if (digitalRead(bopBtn) == HIGH) {
      digitalWrite(bopLed, LOW);
      return true;
    }
   }
  digitalWrite(bopLed, LOW);
  return false;
}
