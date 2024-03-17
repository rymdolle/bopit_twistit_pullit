#include <Wire.h>

/*
  Multiplayer is played by the master always polling the slave for
  game updates.

  If the slave fails, it replies with a game over message and a new
  game is started.

  If the master fails, it sends a game over message and a new game is
  started.
 */


enum MultiplayerCommand {
  MP_CONNECT,
  MP_START,
  MP_FAILURE,
  MP_SUCCESS,
};

bool isMaster = false;
int  address = 0x20;
bool multiplayerSetupSuccess = false;

// Return value is true if multiplayer mode is active
bool multiplayer_init(long timeout)
{
  Serial.println("Init multiplayer");
  {
    Serial.println("Trying as slave");
    Wire.begin(address);
    long start = millis();
    Wire.onReceive(multiplayer_receive);
    Wire.onRequest(multiplayer_request);
    // Spend half of timeout in slave search
    while (millis() - start < timeout / 2) {
      if (multiplayerSetupSuccess) {
        Serial.print("Multiplayer started as SLAVE with address ");
        Serial.println(address);
        return true;
      }
    }

    Serial.println("Could not find game leader");
    Wire.end();
  }

  {
    Serial.println("Trying as master");
    Wire.begin();
    long start = millis();
    Wire.setWireTimeout();
    // Half timeout as master search
    while (millis() - start < timeout / 2) {
      Wire.requestFrom(address, 1);
      if (Wire.available() > 0) {
        const byte cmd = Wire.read();
        if (cmd == MP_CONNECT) {
          isMaster = true;
          multiplayerSetupSuccess = true;
          Serial.print("Multiplayer started as MASTER with slave ");
          Serial.println(address);
          return true;
        }
      }
    }

    Serial.println("Could not find opponent");
    Wire.end();
  }

  return false;
}

void multiplayer_request() {
  Serial.println("onRequest");
  if (!multiplayerSetupSuccess) {
    multiplayerSetupSuccess = true;
    Wire.write(MP_CONNECT);
    return;
  }

  Wire.write(gameOver ? MP_FAILURE : MP_SUCCESS);
}

void multiplayer_receive(int bytes) {
  Serial.println("onReceive");
  for (int i = 0; i < bytes; ++i) {
    const byte cmd = Wire.read();
    switch (cmd) {
    case MP_START:
    case MP_FAILURE:
    case MP_SUCCESS:
    default:
      break;
    }
  }
}
