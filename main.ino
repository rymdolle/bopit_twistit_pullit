// Actions section

byte bopLed = 12;
byte bopBtn = 3;

byte coverLed = 4;
byte photoPin = A1;
byte buzzer = 10;
const byte twistPin = A2;
const byte twistLed = 5;
const int maxThreshold = 500;
const int minThreshold = 300;
bool twistSuccess = false;

const int ledHealthPins[] = {6, 7, 8};
static int ledIndex = 0;
int ledCount = sizeof(ledHealthPins) / sizeof(ledHealthPins[0]);

unsigned long interval = 4000;


enum seqInstructions{
  cover,
  bop,
  twist,
  // Pull it eller pass it
  pull
};


enum PianoNoteFrequency {
  NOTE_G3 = 392,
  NOTE_A4 = 440,
  NOTE_B4 = 493,
  NOTE_C4 = 523,
};

int melody[] = {
  NOTE_C4,
  NOTE_G3,
  NOTE_A4,
  NOTE_B4,
};

bool play_melody(int interval, bool restart = false)
{
  static uint8_t note = 0;
  static long start = millis();
  if (restart) {
    note = 0;
    start = millis();
  }

  interval /= 4;
  if (millis() - start > interval) {
    // interval has passed, play next note
    const int length = sizeof(melody) / sizeof(melody[0]);
    Serial.print(note);
    tone(buzzer, melody[note % length], interval / length);
    ++note;
    start = millis();
    Serial.print(" ");
    Serial.println(note % length);
    if (note % length == 0) // melody wrapped around
      return true;
  }
  return false;
}

void setup() {
  pinMode(bopLed, OUTPUT);
  pinMode(bopBtn, INPUT);

  pinMode(twistLed, OUTPUT);
  pinMode(twistPin, INPUT);

  pinMode(coverLed, OUTPUT);
  pinMode(photoPin, INPUT);

  pinMode(buzzer, OUTPUT);

  for (int i = 0; i < ledCount; i++) {
    pinMode(ledHealthPins[i], OUTPUT);
    digitalWrite(ledHealthPins[i], HIGH);
  }

  Serial.begin(9600);
  randomSeed(analogRead(0));
  startBopIt();
}

void loop() {
  if (digitalRead(ledHealthPins[2]) == LOW) {
    // Check if last health pin is off, then its game over
    //blink_health(6, 100);
    delay(2000);
  }

  if (play_melody(interval, false))
    interval = max(1000, interval - 40);
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
  digitalWrite(bopLed, HIGH);

  long start = millis();
  while (millis() - start < interval) {
    play_melody(interval, false);
    if (digitalRead(bopBtn) == HIGH) {
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
    play_melody(interval, false);
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

bool twistIt()
{
  digitalWrite(twistLed, HIGH);
  long start = millis();
  while(millis() - start < interval) {
    play_melody(interval, false);
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

void failure()
{
  tone(buzzer,350);
  delay(200);
  tone(buzzer,200, 500);
  for (int i = 0; i < 6; i++) {
    digitalWrite(ledHealthPins[ledIndex], HIGH);
    delay(250);
    digitalWrite(ledHealthPins[ledIndex], LOW);
    delay(250);
  }
  delay(1000);
  ledIndex++;
  if (ledIndex >= ledCount) {
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(ledHealthPins[i],HIGH);
    }
    ledIndex = 0;
  }
  interval += 40;
  play_melody(interval, true);
}

void startBopIt()
{
  int blinkCount = 4;
  int delayTime = 200;
  // Loopa blinkCount gånger
  for (int i = 0; i < blinkCount; i++) {
    // Tänd lysdioderna
    digitalWrite(bopLed, HIGH);
    digitalWrite(twistLed, HIGH);
    digitalWrite(coverLed, HIGH);
    for (int k = 0; k < sizeof(ledHealthPins) / sizeof(ledHealthPins[0]); k++) {
      digitalWrite(ledHealthPins[k], HIGH);
    };
    // Fördröj i en kort stund
    delay(delayTime);
    // Släck lysdioderna
    digitalWrite(bopLed, LOW);
    digitalWrite(twistLed, LOW);
    digitalWrite(coverLed, LOW);
    for (int j = 0; j < sizeof(ledHealthPins) / sizeof(ledHealthPins[0]); j++) {
      digitalWrite(ledHealthPins[j], LOW);
    };
    // Fördröj igen innan nästa blinkning
    delay(delayTime);
  }
  for (int k = 0; k < sizeof(ledHealthPins) / sizeof(ledHealthPins[0]); k++) {
    digitalWrite(ledHealthPins[k], HIGH);
  };
}
