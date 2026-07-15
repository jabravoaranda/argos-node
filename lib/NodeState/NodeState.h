#pragma once

#include <Config.h>
#include <Metrics.h>
#include <Relays.h>
#include <WiFiManager.h>
#include <stdint.h>

namespace argos {

struct NodeHealth {
    bool ok = true;
};

struct CapabilityStatus {
    uint8_t available = 0;
    bool implemented = false;
};

struct BooleanCapabilityStatus {
    bool available = false;
    bool implemented = false;
};

struct NetworkInfo {
    char mac[18] = {};
    char ip[16] = {};
};

struct NodeInfo {
    const char* name = "argos-node";
    const char* firmwareVersion = "";
    const char* buildDate = "";
    const char* board = "";
    NetworkInfo network;
    CapabilityStatus relays;
    CapabilityStatus digitalInputs;
    CapabilityStatus flowmeters;
    CapabilityStatus analogInputs;
    BooleanCapabilityStatus wifi;
    BooleanCapabilityStatus ethernet;
    BooleanCapabilityStatus rs485;
};

struct WifiState {
    bool connected = false;
    char ssid[33] = {};
    char ip[16] = {};
    bool rssiAvailable = false;
    int32_t rssiDbm = 0;
    char mac[18] = {};
};

struct RelayState {
    uint8_t id = 0;
    bool state = false;
};

struct DigitalInputState {
    uint8_t id = 0;
    bool implemented = false;
    bool stateAvailable = false;
    bool state = false;
};

struct FlowmeterState {
    bool implemented = false;
    bool pulseCountAvailable = false;
    uint32_t pulseCount = 0;
    bool flowLMinAvailable = false;
    float flowLMin = 0.0F;
    bool totalLAvailable = false;
    float totalL = 0.0F;
};

struct NodeStatus {
    const char* name = "argos-node";
    const char* firmwareVersion = "";
    uint32_t uptimeS = 0;
    WifiState network;
    RelayState relays[8] = {};
    DigitalInputState digitalInputs[8] = {};
    FlowmeterState flowmeter;
    uint32_t freeHeapBytes = 0;
    uint32_t minimumFreeHeapBytes = 0;
};

struct NodeMetrics {
    const char* firmwareVersion = "";
    const char* buildDate = "";
    SystemTelemetry system;
    WifiState wifi;
};

/**
 * Transport-independent ARGOS Node state aggregator.
 */
class NodeState {
public:
    /** Attach state sources owned by hardware and service modules. */
    void begin(const Config& config, const Relays& relays, const WiFiManager& wifi, const Metrics& metrics);

    NodeHealth health() const;
    NodeInfo info() const;
    NodeStatus status() const;
    NodeMetrics metrics() const;

private:
    static constexpr uint8_t kRelayCount = 8;
    static constexpr uint8_t kDigitalInputCount = 8;

    WifiState wifiState() const;
    const char* buildDate() const;

    const Config* config_ = nullptr;
    const Relays* relays_ = nullptr;
    const WiFiManager* wifi_ = nullptr;
    const Metrics* metrics_ = nullptr;
};

}  // namespace argos
