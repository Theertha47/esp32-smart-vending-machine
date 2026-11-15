
#include <Arduino.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// =====================================
// LCD CONFIGURATION
// =====================================

#define SDA_PIN 22
#define SCL_PIN 21

LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================
// KEYPAD CONFIGURATION
// =====================================

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

// =====================================
// MOTOR CONFIGURATION
// =====================================

// Motor 1 - ULN2003 Driver 1
const int motor1Pins[4] = {18, 19, 16, 17};

// Motor 2 - ULN2003 Driver 2
const int motor2Pins[4] = {25, 26, 27, 32};

// Half-step sequence
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
int position2 = 0;

const int stepsToRotate = 1024;

// =====================================
// PIN AUTHENTICATION
// =====================================

const char* validPins[] = {
  "1234", "5678", "2468", "1357"
};

const int pinCount = 4;

String enteredPin = "";
bool pinVerified = false;

// Check entered PIN
bool checkPin(String pin) {
  for (int i = 0; i < pinCount; i++) {
    if (pin == validPins[i]) {
      return true;
    }
  }
  return false;
}

// =====================================
// MOTOR FUNCTIONS
// =====================================

// Release motor coils
void releaseMotor(const int pins[4]) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(pins[i], LOW);
  }
}

// Rotate selected motor
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

// =====================================
// LCD FUNCTIONS
// =====================================

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

// =====================================
// DISPENSING FUNCTION
// =====================================

void dispense(int product) {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DISPENSING...");
  lcd.setCursor(0, 1);
  lcd.print("PRODUCT ");
  lcd.print(product);

  Serial.print("Dispensing Product ");
  Serial.println(product);

  if (product == 1) {

    Serial.println("Motor 1 rotating...");
    rotateMotor(motor1Pins, position1);

  } else if (product == 2) {

    Serial.println("Motor 2 rotating...");
    rotateMotor(motor2Pins, position2);
  }

  Serial.println("Motor cycle completed.");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MOTOR FINISHED");
  lcd.setCursor(0, 1);
  lcd.print("COLLECT ITEM");

  delay(2000);

  resetMachine();
}

// =====================================
// SETUP
// =====================================

void setup() {

  Serial.begin(115200);

  // LCD initialization
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

  // Initialize both motors
  for (int i = 0; i < 4; i++) {
    pinMode(motor1Pins[i], OUTPUT);
    pinMode(motor2Pins[i], OUTPUT);
  }

  releaseMotor(motor1Pins);
  releaseMotor(motor2Pins);

  resetMachine();
}

// =====================================
// MAIN LOOP
// =====================================

void loop() {

  char key = keypad.getKey();

  if (!key) return;

  // * = Reset / Logout
  if (key == '*') {
    resetMachine();
    return;
  }

  // Authentication stage
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
        lcd.print("SELECT 1 OR 2");

        Serial.println("ACCESS GRANTED");
        Serial.println("Select Product 1 or 2");

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

  // After successful authentication
  else {

    if (key == '1') {

      dispense(1);

    } else if (key == '2') {

      dispense(2);

    }
  }
}
