#pragma once

#include <Arduino.h>
#include <Config.h>
#include <Logger.h>
#include <Relays.h>
#include <stddef.h>
#include <stdint.h>

namespace argos {

enum class ValveState : uint8_t {
    Closed = 0,
    Open = 1,
};

struct ValveStatus {
    uint8_t id = 0;
    const char* name = "";
    uint8_t relayId = 0;
    ValveState state = ValveState::Closed;
};

/**
 * Semantic valve controller.
 *
 * Valve 1 is physically wired to relay CH1:
 * relay ON means valve open, relay OFF means valve closed.
 */
class Valves {
public:
    static constexpr uint8_t kValveCount = 1;
    static constexpr uint8_t kValve1Id = 1;
    static constexpr uint8_t kValve1RelayId = 1;

    /** Attach the relay driver and force configured valves to their safe closed state. */
    void begin(const Config& config, Relays& relays, const Logger& logger);

    /** Reserved for future non-blocking valve work. */
    void update();

    bool setState(uint8_t valveId, ValveState state);
    bool open(uint8_t valveId);
    bool close(uint8_t valveId);
    bool status(uint8_t valveId, ValveStatus& status) const;
    uint8_t count() const;

private:
    struct ValveConfig {
        uint8_t id;
        const char* name;
        uint8_t relayId;
    };

    static constexpr ValveConfig kValveConfigs[kValveCount] = {
        {kValve1Id, "electrovalvula_1", kValve1RelayId},
    };

    const ValveConfig* configFor(uint8_t valveId) const;
    static ValveState stateFromRelay(bool relayState);

    Relays* relays_ = nullptr;
    const Logger* logger_ = nullptr;
};

}  // namespace argos
