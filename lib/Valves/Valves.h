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
 * Configured valves are physically wired to matching relay channels:
 * relay ON means valve open, relay OFF means valve closed.
 */
class Valves {
public:
    static constexpr uint8_t kValveCount = 3;
    static constexpr uint8_t kMaxValveId = 8;
    static constexpr uint8_t kValve6Id = 6;
    static constexpr uint8_t kValve6RelayId = 6;
    static constexpr uint8_t kValve7Id = 7;
    static constexpr uint8_t kValve7RelayId = 7;
    static constexpr uint8_t kValve8Id = 8;
    static constexpr uint8_t kValve8RelayId = 8;
    static constexpr uint8_t kFlowmeterValveId = kValve8Id;

    /** Attach the relay driver and force configured valves to their safe closed state. */
    void begin(const Config& config, Relays& relays, const Logger& logger);

    /** Reserved for future non-blocking valve work. */
    void update();

    bool setState(uint8_t valveId, ValveState state);
    bool open(uint8_t valveId);
    bool close(uint8_t valveId);
    bool status(uint8_t valveId, ValveStatus& status) const;
    bool statusByIndex(uint8_t index, ValveStatus& status) const;

private:
    struct ValveConfig {
        uint8_t id;
        const char* name;
        uint8_t relayId;
    };

    static constexpr ValveConfig kValveConfigs[kValveCount] = {
        {kValve6Id, "electrovalvula_6", kValve6RelayId},
        {kValve7Id, "electrovalvula_7", kValve7RelayId},
        {kValve8Id, "electrovalvula_8", kValve8RelayId},
    };

    const ValveConfig* configFor(uint8_t valveId) const;
    bool statusForConfig(const ValveConfig& valve, ValveStatus& status) const;
    static ValveState stateFromRelay(bool relayState);

    Relays* relays_ = nullptr;
    const Logger* logger_ = nullptr;
};

}  // namespace argos
