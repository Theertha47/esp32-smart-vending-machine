
#include <Keypad.h>

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

// Reset authentication
void resetMachine() {
  enteredPin = "";
  pinVerified = false;

  Serial.println("ENTER 4-DIGIT PIN:");
}

// Setup
void setup() {
  Serial.begin(115200);
  resetMachine();
}

// Main loop
void loop() {
  char key = keypad.getKey();

  if (!key) return;

  // * = Clear PIN / Logout
  if (key == '*') {
    resetMachine();
    return;
  }

  if (!pinVerified) {

    // Accept numeric keys
    if (key >= '0' && key <= '9') {
      if (enteredPin.length() < 4) {
        enteredPin += key;
        Serial.print("*");
      }
    }

    // # = Verify PIN
    else if (key == '#') {
      Serial.println();

      if (enteredPin.length() == 4 &&
          checkPin(enteredPin)) {

        pinVerified = true;
        Serial.println("PIN VERIFIED");
        Serial.println("ACCESS GRANTED");

      } else {
        Serial.println("INVALID PIN");
        resetMachine();
      }
    }
  }
}
