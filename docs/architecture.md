# ARGOS Node Firmware Architecture

ARGOS Node is the field-controller firmware for the Agricultural Remote Guidance
and Observation System. It exposes hardware capabilities, reports local state,
and provides safe direct output control. It does not contain agronomic
decision-making, scheduling, automation, or irrigation logic.

## Boundaries

- Hardware access stays inside the corresponding hardware/service module.
- `ArgosNode` is the application coordinator.
- `NodeState` aggregates transport-independent state.
- `HttpApi` is an HTTP transport adapter.
- JSON serialization is isolated in `HttpApi/JsonSerializer`.
- ARGOS Core owns high-level decisions.

## Modules

- `Config`: firmware version, board identity, and fixed configuration values.
- `Logger`: serial logger and boot banner.
- `WiFiManager`: WiFi connection and read-only WiFi/network accessors.
- `Relays`: TCA9554 relay driver and relay state owner.
- `Valves`: semantic valve controller that maps configured valves to physical relays.
- `Metrics`: transport-independent ESP32 runtime telemetry provider.
- `NodeState`: typed aggregation of health, info, status, and metrics.
- `HttpApi`: HTTP routes, request validation, and response transport.
- `DigitalInputs`: placeholder module; input reading is not implemented yet.
- `Ethernet`: placeholder module; W5500 support is not implemented yet.

Each module exposes `begin()` and, where useful, `update()`.

## Application Ports

HTTP depends on transport-independent application ports:

- `NodeCommandPort`: write commands such as `setRelay`.
- `NodeQueryPort`: read queries such as `relayState`.

`ArgosNode` implements both ports. This keeps HTTP decoupled from the concrete
application class and leaves room for future transports such as MQTT, WebSocket,
or serial command adapters.

## Runtime Flow

`src/main.cpp` delegates to `ArgosNode`.

Startup order:

1. `Config`
2. `Logger`
3. `WiFiManager`
4. `Metrics`
5. `Ethernet`
6. `Relays`
7. `Valves`
8. `NodeState`
9. `HttpApi`
10. `DigitalInputs`

Loop order:

1. Serial development command processing, compiled out by default
2. `Config`
3. `WiFiManager`
4. `Metrics`
5. `HttpApi`
6. `Ethernet`
7. `Relays`
8. `Valves`
9. `DigitalInputs`

Normal firmware boots with all relays OFF. Automatic relay self-tests and
serial development commands are behind compile-time flags and disabled by
default.

## HTTP API Boundary

`HttpApi` may:

- register routes,
- parse HTTP request details,
- validate request bodies,
- call application ports,
- call `NodeState`,
- serialize typed structures to JSON,
- send HTTP responses.

`HttpApi` must not:

- call ESP runtime APIs directly,
- call WiFi APIs directly,
- call TCA9554 or relay hardware code directly,
- own relay state,
- implement agronomic decisions.
