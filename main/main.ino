// Sequence section
byte r = 4;

// Actions section
byte bopLed = 2;
byte bopBtn = 3;
byte coverLed = 4;
byte photoPin = A1;
byte buzzer = 10;

//twistIt
const byte twistPin = A2;
const byte twistLed = 5;
const int maxThreshold = 220;
const int minThreshold = 90;
bool twistSuccess = false;
//
const int delayTime = 500;
const int blinkCount = 3;

byte ledHealthPins[] = {6, 7, 8};
static int ledIndex = 0;

unsigned long interval = 2000;

//Melody
#define NOTE_A4  440
#define NOTE_CS5 554
#define NOTE_B4  494
#define NOTE_D5  587
#define REST      0 
int tempo = 240;                        // change this to make the song slower or faster
int melody[] = {
  REST,2, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //1
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2,
};
// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;   // this calculates the duration of a whole note in ms
int divider = 0, noteDuration = 0;

enum seqInstructions{
  cover,
  bop,
  twist,

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
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }
    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);
    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
  
  startBopit();

  Serial.begin(9600);
  randomSeed(analogRead(0));
}
void startBopit()
{
  // Loopa blinkCount gånger
  for (int i = 0; i < blinkCount; i++) {
    // Tänd lysdioderna
    digitalWrite(bopLed, HIGH);
    digitalWrite(twistLed, HIGH);
    digitalWrite(coverLed, HIGH);
    
    for (int k = 0; k < sizeof(ledHealthPins) / sizeof(ledHealthPins[0]); k++)
    {
    digitalWrite(ledHealthPins[k], HIGH);
    };
    // Fördröj i en kort stund sedan släck LED
    delay(delayTime);
    digitalWrite(bopLed, LOW);
    digitalWrite(twistLed, LOW);
    digitalWrite(coverLed, LOW);
    
    for (int j = 0; j < sizeof(ledHealthPins) / sizeof(ledHealthPins[0]); j++)
    {
    digitalWrite(ledHealthPins[j], LOW);
     };
    delay(delayTime);
  }
 }

void loop() {
  

  if (digitalRead(ledHealthPins[2]) == LOW) {
    // Check if last health pin is off, then its game over
    //blink_health(6, 100);
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

  long start = millis();
  
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
  long start = millis();
  while(millis() - start < interval) {
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
