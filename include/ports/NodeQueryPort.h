#pragma once

#include <stdint.h>

namespace argos {

/**
 * Transport-independent query port for node output state.
 */
class NodeQueryPort {
public:
    virtual ~NodeQueryPort() = default;

    /** Read one relay output by 1-based relay id. */
    virtual bool relayState(uint8_t id) const = 0;
};

}  // namespace argos
