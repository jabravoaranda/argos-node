# argos-node

Firmware for ARGOS field nodes: industrial ESP32-based controllers for sensing,
actuation, local safety, status reporting, and reliable communications.

ARGOS means Agricultural Remote Guidance and Observation System. High-level
agronomic logic belongs to ARGOS Core, not to this firmware.

## Status

Current firmware version: `v0.1.1`.

Validated on target hardware:

- USB serial logging
- WiFi connectivity
- HTTP API
- TCA9554 relay driver
- 8 relay outputs
- 16 MB flash configuration with official `default_16MB.csv` partition table

## Target Hardware

- Waveshare ESP32-S3-POE-ETH-8DI-8RO
- ESP32-S3
- W5500 Ethernet hardware present, firmware support pending
- TCA9554 I2C I/O expander for relay control
- 8 relay outputs
- 8 isolated digital inputs, DI8 firmware support implemented for YF-DN32 flowmeter pulses
- RS485 hardware present, firmware support pending
- USB, WiFi, PoE

## Architecture

`main.cpp` is intentionally minimal. It delegates Arduino `setup()` and `loop()`
to `ArgosNode`.

Main modules:

- `ArgosNode`: application coordinator and application command/query port implementation.
- `Config`: firmware identity and static configuration.
- `Logger`: serial boot banner and runtime log lines.
- `WiFiManager`: WiFi connection and read-only network state accessors.
- `Relays`: hardware-specific TCA9554 relay driver and relay state owner.
- `Valves`: semantic electroválvula controller mapped to physical relays.
- `Metrics`: transport-independent ESP32 runtime telemetry.
- `NodeState`: transport-independent aggregation of node health, info, status, and metrics.
- `HttpApi`: HTTP transport adapter and JSON serialization.
- `DigitalInputs`: isolated digital input driver; DI8 is implemented on GPIO11 as a YF-DN32 flowmeter pulse input.
- `Ethernet`: placeholder module; W5500 support is not implemented yet.

HTTP depends on application ports, not on the concrete `ArgosNode` type:

- `include/ports/NodeCommandPort.h`
- `include/ports/NodeQueryPort.h`

Hardware access stays inside hardware/service modules. `HttpApi` does not call
ESP, WiFi, TCA9554, or relay hardware APIs directly.

## Build

Use PlatformIO:

```powershell
C:\Users\Fizico\.platformio\penv\Scripts\platformio.exe run
```

The current configuration targets 16 MB flash:

```ini
board_upload.flash_size = 16MB
board_upload.maximum_size = 16777216
board_build.partitions = default_16MB.csv
```

Current bench builds keep relay self-tests disabled and serial development
commands enabled:

```ini
-D ARGOS_ENABLE_RELAY_SELFTEST=0
-D ARGOS_ENABLE_SERIAL_DEV_COMMANDS=1
```

## Upload

Close any open serial monitor before upload:

```powershell
C:\Users\Fizico\.platformio\penv\Scripts\platformio.exe run --target upload
```

Open the serial monitor:

```powershell
C:\Users\Fizico\.platformio\penv\Scripts\platformio.exe device monitor --port COM9 --baud 115200
```

## WiFi Configuration

Copy the example credentials file:

```powershell
Copy-Item include\WiFiCredentials.example.h include\WiFiCredentials.h
```

Edit `include/WiFiCredentials.h`:

```cpp
const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
```

`include/WiFiCredentials.h` is ignored by git and must not be committed.

## Serial Output

On startup the node prints a boot banner:

```text
--------------------------------
ARGOS Node
Firmware version: 0.1.1
Build date: <compiler date/time>
Board: Waveshare ESP32-S3-POE-ETH-8DI-8RO
--------------------------------
```

Some startup logs can be missed if the monitor is opened after reset. For API
validation, prefer HTTP requests after WiFi prints the assigned IP address.

## API Overview

Stable endpoints:

- `GET /health`
- `GET /info`
- `GET /status`
- `GET /metrics`
- `GET /outputs`
- `PUT /outputs/relays/<id>`
- `GET /valves`
- `GET /valves/<id>` for configured valve IDs `4` through `8`
- `PUT /valves/<id>` for configured valve IDs `4` through `8`
- `POST /valves/<id>/open` for configured valve IDs `4` through `8`
- `POST /valves/<id>/close` for configured valve IDs `4` through `8`
- `POST /flowmeter/reset-session`
- `POST /flowmeter/reset-total`
- `POST /flowmeter/reset-hydrological-year`

See [docs/API.md](docs/API.md) for request/response formats, status codes, and
examples.

## PowerShell Examples

The node IP is assigned by the current network. Replace `192.168.1.138` in the
examples with the IP address printed by the node after WiFi connects.

Read outputs:

```powershell
Invoke-RestMethod http://192.168.1.138/outputs
```

Read status:

```powershell
Invoke-RestMethod http://192.168.1.138/status
```

Display all relay states:

```powershell
(Invoke-RestMethod http://192.168.1.138/outputs).relays |
    Format-Table id, state
```

Turn relay 8 ON:

```powershell
Invoke-RestMethod `
  -Uri http://192.168.1.138/outputs/relays/8 `
  -Method Put `
  -ContentType "application/json" `
  -Body '{"state":true}'
```

Turn relay 8 OFF:

```powershell
Invoke-RestMethod `
  -Uri http://192.168.1.138/outputs/relays/8 `
  -Method Put `
  -ContentType "application/json" `
  -Body '{"state":false}'
```

Helper script:

```powershell
.\tools\relay.ps1 8 on
.\tools\relay.ps1 8 off
.\tools\relay.ps1 status
```

## Electroválvulas 4 through 8

Electroválvulas 4 through 8 are wired to their matching relay channels CH4
through CH8 on the Waveshare ESP32-S3-POE-ETH-8DI-8RO:

- Relay `COM` to `LOAD+`.
- Relay `NO` to the electroválvula red wire.
- Electroválvula black wire to `LOAD-`.
- Relay ON means electroválvula open.
- Relay OFF means electroválvula closed.

The semantic valve API lets ARGOS request valve operations without addressing
the physical relay directly.

Open electroválvula 6:

```powershell
Invoke-RestMethod `
  -Method Put `
  -Uri "http://192.168.1.138/valves/6" `
  -ContentType "application/json" `
  -Body '{"state":"open"}'
```

Read configured electroválvulas:

```powershell
Invoke-RestMethod http://192.168.1.138/valves
```

Close electroválvula 6:

```powershell
Invoke-RestMethod `
  -Method Put `
  -Uri "http://192.168.1.138/valves/6" `
  -ContentType "application/json" `
  -Body '{"state":"closed"}'
```

Shortcut endpoints:

```powershell
Invoke-RestMethod -Method Post http://192.168.1.138/valves/6/open
Invoke-RestMethod -Method Post http://192.168.1.138/valves/6/close
```

## Flowmeter Counters

The DI8 YF-DN32 flowmeter exposes boot, resettable, hydrological-year, and
valve-session counters in `GET /status`.

- `boot_total_l`: accumulated liters since boot.
- `total_l`: accumulated liters since the last `reset-total` command.
- `hydrological_year_l`: accumulated liters since the last hydrological-year reset.
- `session_l`: liters in the active EV8 session, or the last session if EV8 is closed.
- `last_session_l`: liters in the last closed EV8 session.

Flowmeter sessions are controlled only by `/valves/8` commands. Commands for
EV4, EV5, EV6, EV7, or direct relay writes do not start or stop a valve session.

Manual reset endpoints:

```powershell
Invoke-RestMethod -Method Post http://192.168.1.138/flowmeter/reset-session
Invoke-RestMethod -Method Post http://192.168.1.138/flowmeter/reset-total
Invoke-RestMethod -Method Post http://192.168.1.138/flowmeter/reset-hydrological-year
```

Manual validation checklist:

1. Reboot the ESP32 and verify `GET /valves` returns valves 4 through 8 as `"state":"closed"`.
2. Verify `GET /valves/4` and `GET /valves/5` report relays 4 and 5 respectively.
3. With explicit authorization for hardware actuation, verify each valve command controls its matching relay CH4 through CH8.
4. Run `PUT /valves/8` with `{"state":"bad"}` and verify HTTP 400.
5. Run `GET /valves/1` and verify HTTP 404 with `{"error":"not_found"}`.
6. Verify `session_active` changes only when EV8 opens or closes, not when EV4 through EV7 change.

## Relay Testing

Use the HTTP API or `tools/relay.ps1` to test relays. Normal firmware boots with
all relays OFF and does not energize outputs automatically.

Before connecting external loads:

- Identify `COM`, `NO`, and `NC` for the relay channel.
- With the relay OFF, verify continuity between `COM` and `NC`.
- With the relay ON, verify continuity between `COM` and `NO`.
- Power external devices from their own supply; the relay is only a switch.

## Release

Suggested commit message:

```text
Prepare ARGOS Node v0.1.1
```

Suggested annotated tag:

```powershell
git tag -a v0.1.1 -m "Add EV4 and EV5 valve support"
```
