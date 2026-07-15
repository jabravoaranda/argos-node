#pragma once

#include <Arduino.h>
#include <Config.h>

namespace argos {

/**
 * Serial logger for boot diagnostics and simple runtime messages.
 */
class Logger {
public:
    /** Initialize Serial and print the startup banner. */
    void begin(const Config& config);

    /** Print an informational line. */
    void info(const __FlashStringHelper* message) const;

    /** Print an informational line. */
    void info(const char* message) const;

private:
    void printStartupBanner(const Config& config) const;
};

}  // namespace argos
