#include "Logger.h"

namespace argos {

void Logger::begin(const Config& config) {
    Serial.begin(config.serialBaudRate());
    delay(1000);

    printStartupBanner(config);
}

void Logger::info(const __FlashStringHelper* message) const {
    Serial.println(message);
}

void Logger::info(const char* message) const {
    Serial.println(message);
}

void Logger::printStartupBanner(const Config& config) const {
    Serial.println();
    Serial.println(F("--------------------------------"));
    Serial.println(F("ARGOS Node"));
    Serial.print(F("Firmware version: "));
    Serial.println(config.firmwareVersion());
    Serial.print(F("Build date: "));
    Serial.print(F(__DATE__));
    Serial.print(F(" "));
    Serial.println(F(__TIME__));
    Serial.print(F("Board: "));
    Serial.println(config.boardName());
    Serial.println(F("--------------------------------"));
}

}  // namespace argos
