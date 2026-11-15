
#include <Keypad.h>

// Keypad configuration
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

unsigned long previousTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println("SMART VENDING MACHINE");
  Serial.println("ESP32 Initialized Successfully");
  Serial.println("Keypad Ready");
}

void loop() {
  // Read the keypad
  char key = keypad.getKey();

  if (key != NO_KEY) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
  }

  // Keep the initialization heartbeat from v0.1
  if (millis() - previousTime >= 2000) {
    previousTime = millis();
    Serial.println("ESP32 is Running...");
  }
}
