#pragma once

#include <Config.h>
#include <Logger.h>
#include <stddef.h>
#include <stdint.h>

namespace argos {

/**
 * Minimal WiFi connectivity probe.
 *
 * This module only verifies station-mode connectivity and reports network
 * details. It intentionally does not expose servers, APIs, OTA, MQTT, or
 * application networking behavior.
 */
class WiFiManager {
public:
    /** Attempt to connect to the configured WiFi network once. */
    void begin(const Config& config, const Logger& logger);

    /** Reserved for future non-blocking WiFi maintenance. */
    void update();

    /** Attempt to connect to WiFi once from an explicit user command. */
    void connectOnce();

    /** True when station WiFi is connected. */
    bool isConnected() const;

    /** Current RSSI in dBm. Valid only when connected. */
    int32_t rssiDbm() const;

    /** Copy current SSID into the caller-provided buffer. */
    void ssid(char* buffer, size_t size) const;

    /** Copy current IPv4 address into the caller-provided buffer. */
    void ipAddress(char* buffer, size_t size) const;

    /** Copy station MAC address into the caller-provided buffer. */
    void macAddress(char* buffer, size_t size) const;

private:
    static constexpr unsigned long kConnectionTimeoutMs = 30000UL;
    static constexpr unsigned long kProgressIntervalMs = 1000UL;

    void printConnectionDetails() const;

    const Logger* logger_ = nullptr;
};

}  // namespace argos
