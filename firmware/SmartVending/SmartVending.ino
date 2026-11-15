
#include <Arduino.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// ==================================
// LCD CONFIGURATION
// ==================================

#define SDA_PIN 22
#define SCL_PIN 21

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==================================
// KEYPAD CONFIGURATION
// ==================================

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {13, 14, 15, 12};
byte colPins[COLS] = {23, 33, 4, 5};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);

// ==================================
// MOTOR 1 CONFIGURATION
// ==================================

// ULN2003 Driver 1
// Revised pins for ESP32-WROOM-32
const int motor1Pins[4] = {18, 19, 16, 17};

// Half-step sequence from Day 1
const byte steps[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

int position1 = 0;

const int stepsToRotate = 1024;

// ==================================
// PIN AUTHENTICATION
// ==================================

const char* validPins[] = {
  "1234", "5678", "2468", "1357"
};

const int pinCount = 4;

String enteredPin = "";
bool pinVerified = false;

bool checkPin(String pin) {
  for (int i = 0; i < pinCount; i++) {
    if (pin == validPins[i]) {
      return true;
    }
  }
  return false;
}

// ==================================
// MOTOR CONTROL FUNCTIONS
// ==================================

// Turn off all motor coils
void releaseMotor(const int pins[4]) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(pins[i], LOW);
  }
}

// Rotate Motor 1
void rotateMotor(
  const int pins[4],
  int &position
) {
  for (int i = 0; i < stepsToRotate; i++) {

    position = (position + 1) % 8;

    for (int j = 0; j < 4; j++) {
      digitalWrite(pins[j], steps[position][j]);
    }

    delayMicroseconds(2000);
  }

  releaseMotor(pins);
}

// ==================================
// LCD FUNCTIONS
// ==================================

void showWelcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER 4-DIGIT");
  lcd.setCursor(0, 1);
  lcd.print("PIN: ");
}

void resetMachine() {
  enteredPin = "";
  pinVerified = false;

  showWelcome();
  Serial.println("ENTER 4-DIGIT PIN:");
}

// ==================================
// DISPENSING FUNCTION
// ==================================

void dispenseMotor1() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DISPENSING...");
  lcd.setCursor(0, 1);
  lcd.print("PRODUCT 1");

  Serial.println("Motor 1 rotating...");

  rotateMotor(motor1Pins, position1);

  Serial.println("Motor 1 stopped.");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MOTOR FINISHED");
  lcd.setCursor(0, 1);
  lcd.print("COLLECT ITEM");

  delay(2000);

  resetMachine();
}

// ==================================
// SETUP
// ==================================

void setup() {
  Serial.begin(115200);

  // Initialize LCD
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

  // Initialize Motor 1
  for (int i = 0; i < 4; i++) {
    pinMode(motor1Pins[i], OUTPUT);
  }

  releaseMotor(motor1Pins);

  resetMachine();
}

// ==================================
// MAIN LOOP
// ==================================

void loop() {

  char key = keypad.getKey();

  if (!key) return;

  // Clear / Logout
  if (key == '*') {
    resetMachine();
    return;
  }

  // PIN not verified yet
  if (!pinVerified) {

    if (key >= '0' && key <= '9') {

      if (enteredPin.length() < 4) {
        enteredPin += key;

        lcd.print("*");
        Serial.print("*");
      }
    }

    else if (key == '#') {

      Serial.println();

      if (enteredPin.length() == 4 &&
          checkPin(enteredPin)) {

        pinVerified = true;
        enteredPin = "";

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PIN VERIFIED");
        lcd.setCursor(0, 1);
        lcd.print("PRESS 1 FOR ITEM");

        Serial.println("ACCESS GRANTED");
        Serial.println("Press 1 for Product 1");

      } else {

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("INVALID PIN");
        lcd.setCursor(0, 1);
        lcd.print("TRY AGAIN");

        Serial.println("INVALID PIN");

        delay(1500);
        resetMachine();
      }
    }
  }

  // PIN verified
  else {

    if (key == '1') {
      dispenseMotor1();
    }

  }
}
