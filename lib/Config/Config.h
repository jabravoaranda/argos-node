#pragma once

#include <stddef.h>

namespace argos {

/**
 * Immutable boot-time firmware configuration.
 *
 * The current implementation is intentionally small. It centralizes identity
 * and timing defaults so persistent storage, provisioning, and remote
 * configuration can be added without changing module interfaces.
 */
class Config {
public:
    /** Load configuration defaults for this boot. */
    void begin();

    /** Reserved for future configuration refresh or persistence work. */
    void update();

    /** Human-readable firmware version. */
    const char* firmwareVersion() const;

    /** Human-readable board target. */
    const char* boardName() const;

    /** Serial baud rate used by the logger. */
    unsigned long serialBaudRate() const;
};

}  // namespace argos
