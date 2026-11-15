
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

const int motor1Pins[4] = {18, 19, 16, 17};
const int motor2Pins[4] = {25, 26, 27, 32};

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

// =====================================
// SECURITY LOCK CONFIGURATION
// =====================================

// NEW: Count incorrect PIN attempts
int failedAttempts = 0;

// NEW: Maximum incorrect attempts
const int maxAttempts = 3;

// NEW: Lock duration = 30 seconds
const unsigned long lockDuration = 30000UL;

// NEW: Lock status and starting time
bool systemLocked = false;
unsigned long lockStartTime = 0;

int lastSecondsShown = -1;

// =====================================
// CHECK PIN
// =====================================

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

void releaseMotor(const int pins[4]) {

  for (int i = 0; i < 4; i++) {
    digitalWrite(pins[i], LOW);
  }
}

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
// LCD AND RESET FUNCTIONS
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
// NEW: START SECURITY LOCK
// =====================================

void startLock() {

  systemLocked = true;
  pinVerified = false;
  enteredPin = "";

  lockStartTime = millis();
  lastSecondsShown = -1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SYSTEM LOCKED");

  lcd.setCursor(0, 1);
  lcd.print("RETRY IN 30 SEC");

  Serial.println("SYSTEM LOCKED FOR 30 SECONDS");
}

// =====================================
// NEW: HANDLE SECURITY LOCK
// =====================================

void handleLock() {

  unsigned long elapsed = millis() - lockStartTime;

  // Check whether 30 seconds have passed
  if (elapsed >= lockDuration) {

    systemLocked = false;
    failedAttempts = 0;
    lastSecondsShown = -1;

    Serial.println("SYSTEM UNLOCKED");

    resetMachine();
    return;
  }

  // Calculate seconds remaining
  int secondsLeft =
      (lockDuration - elapsed + 999UL) / 1000UL;

  // Update display only when seconds change
  if (secondsLeft != lastSecondsShown) {

    lastSecondsShown = secondsLeft;

    lcd.setCursor(0, 0);
    lcd.print("SYSTEM LOCKED   ");

    lcd.setCursor(0, 1);
    lcd.print("RETRY IN ");
    lcd.print(secondsLeft);
    lcd.print(" SEC   ");
  }
}

// =====================================
// DISPENSING FUNCTION
// =====================================

void dispense(int product) {

  // Prevent operation during lockout
  if (systemLocked || !pinVerified) {
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DISPENSING...");

  lcd.setCursor(0, 1);
  lcd.print("PRODUCT ");
  lcd.print(product);

  Serial.print("Dispensing Product ");
  Serial.println(product);

  if (product == 1) {

    rotateMotor(motor1Pins, position1);

  } else if (product == 2) {

    rotateMotor(motor2Pins, position2);

  } else {
    return;
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

  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

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

  // NEW: Security lock has first priority
  if (systemLocked) {

    handleLock();
    return;
  }

  char key = keypad.getKey();

  if (!key) return;

  // Clear entered PIN or logout
  // Does not reset the failed-attempt count
  if (key == '*') {

    resetMachine();
    return;
  }

  // ===================================
  // AUTHENTICATION
  // ===================================

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

        // Successful authentication
        pinVerified = true;
        enteredPin = "";

        // Reset failed attempts
        failedAttempts = 0;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PIN VERIFIED");

        lcd.setCursor(0, 1);
        lcd.print("SELECT 1 OR 2");

        Serial.println("ACCESS GRANTED");

      } else {

        // Wrong PIN
        failedAttempts++;

        Serial.print("Failed Attempts: ");
        Serial.println(failedAttempts);

        // Lock after 3 incorrect attempts
        if (failedAttempts >= maxAttempts) {

          startLock();

        } else {

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("WRONG PIN");

          lcd.setCursor(0, 1);
          lcd.print("ATTEMPT ");
          lcd.print(failedAttempts);
          lcd.print(" OF 3");

          delay(1500);

          resetMachine();
        }
      }
    }
  }

  // ===================================
  // PRODUCT SELECTION
  // ===================================

  else {

    if (key == '1') {

      dispense(1);

    } else if (key == '2') {

      dispense(2);
    }
  }
}
