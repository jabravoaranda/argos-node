#include "Relays.h"

#include <Wire.h>
#include <stdio.h>

namespace argos {

void Relays::begin(const Config& config, const Logger& logger) {
    (void)config;
    logger_ = &logger;

    Wire.begin(kI2cSdaPin, kI2cSclPin);

    outputState_ = kAllRelaysOff;
    if (!writeRegister(kTca9554OutputRegister, outputState_)) {
        logger_->info(F("Relays: failed to write OFF state"));
        return;
    }

    logger_->info(F("Relays: configuring TCA9554 pins as outputs"));
    if (!writeRegister(kTca9554ConfigRegister, kAllPinsOutputMode)) {
        logger_->info(F("Relays: failed to configure TCA9554 outputs"));
        return;
    }

    logger_->info(F("Relay subsystem initialized"));
    logger_->info(F("All relays OFF"));
}

void Relays::update() {}

#if (defined(ARGOS_ENABLE_RELAY_SELFTEST) && ARGOS_ENABLE_RELAY_SELFTEST) || \
    (defined(ARGOS_ENABLE_SERIAL_DEV_COMMANDS) && ARGOS_ENABLE_SERIAL_DEV_COMMANDS)
bool Relays::testRelay(uint8_t relay) {
    if (logger_ != nullptr) {
        char message[24] = {};
        snprintf(message, sizeof(message), "Testing relay %u", relay);
        logger_->info(message);
    }

    if (!setRelay(relay, true)) {
        return false;
    }

    delay(3000);

    return setRelay(relay, false);
}
#endif

bool Relays::allOff() {
    outputState_ = kAllRelaysOff;
    if (!writeRegister(kTca9554OutputRegister, outputState_)) {
        if (logger_ != nullptr) {
            logger_->info(F("Relays: failed to write OFF state"));
        }
        return false;
    }

    if (logger_ != nullptr) {
        logger_->info(F("All relays OFF"));
    }
    return true;
}

bool Relays::isOn(uint8_t relay) const {
    if (!isValidRelay(relay)) {
        return false;
    }

    const uint8_t bit = static_cast<uint8_t>(1U << (relay - 1U));
    return (outputState_ & bit) != 0;
}

bool Relays::setRelay(uint8_t relay, bool enabled) {
    if (!isValidRelay(relay)) {
        if (logger_ != nullptr) {
            logger_->info(F("Relays: invalid relay number"));
        }
        return false;
    }

    const uint8_t bit = static_cast<uint8_t>(1U << (relay - 1U));
    if (enabled) {
        outputState_ = static_cast<uint8_t>(outputState_ | bit);
    } else {
        outputState_ = static_cast<uint8_t>(outputState_ & ~bit);
    }

    if (!writeRegister(kTca9554OutputRegister, outputState_)) {
        if (logger_ != nullptr) {
            logger_->info(F("Relays: failed to write relay state"));
        }
        return false;
    }

    logRelayState(relay, enabled);
    return true;
}

bool Relays::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(kTca9554Address);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

bool Relays::isValidRelay(uint8_t relay) const {
    return relay >= 1U && relay <= kRelayCount;
}

void Relays::logRelayState(uint8_t relay, bool enabled) const {
    if (logger_ == nullptr) {
        return;
    }

    char message[32] = {};
    snprintf(
        message,
        sizeof(message),
        "Relay %u -> %s",
        relay,
        enabled ? "ON" : "OFF");
    logger_->info(message);
}

}  // namespace argos
