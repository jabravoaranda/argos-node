#include "NodeState.h"

#include <stdio.h>

namespace argos {

void NodeState::begin(const Config& config, const Relays& relays, const Valves& valves, const WiFiManager& wifi, const Metrics& metrics) {
    config_ = &config;
    relays_ = &relays;
    valves_ = &valves;
    wifi_ = &wifi;
    metrics_ = &metrics;
}

NodeHealth NodeState::health() const {
    return {};
}

NodeInfo NodeState::info() const {
    NodeInfo info;
    info.firmwareVersion = config_->firmwareVersion();
    info.buildDate = buildDate();
    info.board = config_->boardName();
    wifi_->macAddress(info.network.mac, sizeof(info.network.mac));
    wifi_->ipAddress(info.network.ip, sizeof(info.network.ip));
    info.relays.available = 8;
    info.relays.implemented = true;
    info.valves.available = Valves::kValveCount;
    info.valves.implemented = true;
    info.digitalInputs.available = 8;
    info.digitalInputs.implemented = false;
    info.flowmeters.available = 0;
    info.flowmeters.implemented = false;
    info.analogInputs.available = 0;
    info.analogInputs.implemented = false;
    info.wifi.available = true;
    info.wifi.implemented = true;
    info.ethernet.available = true;
    info.ethernet.implemented = false;
    info.rs485.available = true;
    info.rs485.implemented = false;
    return info;
}

NodeStatus NodeState::status() const {
    NodeStatus status;
    const SystemTelemetry telemetry = metrics_->snapshot();
    status.firmwareVersion = config_->firmwareVersion();
    status.uptimeS = telemetry.uptimeS;
    status.network = wifiState();
    status.freeHeapBytes = telemetry.heapFreeBytes;
    status.minimumFreeHeapBytes = telemetry.heapMinimumBytes;

    for (uint8_t relay = 1; relay <= kRelayCount; ++relay) {
        status.relays[relay - 1].id = relay;
        status.relays[relay - 1].state = relays_->isOn(relay);
    }

    for (uint8_t valve = 1; valve <= Valves::kValveCount; ++valve) {
        valves_->status(valve, status.valves[valve - 1]);
    }

    for (uint8_t input = 1; input <= kDigitalInputCount; ++input) {
        status.digitalInputs[input - 1].id = input;
        status.digitalInputs[input - 1].implemented = false;
        status.digitalInputs[input - 1].stateAvailable = false;
    }

    return status;
}

NodeMetrics NodeState::metrics() const {
    NodeMetrics nodeMetrics;
    nodeMetrics.firmwareVersion = config_->firmwareVersion();
    nodeMetrics.buildDate = buildDate();
    nodeMetrics.system = metrics_->snapshot();
    nodeMetrics.wifi = wifiState();
    return nodeMetrics;
}

WifiState NodeState::wifiState() const {
    WifiState state;
    state.connected = wifi_->isConnected();
    wifi_->ssid(state.ssid, sizeof(state.ssid));
    wifi_->ipAddress(state.ip, sizeof(state.ip));
    wifi_->macAddress(state.mac, sizeof(state.mac));
    if (state.connected) {
        state.rssiAvailable = true;
        state.rssiDbm = wifi_->rssiDbm();
    }
    return state;
}

const char* NodeState::buildDate() const {
    return __DATE__ " " __TIME__;
}

}  // namespace argos
