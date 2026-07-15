#pragma once

#include <stdint.h>

namespace argos {

struct SystemTelemetry {
    uint32_t uptimeS = 0;
    bool bootCountAvailable = false;
    uint32_t bootCount = 0;
    uint32_t cpuFrequencyMhz = 0;
    uint32_t heapFreeBytes = 0;
    uint32_t heapMinimumBytes = 0;
    uint32_t heapLargestBlockBytes = 0;
    uint32_t flashSizeBytes = 0;
    uint32_t flashUsedBytes = 0;
    bool mainLoopHzAvailable = false;
    uint32_t mainLoopHz = 0;
};

/**
 * Transport-independent system telemetry provider.
 */
class Metrics {
public:
    /** Prepare system telemetry collection. */
    void begin();

    /** Reserved for future non-blocking metric sampling. */
    void update();

    /** Return the current typed system telemetry snapshot. */
    SystemTelemetry snapshot() const;
};

}  // namespace argos
