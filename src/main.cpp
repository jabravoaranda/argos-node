#include <Arduino.h>

void setup() {
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("==================================");
    Serial.println("      ARGOS NODE");
    Serial.println("      Boot OK");
    Serial.println("==================================");
}

void loop() {
    Serial.printf("Uptime: %lu s\n", millis() / 1000);
    delay(1000);
}