#pragma once

#include <Arduino.h>
#include <Config.h>
#include <stdint.h>

namespace argos {

/**
 * Isolated digital input abstraction.
 *
 * DI8 is wired to GPIO11 on the Waveshare ESP32-S3-POE-ETH-8DI-8RO.
 * The public state follows the ESP32 pin level: true means the field input is
 * active/energized on the tested board wiring.
 */
class DigitalInputs {
public:
    static constexpr uint8_t kInputCount = 8;
    static constexpr float kYfDn32HzPerLMin = 0.45F;
    static constexpr float kYfDn32PulsesPerLiter = kYfDn32HzPerLMin * 60.0F;

    /** Prepare the implemented digital input pins. */
    void begin(const Config& config);

    /** Sample implemented inputs. */
    void update();

    /** Whether this 1-based digital input is implemented in firmware. */
    bool isImplemented(uint8_t input) const;

    /** Whether this 1-based digital input has a sampled state. */
    bool stateAvailable(uint8_t input) const;

    /** Read the latest sampled field state for one 1-based digital input. */
    bool isActive(uint8_t input) const;

    /** Return total DI8 flowmeter pulses counted since boot. */
    uint32_t flowmeterPulseCount() const;

    /** Return latest calculated YF-DN32 flow in liters per minute. */
    float flowmeterFlowLMin() const;

    /** Return accumulated YF-DN32 volume in liters since boot. */
    float flowmeterTotalL() const;

private:
    static constexpr uint8_t kFlowmeterInput = 8;
    static constexpr uint8_t kFlowmeterPin = 11;
    static constexpr unsigned long kFlowSampleIntervalMs = 1000UL;
    static constexpr unsigned long kMinPulseIntervalUs = 2000UL;

    static void IRAM_ATTR handleFlowmeterPulse();

    bool isValidInput(uint8_t input) const;
    uint32_t pulseCountSnapshot() const;

    bool states_[kInputCount] = {};
    bool stateAvailable_[kInputCount] = {};
    unsigned long lastFlowSampleMs_ = 0;
    uint32_t lastFlowSamplePulseCount_ = 0;
    float flowLMin_ = 0.0F;

    volatile uint32_t flowmeterPulseCount_ = 0;
    volatile unsigned long lastPulseUs_ = 0;
    static DigitalInputs* instance_;
};

}  // namespace argos
