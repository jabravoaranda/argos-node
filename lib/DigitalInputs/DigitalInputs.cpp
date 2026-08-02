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

uint32_t DigitalInputs::flowmeterResettablePulseCount() const {
    return pulseCountSnapshot() - totalResetPulseCount_;
}

uint32_t DigitalInputs::flowmeterHydrologicalYearPulseCount() const {
    return pulseCountSnapshot() - hydrologicalYearResetPulseCount_;
}

uint32_t DigitalInputs::flowmeterSessionPulseCount() const {
    if (!sessionActive_) {
        return lastSessionPulseCount_;
    }

    return pulseCountSnapshot() - sessionStartPulseCount_;
}

float DigitalInputs::flowmeterFlowLMin() const {
    return flowLMin_;
}

float DigitalInputs::flowmeterBootTotalL() const {
    return pulsesToLiters(pulseCountSnapshot());
}

float DigitalInputs::flowmeterTotalL() const {
    return pulsesToLiters(flowmeterResettablePulseCount());
}

float DigitalInputs::flowmeterHydrologicalYearL() const {
    return pulsesToLiters(flowmeterHydrologicalYearPulseCount());
}

float DigitalInputs::flowmeterSessionL() const {
    return pulsesToLiters(flowmeterSessionPulseCount());
}

float DigitalInputs::flowmeterLastSessionL() const {
    return pulsesToLiters(lastSessionPulseCount_);
}

bool DigitalInputs::flowmeterSessionActive() const {
    return sessionActive_;
}

void DigitalInputs::startFlowmeterSession() {
    sessionStartPulseCount_ = pulseCountSnapshot();
    lastSessionPulseCount_ = 0;
    sessionActive_ = true;
}

void DigitalInputs::stopFlowmeterSession() {
    if (!sessionActive_) {
        return;
    }

    lastSessionPulseCount_ = pulseCountSnapshot() - sessionStartPulseCount_;
    sessionActive_ = false;
}

void DigitalInputs::resetFlowmeterSession() {
    sessionStartPulseCount_ = pulseCountSnapshot();
    lastSessionPulseCount_ = 0;
}

void DigitalInputs::resetFlowmeterTotal() {
    totalResetPulseCount_ = pulseCountSnapshot();
}

void DigitalInputs::resetFlowmeterHydrologicalYear() {
    hydrologicalYearResetPulseCount_ = pulseCountSnapshot();
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

float DigitalInputs::pulsesToLiters(uint32_t pulses) {
    return static_cast<float>(pulses) / kYfDn32PulsesPerLiter;
}

}  // namespace argos
