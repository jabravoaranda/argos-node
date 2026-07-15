#include "ArgosNode.h"

#include <Arduino.h>

namespace argos {

void ArgosNode::begin() {
    config_.begin();
    logger_.begin(config_);
    wifi_.begin(config_, logger_);
    metrics_.begin();
    ethernet_.begin(config_);
    relays_.begin(config_, logger_);
    nodeState_.begin(config_, relays_, wifi_, metrics_);
    httpApi_.begin(logger_, nodeState_, *this, *this);
    digitalInputs_.begin(config_);
#if defined(ARGOS_ENABLE_RELAY_SELFTEST) && ARGOS_ENABLE_RELAY_SELFTEST
    runBootSelfTest();
#endif
}

void ArgosNode::update() {
#if defined(ARGOS_ENABLE_SERIAL_DEV_COMMANDS) && ARGOS_ENABLE_SERIAL_DEV_COMMANDS
    processSerialCommands();
#endif
    config_.update();
    wifi_.update();
    metrics_.update();
    httpApi_.update();
    ethernet_.update();
    relays_.update();
    digitalInputs_.update();

    delay(1);
}

bool ArgosNode::setRelay(uint8_t id, bool state) {
    return relays_.setRelay(id, state);
}

bool ArgosNode::relayState(uint8_t id) const {
    return relays_.isOn(id);
}

#if defined(ARGOS_ENABLE_RELAY_SELFTEST) && ARGOS_ENABLE_RELAY_SELFTEST
void ArgosNode::runBootSelfTest() {
    delay(3000);
    relays_.testRelay(1);
}
#endif

#if defined(ARGOS_ENABLE_SERIAL_DEV_COMMANDS) && ARGOS_ENABLE_SERIAL_DEV_COMMANDS
void ArgosNode::processSerialCommands() {
    while (Serial.available() > 0) {
        const char input = static_cast<char>(Serial.read());

        if (input == '\r') {
            continue;
        }

        if (input == '\n') {
            serialCommandBuffer_[serialCommandLength_] = '\0';
            handleSerialCommand(serialCommandBuffer_);
            serialCommandLength_ = 0;
            serialCommandBuffer_[0] = '\0';
            continue;
        }

        if (serialCommandLength_ < kSerialCommandBufferSize - 1U) {
            serialCommandBuffer_[serialCommandLength_] = input;
            ++serialCommandLength_;
        } else {
            serialCommandLength_ = 0;
            serialCommandBuffer_[0] = '\0';
            logger_.info(F("Serial command too long"));
        }
    }
}

void ArgosNode::handleSerialCommand(const char* command) {
    if (command[0] == '\0') {
        return;
    }

    if (command[0] == 't' && command[1] >= '1' && command[1] <= '8' && command[2] == '\0') {
        relays_.testRelay(static_cast<uint8_t>(command[1] - '0'));
        return;
    }

    if (command[0] == 'o' && command[1] == 'f' && command[2] == 'f' && command[3] == '\0') {
        relays_.allOff();
        return;
    }

    if (command[0] == 'w' && command[1] == 'i' && command[2] == 'f' && command[3] == 'i' && command[4] == '\0') {
        wifi_.connectOnce();
        return;
    }

    logger_.info(F("Unknown command. Use t1..t8, off, or wifi"));
}
#endif

ArgosNode& node() {
    static ArgosNode instance;
    return instance;
}

}  // namespace argos
