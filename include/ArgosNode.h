#pragma once

#include <Config.h>
#include <DigitalInputs.h>
#include <HttpApi.h>
#include <Logger.h>
#include <Metrics.h>
#include <NetworkEthernet.h>
#include <NodeState.h>
#include <Relays.h>
#include <WiFiManager.h>
#include <ports/NodeCommandPort.h>
#include <ports/NodeQueryPort.h>

namespace argos {

/**
 * Coordinates firmware modules for the ARGOS field controller.
 *
 * This class intentionally contains no agronomic decision logic. It only owns
 * subsystem lifecycle and dispatches deterministic updates.
 */
class ArgosNode : public NodeCommandPort, public NodeQueryPort {
public:
    /** Initialize configuration and all hardware abstraction modules. */
    void begin();

    /** Run one cooperative firmware update cycle. */
    void update();

    /** Set one relay output by 1-based relay id. */
    bool setRelay(uint8_t id, bool state) override;

    /** Read one relay output by 1-based relay id. */
    bool relayState(uint8_t id) const override;

private:
#if defined(ARGOS_ENABLE_SERIAL_DEV_COMMANDS) && ARGOS_ENABLE_SERIAL_DEV_COMMANDS
    static constexpr size_t kSerialCommandBufferSize = 16;
#endif

#if defined(ARGOS_ENABLE_RELAY_SELFTEST) && ARGOS_ENABLE_RELAY_SELFTEST
    void runBootSelfTest();
#endif
#if defined(ARGOS_ENABLE_SERIAL_DEV_COMMANDS) && ARGOS_ENABLE_SERIAL_DEV_COMMANDS
    void processSerialCommands();
    void handleSerialCommand(const char* command);
#endif

    Config config_;
    Logger logger_;
    WiFiManager wifi_;
    Metrics metrics_;
    NodeState nodeState_;
    HttpApi httpApi_;
    NetworkEthernet ethernet_;
    Relays relays_;
    DigitalInputs digitalInputs_;
#if defined(ARGOS_ENABLE_SERIAL_DEV_COMMANDS) && ARGOS_ENABLE_SERIAL_DEV_COMMANDS
    char serialCommandBuffer_[kSerialCommandBufferSize] = {};
    size_t serialCommandLength_ = 0;
#endif
};

/** Returns the single firmware application instance. */
ArgosNode& node();

}  // namespace argos
