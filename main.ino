// Sequence section
byte r = 4;


// Actions section

byte bopLed = 2;
byte bopBtn = 3;

byte coverLed = 4;
byte photoPin = A1;
byte buzzer = 10;

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
  
   for (int i = 0; i < sizeof(ledHealthPins) / sizeof(ledHealthPins[0]); i++)
 {
    pinMode(ledHealthPins[i], OUTPUT);
    digitalWrite(ledHealthPins[i], HIGH);
 }

  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop() {
  
  if (digitalRead(ledHealthPins[2]) == LOW) {
    // Check if last health pin is off, then its game over
    blink_health(6, 100);
    delay(2000);
  }

  

  switch(random(3)) {
    case cover:
    Serial.println("Cover it");
    if (cover_it() == true) {
      Serial.println("Cover it succeeded.");
    } else {
      failure();
      Serial.println("Cover it failed");
      
    }
    break;
    case bop:
    Serial.println("Bop it");
    if (bopIt() == true) {
      Serial.println("Bop it succeeded.");
    } else {
      failure();
      Serial.println("Bop it failed");
    }
    break;
    case twist:
    Serial.println("Twist it");
    if (twistIt() == true) {
      Serial.println("Twist it succeeded.");
    } else {
      failure();
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

bool cover_it()
{
  digitalWrite(coverLed, 1);

  int start = millis();
  
  while (millis() - start < interval) {
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

bool read_photosensor()
{
  static int psensor_min = 1 << 10;
  static int psensor_max = 0;

  int value = analogRead(photoPin);
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

void failure()
  {
    
	tone(buzzer,350);
  	delay(200);
  	tone(buzzer,200, 500);  
  
    for (int i = 0; i < 6; i++)
    {
      digitalWrite(ledHealthPins[ledIndex], HIGH);
      delay(250);
      digitalWrite(ledHealthPins[ledIndex], LOW);
      delay(250);
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
