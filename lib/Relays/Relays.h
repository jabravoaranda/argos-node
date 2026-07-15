#pragma once

#include <Arduino.h>
#include <Config.h>
#include <Logger.h>
#include <stdint.h>

namespace argos {

/**
 * Relay output abstraction.
 *
 * Hardware-specific TCA9554 access for the Waveshare
 * ESP32-S3-POE-ETH-8DI-8RO is isolated in this module.
 */
class Relays {
public:
    /** Initialize the TCA9554 relay expander and force every relay OFF. */
    void begin(const Config& config, const Logger& logger);

    /** Reserved for future non-blocking relay output work. */
    void update();

#if (defined(ARGOS_ENABLE_RELAY_SELFTEST) && ARGOS_ENABLE_RELAY_SELFTEST) || \
    (defined(ARGOS_ENABLE_SERIAL_DEV_COMMANDS) && ARGOS_ENABLE_SERIAL_DEV_COMMANDS)
    /** Development self-test: turn one relay ON for three seconds, then OFF. */
    bool testRelay(uint8_t relay);
#endif

    /** Turn every relay OFF immediately. */
    bool allOff();

    /** Set one relay output by 1-based relay number. */
    bool setRelay(uint8_t relay, bool enabled);

    /** Read desired relay output state by 1-based relay number. */
    bool isOn(uint8_t relay) const;

private:
    static constexpr uint8_t kI2cSdaPin = 42;
    static constexpr uint8_t kI2cSclPin = 41;
    static constexpr uint8_t kTca9554Address = 0x20;
    static constexpr uint8_t kTca9554OutputRegister = 0x01;
    static constexpr uint8_t kTca9554ConfigRegister = 0x03;
    static constexpr uint8_t kRelayCount = 8;
    static constexpr uint8_t kAllRelaysOff = 0x00;
    static constexpr uint8_t kAllPinsOutputMode = 0x00;

    bool writeRegister(uint8_t reg, uint8_t value);
    bool isValidRelay(uint8_t relay) const;
    void logRelayState(uint8_t relay, bool enabled) const;

    const Logger* logger_ = nullptr;
    uint8_t outputState_ = kAllRelaysOff;
};

}  // namespace argos
