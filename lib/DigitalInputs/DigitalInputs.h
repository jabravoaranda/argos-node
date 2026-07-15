#pragma once

#include <Config.h>

namespace argos {

/**
 * Isolated digital input abstraction.
 *
 * The class owns stable input state storage while board-specific sampling is
 * deferred to a future driver implementation.
 */
class DigitalInputs {
public:
    /** Prepare the digital input subsystem placeholder. */
    void begin(const Config& config);

    /** Reserved for future non-blocking input sampling. */
    void update();
};

}  // namespace argos
