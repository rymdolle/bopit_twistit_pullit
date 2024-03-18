#include <Wire.h>

bool isMaster = false;
int slave_address = 0x20;
bool multiplayerSetupSuccess = false;

// Return value is true if multiplayer mode is active
bool multiplayer_init()
{
  Serial.println("Init multiplayer");
  // Set I2C to slave address to look for others
  Wire.begin(slave_address);
  long timeout = 4000;
  long start = millis();
  Wire.onReceive(multiplayer_onReceive);
  while (millis() - start < timeout) {
    led_loading(500);
    if (multiplayerSetupSuccess) {
      Serial.print("Multiplayer started as SLAVE address ");
      Serial.println(slave_address);
      return true;
    }
  }

  Serial.println("Could not find master, trying as master");
  Wire.end();

  // Try again as master
  start = millis();
  Wire.begin();
  Wire.onReceive(multiplayer_onReceive);
  while (millis() - start < timeout) {
    led_loading(500);
    Wire.beginTransmission(slave_address);
    Wire.write(slave_address);
    Wire.endTransmission();
    Wire.requestFrom(slave_address, 1, true);
    if (Wire.available() > 0) {
      Wire.read();
      isMaster = true;
      Serial.print("Multiplayer started as MASTER with slave ");
      multiplayerSetupSuccess = true;
      Serial.println(slave_address);
      return true;
    }
  }

  Serial.println("Multiplayer is not set up");
  Wire.end();
  return false;
}

void multiplayer_onRequest() {
  Serial.println("Request");
}

void multiplayer_onReceive(int bytes) {
  Serial.print("Got ");
  Serial.print(bytes);
  Serial.print(" bytes");
  Serial.println();
  for (int i = 0; i < bytes; ++i) {
    byte cmd = Wire.read();
    Serial.print("Command: ");
    Serial.println(cmd);
    multiplayerSetupSuccess = true;
  }
}

void led_loading(long timeout)
{
  static byte index = 0;
  static long start = millis();
  for (int i = 0; i < ledCount; ++i) {
    digitalWrite(ledHealthPins[i], 0);
  }
  digitalWrite(ledHealthPins[index % ledCount], 1);
  if (millis() - start > timeout) {
    start = millis();
    index++;
  }
}
