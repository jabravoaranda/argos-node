#pragma once

#include <Config.h>

namespace argos {

/**
 * Ethernet capability boundary.
 *
 * Hardware-specific W5500 support will live behind this interface. The first
 * architecture commit keeps the implementation inactive so the firmware
 * remains portable while the board support contract settles.
 */
class NetworkEthernet {
public:
    /** Prepare the Ethernet subsystem placeholder. */
    void begin(const Config& config);

    /** Reserved for future non-blocking Ethernet maintenance. */
    void update();
};

}  // namespace argos
