#include "Valves.h"

#include <stdio.h>

namespace argos {

void Valves::begin(const Config& config, Relays& relays, const Logger& logger) {
    (void)config;
    relays_ = &relays;
    logger_ = &logger;

    for (uint8_t i = 0; i < kValveCount; ++i) {
        if (!relays_->setRelay(kValveConfigs[i].relayId, false) && logger_ != nullptr) {
            logger_->info(F("Valves: failed to force valve closed"));
        }
    }

    if (logger_ != nullptr) {
        logger_->info(F("Valve subsystem initialized"));
    }
}

void Valves::update() {}

bool Valves::setState(uint8_t valveId, ValveState state) {
    const ValveConfig* valve = configFor(valveId);
    if (valve == nullptr || relays_ == nullptr) {
        return false;
    }

    const bool relayEnabled = state == ValveState::Open;
    return relays_->setRelay(valve->relayId, relayEnabled);
}

bool Valves::open(uint8_t valveId) {
    return setState(valveId, ValveState::Open);
}

bool Valves::close(uint8_t valveId) {
    return setState(valveId, ValveState::Closed);
}

bool Valves::status(uint8_t valveId, ValveStatus& status) const {
    const ValveConfig* valve = configFor(valveId);
    if (valve == nullptr || relays_ == nullptr) {
        return false;
    }

    status.id = valve->id;
    status.name = valve->name;
    status.relayId = valve->relayId;
    status.state = stateFromRelay(relays_->isOn(valve->relayId));
    return true;
}

uint8_t Valves::count() const {
    return kValveCount;
}

const Valves::ValveConfig* Valves::configFor(uint8_t valveId) const {
    for (uint8_t i = 0; i < kValveCount; ++i) {
        if (kValveConfigs[i].id == valveId) {
            return &kValveConfigs[i];
        }
    }
    return nullptr;
}

ValveState Valves::stateFromRelay(bool relayState) {
    return relayState ? ValveState::Open : ValveState::Closed;
}

}  // namespace argos
