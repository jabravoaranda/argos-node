#pragma once

#include <NodeState.h>
#include <WString.h>

namespace argos {

/**
 * JSON serialization for transport-independent node state structures.
 */
class JsonSerializer {
public:
    static String health(const NodeHealth& health);
    static String info(const NodeInfo& info);
    static String status(const NodeStatus& status);
    static String metrics(const NodeMetrics& metrics);
    static String outputs(const NodeStatus& status);
    static String relayCommand(uint8_t relay, bool state);

private:
    static void appendRelayStates(String& body, const RelayState* relays, uint8_t count);
    static void appendDigitalInputs(String& body, const DigitalInputState* inputs, uint8_t count);
};

}  // namespace argos
