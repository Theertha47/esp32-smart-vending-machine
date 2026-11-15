
void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println("SMART VENDING MACHINE");
    Serial.println("ESP32 Initialized Successfully");
}

void loop() {
    Serial.println("ESP32 is Running...");
    delay(2000);
}
