#include "JsonSerializer.h"

namespace argos {

String JsonSerializer::health(const NodeHealth& health) {
    return health.ok ? F("{\"status\":\"ok\"}") : F("{\"status\":\"error\"}");
}

String JsonSerializer::info(const NodeInfo& info) {
    String body = F("{\"node\":{\"name\":\"");
    body += info.name;
    body += F("\",\"firmware_version\":\"");
    body += info.firmwareVersion;
    body += F("\",\"build_date\":\"");
    body += info.buildDate;
    body += F("\",\"board\":\"");
    body += info.board;
    body += F("\"},\"network\":{\"mac\":\"");
    body += info.network.mac;
    body += F("\",\"ip\":\"");
    body += info.network.ip;
    body += F("\"},\"capabilities\":{\"relays\":{\"available\":");
    body += info.relays.available;
    body += F(",\"implemented\":");
    body += info.relays.implemented ? F("true") : F("false");
    body += F("},\"valves\":{\"available\":");
    body += info.valves.available;
    body += F(",\"implemented\":");
    body += info.valves.implemented ? F("true") : F("false");
    body += F("},\"digital_inputs\":{\"available\":");
    body += info.digitalInputs.available;
    body += F(",\"implemented\":");
    body += info.digitalInputs.implemented ? F("true") : F("false");
    body += F("},\"flowmeters\":{\"available\":");
    body += info.flowmeters.available;
    body += F(",\"implemented\":");
    body += info.flowmeters.implemented ? F("true") : F("false");
    body += F("},\"analog_inputs\":{\"available\":");
    body += info.analogInputs.available;
    body += F(",\"implemented\":");
    body += info.analogInputs.implemented ? F("true") : F("false");
    body += F("},\"wifi\":{\"available\":");
    body += info.wifi.available ? F("true") : F("false");
    body += F(",\"implemented\":");
    body += info.wifi.implemented ? F("true") : F("false");
    body += F("},\"ethernet\":{\"available\":");
    body += info.ethernet.available ? F("true") : F("false");
    body += F(",\"implemented\":");
    body += info.ethernet.implemented ? F("true") : F("false");
    body += F("},\"rs485\":{\"available\":");
    body += info.rs485.available ? F("true") : F("false");
    body += F(",\"implemented\":");
    body += info.rs485.implemented ? F("true") : F("false");
    body += F("}}}");
    return body;
}

String JsonSerializer::status(const NodeStatus& status) {
    String body = F("{\"node\":{\"name\":\"");
    body += status.name;
    body += F("\",\"firmware_version\":\"");
    body += status.firmwareVersion;
    body += F("\",\"uptime_s\":");
    body += status.uptimeS;
    body += F("},\"network\":{\"wifi_connected\":");
    body += status.network.connected ? F("true") : F("false");
    body += F(",\"ssid\":\"");
    body += status.network.ssid;
    body += F("\",\"ip\":\"");
    body += status.network.ip;
    body += F("\",\"rssi_dbm\":");
    body += status.network.rssiAvailable ? String(status.network.rssiDbm) : F("null");
    body += F(",\"mac\":\"");
    body += status.network.mac;
    body += F("\"},\"outputs\":{\"relays\":");
    appendRelayStates(body, status.relays, 8);
    body += F("},\"valves\":");
    appendValveStates(body, status.valves, Valves::kValveCount, false);
    body += F(",\"inputs\":{\"digital\":");
    appendDigitalInputs(body, status.digitalInputs, 8);
    body += F("},\"flowmeter\":{\"implemented\":false,\"pulse_count\":null,\"flow_l_min\":null,\"total_l\":null},");
    body += F("\"system\":{\"free_heap_bytes\":");
    body += status.freeHeapBytes;
    body += F(",\"minimum_free_heap_bytes\":");
    body += status.minimumFreeHeapBytes;
    body += F("}}");
    return body;
}

String JsonSerializer::metrics(const NodeMetrics& metrics) {
    String body = F("{\"system\":{\"uptime_s\":");
    body += metrics.system.uptimeS;
    body += F(",\"boot_count\":null,\"firmware_version\":\"");
    body += metrics.firmwareVersion;
    body += F("\",\"build_date\":\"");
    body += metrics.buildDate;
    body += F("\",\"cpu_frequency_mhz\":");
    body += metrics.system.cpuFrequencyMhz;
    body += F("},\"memory\":{\"heap_free_bytes\":");
    body += metrics.system.heapFreeBytes;
    body += F(",\"heap_minimum_bytes\":");
    body += metrics.system.heapMinimumBytes;
    body += F(",\"heap_largest_block_bytes\":");
    body += metrics.system.heapLargestBlockBytes;
    body += F("},\"wifi\":{\"connected\":");
    body += metrics.wifi.connected ? F("true") : F("false");
    body += F(",\"rssi_dbm\":");
    body += metrics.wifi.rssiAvailable ? String(metrics.wifi.rssiDbm) : F("null");
    body += F("},\"network\":{\"ip\":\"");
    body += metrics.wifi.ip;
    body += F("\",\"mac\":\"");
    body += metrics.wifi.mac;
    body += F("\"},\"flash\":{\"size_bytes\":");
    body += metrics.system.flashSizeBytes;
    body += F(",\"used_bytes\":");
    body += metrics.system.flashUsedBytes;
    body += F("},\"tasks\":{\"main_loop_hz\":null}}");
    return body;
}

String JsonSerializer::outputs(const NodeStatus& status) {
    String body = F("{\"relays\":");
    appendRelayStates(body, status.relays, 8);
    body += F("}");
    return body;
}

String JsonSerializer::valves(const NodeStatus& status) {
    String body = F("{\"valves\":");
    appendValveStates(body, status.valves, Valves::kValveCount, true);
    body += F("}");
    return body;
}

String JsonSerializer::valve(const ValveStatus& valve) {
    String body = F("{\"id\":");
    body += valve.id;
    body += F(",\"name\":\"");
    body += valve.name;
    body += F("\",\"relay_id\":");
    body += valve.relayId;
    body += F(",\"state\":\"");
    body += valveStateText(valve.state);
    body += F("\"}");
    return body;
}

String JsonSerializer::relayCommand(uint8_t relay, bool state) {
    String body = F("{\"relay\":");
    body += relay;
    body += F(",\"state\":");
    body += state ? F("true") : F("false");
    body += F(",\"result\":\"ok\"}");
    return body;
}

void JsonSerializer::appendRelayStates(String& body, const RelayState* relays, uint8_t count) {
    body += F("[");
    for (uint8_t i = 0; i < count; ++i) {
        if (i > 0) {
            body += F(",");
        }
        body += F("{\"id\":");
        body += relays[i].id;
        body += F(",\"state\":");
        body += relays[i].state ? F("true") : F("false");
        body += F("}");
    }
    body += F("]");
}

void JsonSerializer::appendValveStates(String& body, const ValveStatus* valves, uint8_t count, bool includeRelayId) {
    body += F("[");
    for (uint8_t i = 0; i < count; ++i) {
        if (i > 0) {
            body += F(",");
        }
        body += F("{\"id\":");
        body += valves[i].id;
        body += F(",\"name\":\"");
        body += valves[i].name;
        body += F("\"");
        if (includeRelayId) {
            body += F(",\"relay_id\":");
            body += valves[i].relayId;
        }
        body += F(",\"state\":\"");
        body += valveStateText(valves[i].state);
        body += F("\"}");
    }
    body += F("]");
}

void JsonSerializer::appendDigitalInputs(String& body, const DigitalInputState* inputs, uint8_t count) {
    body += F("[");
    for (uint8_t i = 0; i < count; ++i) {
        if (i > 0) {
            body += F(",");
        }
        body += F("{\"id\":");
        body += inputs[i].id;
        body += F(",\"state\":");
        if (inputs[i].stateAvailable) {
            body += inputs[i].state ? F("true") : F("false");
        } else {
            body += F("null");
        }
        body += F(",\"implemented\":");
        body += inputs[i].implemented ? F("true") : F("false");
        body += F("}");
    }
    body += F("]");
}

const __FlashStringHelper* JsonSerializer::valveStateText(ValveState state) {
    return state == ValveState::Open ? F("open") : F("closed");
}

}  // namespace argos
