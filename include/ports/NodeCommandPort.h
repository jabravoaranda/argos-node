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
};

}  // namespace argos
