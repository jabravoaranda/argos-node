#pragma once

#include <stdint.h>

namespace argos {

/**
 * Transport-independent command port for node output mutations.
 */
class NodeCommandPort {
public:
    virtual ~NodeCommandPort() = default;

    /** Set one relay output by 1-based relay id. */
    virtual bool setRelay(uint8_t id, bool state) = 0;

    /** Start a new flowmeter valve session. */
    virtual void startFlowmeterSession() = 0;

    /** Stop the active flowmeter valve session. */
    virtual void stopFlowmeterSession() = 0;

    /** Reset the current and last flowmeter session counters. */
    virtual void resetFlowmeterSession() = 0;

    /** Reset the resettable flowmeter total counter. */
    virtual void resetFlowmeterTotal() = 0;

    /** Reset the hydrological-year flowmeter counter. */
    virtual void resetFlowmeterHydrologicalYear() = 0;
};

}  // namespace argos
