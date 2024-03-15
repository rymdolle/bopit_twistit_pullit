#include <Wire.h>

bool isMaster = false;
byte slave_address = 0x20;

// Return value is true if multiplayer mode is active
bool multiplayer_init()
{
  // Set I2C to slave address to look for others
  Wire.begin(slave_address);
  const long timeout = 2000;
  long start = millis();
  while (millis() - start < timeout) {
    if (Wire.available() > 0) {
      // Read one byte as address
      byte address = Wire.read();
      Wire.write(address);
      // We are now slave

      // Set address to given address
      Wire.end();
      Wire.begin(address);
      slave_address = address;
      Serial.print("Multiplayer started as SLAVE address ");
      Serial.println(address);
      return true;
    }
  }

  // Try again as master
  start = millis();
  Wire.begin();
  while (millis() - start < timeout) {
    Wire.write(slave_address);
    if (Wire.available() > 0) {
      byte address = Wire.read();
      isMaster = true;
      Serial.print("Multiplayer started as MASTER with slave ");
      Serial.println(address);
      return true;
    }
  }

  Serial.println("Multiplayer is not set up");
  Wire.end();
  return false;
}
