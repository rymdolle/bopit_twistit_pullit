
// Sequence section
byte r = 4;
int* genSeq = 0;
byte cutSeq = ' ';

// Actions section

byte bopLed = 2;
byte bopBtn = 3;

byte coverLed = 4;
byte photoPin = A1;

const byte twistPin = A2;
const byte twistLed = 5;
const int maxThreshold = 900;
const int minThreshold = 90;
bool twistSuccess = false;

const int ledHealthPins[] = {6, 7, 8};
static int ledIndex = 0;

const unsigned long interval = 2000;


enum seqInstructions{
  cover,
  bop,
  twist,
  // Pull it eller pass it
  pull
};

void setup() {
  pinMode(bopLed, OUTPUT);
  pinMode(bopBtn, INPUT);

  pinMode(twistLed, OUTPUT);

  pinMode(coverLed, OUTPUT);

  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop() {

  genSeq = generateSequence();

  for (int i = 0; i < r; i++) {
    cutSeq = genSeq[i];
    delay(1000);

    // Göra om en array och deklarera den vid varje loop
    // med r som storlek av arrayn

    switch(cutSeq) {
    case cover:
      Serial.println("Cover it");
      if (cover_it(coverLed, photoPin, interval) == true) {
        Serial.println("Cover it succeeded.");
      } else {
        health(false);
        Serial.println("Cover it failed");

      }
      break;
    case bop:
      Serial.println("Bop it");
      if (bopIt() == true) {
        Serial.println("Bop it succeeded.");
      } else {
        health(false);
        Serial.println("Bop it failed");
      }
      break;
    case twist:
      Serial.println("Twist it");
      if (twistIt() == true) {
        Serial.println("Twist it succeeded.");
      } else {
        health(false);
        Serial.println("Twist it failed");
      }
      break;
    case pull:
      Serial.println("Pull it");
      break;
    default:
      Serial.println("Something went wrong in the sequence instructions statement");
      break;
    }
  }

  Serial.println("Sequence complete");

  delay(1000);

}

int* generateSequence(){
  if(genSeq != nullptr){
    delete[] genSeq;
  }
  int* sequence = new int[r];

  for (int i = 0; i < r; i++) {
    sequence[i] = random(4);

  }

  r++;
  return sequence;
}

bool bopIt() {
  long start = millis();
  byte lRead = digitalRead(bopLed);

  digitalWrite(bopLed, HIGH);

  while (millis() - start < interval) {
    if (digitalRead(bopBtn) == HIGH) {
      Serial.println("You have succeeded!");
      digitalWrite(bopLed, LOW);
      return true;
    }
   }
  digitalWrite(bopLed, LOW);
  return false;
}

bool cover_it(uint8_t led_pin, uint8_t sensor_pin, int timeout)
{
  digitalWrite(led_pin, 1);

  int start = millis();

  while (millis() - start < timeout) {
    if (read_photosensor(sensor_pin)) {
      // Player succeeded to cover sensor
      digitalWrite(sensor_pin, 0);
      return true;
    }
  }

  // timeout reached
  digitalWrite(led_pin, 0);
  return false;
}

bool read_photosensor(uint8_t sensor_pin)
{
  static int psensor_min = 1 << 10;
  static int psensor_max = 0;

  int value = analogRead(sensor_pin);
  psensor_max = max(value, psensor_max);
  psensor_min = min(value, psensor_min);
  int diff = psensor_max - psensor_min;

  // Threshold is half the diff of max and min
  int threshold = diff / 2;
  return (diff - (value - psensor_min)) > threshold;
}


bool twistIt()
  {
  digitalWrite(twistLed, HIGH);
  int start = millis();
  while(millis() - start < interval) {
    int currentValue = analogRead(twistPin);

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

void health(bool health)
  {
    
  if (health == false) 
  {
    for (int i = 0; i < 10; i++)
    {
   digitalWrite(ledHealthPins[ledIndex], HIGH);
      delay(100);
   digitalWrite(ledHealthPins[ledIndex], LOW);
      delay(100);
    }
  }
    delay(1000);
    ledIndex++;
    if (ledIndex >= sizeof(ledHealthPins) / sizeof(ledHealthPins[0]))
    {
      for (int i = 0; i < sizeof(ledHealthPins) / sizeof(ledHealthPins[0]); i++)
      {
        digitalWrite(ledHealthPins[i],HIGH);
      }
      ledIndex = 0;
    }
  
  }

