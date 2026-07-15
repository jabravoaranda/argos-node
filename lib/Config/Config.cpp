#include "Config.h"

#ifndef ARGOS_FIRMWARE_VERSION
#define ARGOS_FIRMWARE_VERSION "0.1.0"
#endif

#ifndef ARGOS_BOARD_NAME
#define ARGOS_BOARD_NAME "Waveshare ESP32-S3-POE-ETH-8DI-8RO"
#endif

namespace argos {

void Config::begin() {}

void Config::update() {}

const char* Config::firmwareVersion() const {
    return ARGOS_FIRMWARE_VERSION;
}

const char* Config::boardName() const {
    return ARGOS_BOARD_NAME;
}

unsigned long Config::serialBaudRate() const {
    return 115200UL;
}

}  // namespace argos
