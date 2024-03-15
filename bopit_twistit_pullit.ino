#include "cover_it.h"
#include "twist_it.h"
#include "bop_it.h"
#include "melody.h"
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
const int ledCount = sizeof(ledHealthPins) / sizeof(ledHealthPins[0]);

unsigned long interval = 4000;
bool gameOver = false;
bool success = false;


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
  start();
}

void loop() {
  if (digitalRead(ledHealthPins[2]) == LOW) {
    // Check if last health pin is off, then its game over
    //blink_health(6, 100);
    gameOver = true;
    delay(2000);
  }

  switch(random(3)) {
  case cover:
    Serial.println("Cover it");
    if (cover_it(coverLed, photoPin, interval) == true) {
      Serial.println("Cover it succeeded.");
      success = true;
    } else {
      failure();
      Serial.println("Cover it failed");
    }
    break;
  case bop:
    Serial.println("Bop it");
    if (bop_it(bopLed, bopBtn, interval) == true) {
      Serial.println("Bop it succeeded.");
      success = true;
    } else {
      failure();
      Serial.println("Bop it failed");
    }
    break;
  case twist:
    Serial.println("Twist it");
    if (twist_it(twistLed, twistPin, interval) == true) {
      Serial.println("Twist it succeeded.");
      success = true;
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
  lower_interval(success);
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
  play_melody(interval, true);
}

void start()
{
  int blinkCount = 4;
  int delayTime = 200;
  // Loopa blinkCount gånger
  for (int i = 0; i < blinkCount; i++) {
    // Tänd lysdioderna
    digitalWrite(bopLed, HIGH);
    digitalWrite(twistLed, HIGH);
    digitalWrite(coverLed, HIGH);
    for (int k = 0; k < ledCount; k++) {
      digitalWrite(ledHealthPins[k], HIGH);
    };
    // Fördröj i en kort stund
    delay(delayTime);
    // Släck lysdioderna
    digitalWrite(bopLed, LOW);
    digitalWrite(twistLed, LOW);
    digitalWrite(coverLed, LOW);
    for (int j = 0; j < ledCount; j++) {
      digitalWrite(ledHealthPins[j], LOW);
    };
    // Fördröj igen innan nästa blinkning
    delay(delayTime);
  }
  for (int k = 0; k < ledCount; k++) {
    digitalWrite(ledHealthPins[k], HIGH);
  };
}

void lower_interval(bool success) {
  if (interval > 1000 && success) {
    interval -= 100;
    
    Serial.print("Interval is: ");
    Serial.println(interval);
  } else {
    Serial.print("Action failed or interval reached min: ");
    Serial.println(interval);
  }

  if (gameOver) {
    interval = 4000;
  }
}
