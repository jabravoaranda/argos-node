#include "WiFiManager.h"

#include <Arduino.h>
#include <WiFi.h>
#include <stdio.h>

#if __has_include("../../include/WiFiCredentials.h")
#include "../../include/WiFiCredentials.h"
#else
#include "../../include/WiFiCredentials.example.h"
#endif

namespace argos {

void WiFiManager::begin(const Config& config, const Logger& logger) {
    (void)config;
    logger_ = &logger;

    connectOnce();
}

void WiFiManager::update() {}

void WiFiManager::connectOnce() {
    if (logger_ == nullptr) {
        return;
    }

    logger_->info(F("WiFi: starting station mode"));
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    const unsigned long startedAt = millis();
    unsigned long lastProgressAt = 0;

    while (WiFi.status() != WL_CONNECTED && millis() - startedAt < kConnectionTimeoutMs) {
        const unsigned long now = millis();
        if (lastProgressAt == 0 || now - lastProgressAt >= kProgressIntervalMs) {
            logger_->info(F("WiFi: connecting..."));
            lastProgressAt = now;
        }
        delay(10);
    }

    if (WiFi.status() == WL_CONNECTED) {
        printConnectionDetails();
        return;
    }

    logger_->info(F("WiFi connection failed"));
}

void WiFiManager::printConnectionDetails() const {
    if (logger_ == nullptr) {
        return;
    }

    Serial.println(F("------------------------------------"));
    Serial.println(F("WiFi connected"));
    Serial.print(F("SSID: "));
    Serial.println(WiFi.SSID());
    Serial.print(F("RSSI: "));
    Serial.println(WiFi.RSSI());
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("Gateway: "));
    Serial.println(WiFi.gatewayIP());
    Serial.print(F("Subnet mask: "));
    Serial.println(WiFi.subnetMask());
    Serial.print(F("MAC address: "));
    Serial.println(WiFi.macAddress());
    Serial.println(F("------------------------------------"));
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

int32_t WiFiManager::rssiDbm() const {
    return WiFi.RSSI();
}

void WiFiManager::ssid(char* buffer, size_t size) const {
    if (buffer == nullptr || size == 0) {
        return;
    }

    snprintf(buffer, size, "%s", WiFi.SSID().c_str());
}

void WiFiManager::ipAddress(char* buffer, size_t size) const {
    if (buffer == nullptr || size == 0) {
        return;
    }

    snprintf(buffer, size, "%s", WiFi.localIP().toString().c_str());
}

void WiFiManager::macAddress(char* buffer, size_t size) const {
    if (buffer == nullptr || size == 0) {
        return;
    }

    snprintf(buffer, size, "%s", WiFi.macAddress().c_str());
}

}  // namespace argos
