#include "cover_it.h"
#include "twist_it.h"
#include "bop_it.h"
#include "melody.h"

// Actions section
byte rRGB = 2;
byte gRGB = 9;
byte bRGB = 11;
String chosenMode = "classic";


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
  twist
};


void setup() {
  pinMode(rRGB, OUTPUT);
  pinMode(gRGB, OUTPUT);
  pinMode(bRGB, OUTPUT);

  pinMode(bopLed, OUTPUT);
  pinMode(bopBtn, INPUT);

  pinMode(twistLed, OUTPUT);
  pinMode(twistPin, INPUT);

  pinMode(coverLed, OUTPUT);
  pinMode(photoPin, INPUT);

  pinMode(buzzer, OUTPUT);

  for (int i = 0; i < ledCount; i++) {
    pinMode(ledHealthPins[i], OUTPUT);
  }
  Serial.begin(9600);
  randomSeed(analogRead(0));
  // Sets RGB to classic as the default game mode is classic
  setRGB(0,0,255);
  // Runs the set up of the game
  start();
  multiplayer_init();
}

void loop() {

  // Checks for which game was chosen
  if (chosenMode == "classic") {
    classicMode();
  } else if (chosenMode == "multiplayer") {
    multiplayerMode();
  }

  // Lowers the interval depending on if the game action was successful
 lower_interval(success);
}

void failure()
{
  success = false;
  // Emits a tone if the game action was failed
  tone(buzzer,350);
  delay(200);
  tone(buzzer,200, 500);
  // Runs the classic set up for blinking the lights
  if (chosenMode == "classic"){
  for (int i = 0; i < 6; i++) {
    digitalWrite(ledHealthPins[ledIndex], HIGH);
    delay(250);
    digitalWrite(ledHealthPins[ledIndex], LOW);
    delay(250);
  }
  delay(1000);
  ledIndex++;
  if (ledIndex >= ledCount) {
    interval = 4000;

    for (int i = 0; i < ledCount; i++) {
      digitalWrite(ledHealthPins[i],HIGH);
    }
    ledIndex = 0;
  }
  play_melody(interval, true);
  // Runs the multiplayer mode for blinking the light
  } else if(chosenMode == "multiplayer") {

      for (int i = 0; i < 6; i++) {
    digitalWrite(ledHealthPins[2], HIGH);
    delay(250);
    digitalWrite(ledHealthPins[2], LOW);
    delay(250);
  }
  delay(1000);
  if (digitalRead(ledHealthPins[2]) == LOW) {
    interval = 4000;
    digitalWrite(ledHealthPins[2],HIGH);
    gameOver = true;
  }
  play_melody(interval, true);
  }
}

void start()
{
  setGameMode();

  int blinkCount = 4;
  int delayTime = 200;
  // Loops through the amount of blinkCount
  if (chosenMode == "classic") {
  for (int i = 0; i < blinkCount; i++) {
    digitalWrite(bopLed, HIGH);
    digitalWrite(twistLed, HIGH);
    digitalWrite(coverLed, HIGH);
    for (int k = 0; k < ledCount; k++) {
      digitalWrite(ledHealthPins[k], HIGH);
    };
    delay(delayTime);
    digitalWrite(bopLed, LOW);
    digitalWrite(twistLed, LOW);
    digitalWrite(coverLed, LOW);
    for (int j = 0; j < ledCount; j++) {
      digitalWrite(ledHealthPins[j], LOW);
    };
    delay(delayTime);
  }
  for (int k = 0; k < ledCount; k++) {
    digitalWrite(ledHealthPins[k], HIGH);
  }
  // Only flash one light in multiplayer mode
  } else if(chosenMode == "multiplayer") {
      for (int i = 0; i < blinkCount; i++) {
    digitalWrite(bopLed, HIGH);
    digitalWrite(twistLed, HIGH);
    digitalWrite(coverLed, HIGH);
    digitalWrite(ledHealthPins[2], HIGH);
    delay(delayTime);
    digitalWrite(bopLed, LOW);
    digitalWrite(twistLed, LOW);
    digitalWrite(coverLed, LOW);
    digitalWrite(ledHealthPins[2], LOW);
    delay(delayTime);
  }
  digitalWrite(ledHealthPins[2], HIGH);
  }
}

void lower_interval(bool success) {
  // Lowers the interval between action and music if the action was successful and above 1000ms
  if (interval > 1000 && success) {
    interval -= 100;
    
    Serial.print("Interval is: ");
    Serial.println(interval);
  } else {
    Serial.print("Action failed or interval reached min: ");
    Serial.println(interval);
  }
}

void setRGB(byte r, byte g, byte b) {
  analogWrite(rRGB, r);
  analogWrite(gRGB, g);
  analogWrite(bRGB, b);
}

void setGameMode() {
  bool toggle = true;
  int timeout = 4000;
  int start = millis();

  while (millis() - start < timeout){
  byte btnRead = digitalRead(bopBtn);

  if ((btnRead == HIGH) && toggle) {
    // Sets game mode to multiplayer
    setRGB(0,255,0);
    chosenMode = "multiplayer";
    toggle = false;

  } else if((btnRead == HIGH) && !toggle) {
    // Sets game mode to classic
    setRGB(0,0,255);
    chosenMode = "classic";
    toggle = true;
    
    }
  }
}

void classicMode() {
  // Runs vanilla game loop
  gameLoop(random(3));
}

void multiplayerMode() {
  // Declare and initalize variables used for multiplayer purpose
  byte maxLevel = 30;
  byte currentLevel = 0;
  bool win = false;

  // Runs vanilla game loop
  gameLoop(random(3));

  // Increases level counter and checks if level 30 has been reached
  if (success) {
    currentLevel++;
  } else if(currentLevel == maxLevel) {
    win = true;
  }
 
}

void gameLoop(byte randomNum) {
  // The switch case calls every game action and makes a check on the return value
  // if it does not return true it counts as a failed action and the failure method is called
  switch(randomNum) {
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
  default:
    Serial.println("Something went wrong in the sequence instructions statement");
    break;
  }
}
