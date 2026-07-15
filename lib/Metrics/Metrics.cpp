#include "Metrics.h"

#include <Arduino.h>
#include <Esp.h>

namespace argos {

void Metrics::begin() {}

void Metrics::update() {}

SystemTelemetry Metrics::snapshot() const {
    SystemTelemetry telemetry;
    telemetry.uptimeS = millis() / 1000UL;
    telemetry.cpuFrequencyMhz = ESP.getCpuFreqMHz();
    telemetry.heapFreeBytes = ESP.getFreeHeap();
    telemetry.heapMinimumBytes = ESP.getMinFreeHeap();
    telemetry.heapLargestBlockBytes = ESP.getMaxAllocHeap();
    telemetry.flashSizeBytes = ESP.getFlashChipSize();
    telemetry.flashUsedBytes = ESP.getSketchSize();
    return telemetry;
}

}  // namespace argos
