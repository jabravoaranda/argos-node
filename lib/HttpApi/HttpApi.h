#pragma once

#include <Logger.h>
#include <NodeState.h>
#include <WebServer.h>
#include <ports/NodeCommandPort.h>
#include <ports/NodeQueryPort.h>

namespace argos {

/**
 * HTTP API transport adapter for ARGOS Node.
 *
 * The API exposes status, telemetry, and development output commands. It
 * intentionally does not implement authentication, OTA, MQTT, WebSockets,
 * dashboards, irrigation logic, or ARGOS Core integration.
 */
class HttpApi {
public:
    /** Start the HTTP API and register routes. */
    void begin(const Logger& logger, const NodeState& nodeState, NodeCommandPort& commandPort, const NodeQueryPort& queryPort);

    /** Process pending HTTP clients without blocking the firmware loop. */
    void update();

private:
    static constexpr uint16_t kHttpPort = 80;
    void registerRoutes();
    void registerReadRoutes();
    void registerWriteRoutes();
    void handleHealth();
    void handleInfo();
    void handleStatus();
    void handleMetrics();
    void handleOutputs();
    void handleSetRelay();
    void handleNotFound();
    void sendJson(int statusCode, const String& body);
    void sendError(int statusCode, const __FlashStringHelper* error);
    bool parseRelayId(const String& text, uint8_t& relay) const;
    bool parseRelayStateBody(const String& body, bool& state, String& error) const;
    void logWriteClient(uint8_t relay, bool state);

    WebServer server_{kHttpPort};
    const Logger* logger_ = nullptr;
    const NodeState* nodeState_ = nullptr;
    NodeCommandPort* commandPort_ = nullptr;
    const NodeQueryPort* queryPort_ = nullptr;
    bool started_ = false;
};

}  // namespace argos
