
#include <Arduino.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// LCD configuration
#define SDA_PIN 22
#define SCL_PIN 21

LiquidCrystal_I2C lcd(0x27, 16, 2);

// 4x4 Keypad
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
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

// Predefined 4-digit PINs
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

// Display PIN entry screen
void showWelcome() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER 4-DIGIT");
  lcd.setCursor(0, 1);
  lcd.print("PIN: ");
}

// Reset authentication
void resetMachine() {
  enteredPin = "";
  pinVerified = false;

  showWelcome();
  Serial.println("ENTER 4-DIGIT PIN:");
}

void setup() {
  Serial.begin(115200);

  // Initialize I2C LCD
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

  resetMachine();
}

void loop() {
  char key = keypad.getKey();

  if (!key) return;

  // * = Clear PIN / Logout
  if (key == '*') {
    resetMachine();
    return;
  }

  if (!pinVerified) {

    // Read numeric keys
    if (key >= '0' && key <= '9') {
      if (enteredPin.length() < 4) {
        enteredPin += key;

        // Hide PIN digits on LCD
        lcd.print("*");

        Serial.print("*");
      }
    }

    // # = Submit PIN
    else if (key == '#') {
      Serial.println();

      if (enteredPin.length() == 4 &&
          checkPin(enteredPin)) {

        pinVerified = true;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PIN VERIFIED");
        lcd.setCursor(0, 1);
        lcd.print("ACCESS GRANTED");

        Serial.println("ACCESS GRANTED");

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
}
