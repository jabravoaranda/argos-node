#include "DigitalInputs.h"

namespace argos {

DigitalInputs* DigitalInputs::instance_ = nullptr;

void DigitalInputs::begin(const Config& config) {
    (void)config;
    instance_ = this;
    pinMode(kFlowmeterPin, INPUT_PULLUP);
    lastFlowSampleMs_ = millis();
    lastFlowSamplePulseCount_ = pulseCountSnapshot();
    attachInterrupt(digitalPinToInterrupt(kFlowmeterPin), handleFlowmeterPulse, RISING);
    update();
}

void DigitalInputs::update() {
    states_[kFlowmeterInput - 1U] = digitalRead(kFlowmeterPin) == HIGH;
    stateAvailable_[kFlowmeterInput - 1U] = true;

    const unsigned long now = millis();
    if (now - lastFlowSampleMs_ < kFlowSampleIntervalMs) {
        return;
    }

    const uint32_t pulseCount = pulseCountSnapshot();
    const uint32_t pulseDelta = pulseCount - lastFlowSamplePulseCount_;
    const unsigned long elapsedMs = now - lastFlowSampleMs_;
    const float frequencyHz = (static_cast<float>(pulseDelta) * 1000.0F) / static_cast<float>(elapsedMs);
    flowLMin_ = frequencyHz / kYfDn32HzPerLMin;
    lastFlowSampleMs_ = now;
    lastFlowSamplePulseCount_ = pulseCount;
}

bool DigitalInputs::isImplemented(uint8_t input) const {
    return input == kFlowmeterInput;
}

bool DigitalInputs::stateAvailable(uint8_t input) const {
    if (!isValidInput(input)) {
        return false;
    }

    return stateAvailable_[input - 1U];
}

bool DigitalInputs::isActive(uint8_t input) const {
    if (!isValidInput(input)) {
        return false;
    }

    return states_[input - 1U];
}

uint32_t DigitalInputs::flowmeterPulseCount() const {
    return pulseCountSnapshot();
}

float DigitalInputs::flowmeterFlowLMin() const {
    return flowLMin_;
}

float DigitalInputs::flowmeterTotalL() const {
    return static_cast<float>(pulseCountSnapshot()) / kYfDn32PulsesPerLiter;
}

void IRAM_ATTR DigitalInputs::handleFlowmeterPulse() {
    if (instance_ == nullptr) {
        return;
    }

    const unsigned long now = micros();
    if (now - instance_->lastPulseUs_ < kMinPulseIntervalUs) {
        return;
    }

    instance_->lastPulseUs_ = now;
    ++instance_->flowmeterPulseCount_;
}

bool DigitalInputs::isValidInput(uint8_t input) const {
    return input >= 1U && input <= kInputCount;
}

uint32_t DigitalInputs::pulseCountSnapshot() const {
    noInterrupts();
    const uint32_t pulseCount = flowmeterPulseCount_;
    interrupts();
    return pulseCount;
}

}  // namespace argos
