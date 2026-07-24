# argos-node

Firmware for ARGOS field nodes: industrial ESP32-based controllers for sensing,
actuation, local safety, status reporting, and reliable communications.

ARGOS means Agricultural Remote Guidance and Observation System. High-level
agronomic logic belongs to ARGOS Core, not to this firmware.

## Status

First stable milestone: `v0.1.0`.

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
- 8 isolated digital inputs, firmware support pending
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
- `DigitalInputs`: placeholder module; inputs are not implemented yet.
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
Firmware version: 0.1.0
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
- `GET /valves/1`
- `PUT /valves/1`
- `POST /valves/1/open`
- `POST /valves/1/close`

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

Turn relay 1 ON:

```powershell
Invoke-RestMethod `
  -Uri http://192.168.1.138/outputs/relays/1 `
  -Method Put `
  -ContentType "application/json" `
  -Body '{"state":true}'
```

Turn relay 1 OFF:

```powershell
Invoke-RestMethod `
  -Uri http://192.168.1.138/outputs/relays/1 `
  -Method Put `
  -ContentType "application/json" `
  -Body '{"state":false}'
```

Helper script:

```powershell
.\tools\relay.ps1 1 on
.\tools\relay.ps1 1 off
.\tools\relay.ps1 status
```

## Electroválvula 1

Electroválvula 1 is wired to relay CH1 on the Waveshare
ESP32-S3-POE-ETH-8DI-8RO:

- CH1 `COM` to `LOAD+`.
- CH1 `NO` to the electroválvula red wire.
- Electroválvula black wire to `LOAD-`.
- Relay CH1 ON means electroválvula open.
- Relay CH1 OFF means electroválvula closed.

The semantic valve API lets ARGOS request valve operations without addressing
the physical relay directly.

Open electroválvula 1:

```powershell
Invoke-RestMethod `
  -Method Put `
  -Uri "http://192.168.1.138/valves/1" `
  -ContentType "application/json" `
  -Body '{"state":"open"}'
```

Read electroválvula 1:

```powershell
Invoke-RestMethod http://192.168.1.138/valves/1
```

Close electroválvula 1:

```powershell
Invoke-RestMethod `
  -Method Put `
  -Uri "http://192.168.1.138/valves/1" `
  -ContentType "application/json" `
  -Body '{"state":"closed"}'
```

Shortcut endpoints:

```powershell
Invoke-RestMethod -Method Post http://192.168.1.138/valves/1/open
Invoke-RestMethod -Method Post http://192.168.1.138/valves/1/close
```

Manual validation checklist:

1. Reboot the ESP32 and verify `GET /valves/1` returns `"state":"closed"`.
2. Run `PUT /valves/1` with `{"state":"open"}` and verify CH1 activates.
3. Run `GET /valves/1` and verify it returns `"state":"open"`.
4. Run `PUT /valves/1` with `{"state":"closed"}` and verify CH1 deactivates.
5. Run `PUT /valves/1` with `{"state":"bad"}` and verify HTTP 400.
6. Run `GET /valves/2` and verify HTTP 404.
7. Read `GET /outputs` before and after valve commands and verify relays 2..8 do not change.

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
Prepare ARGOS Node v0.1.0 stable milestone
```

Suggested annotated tag:

```powershell
git tag -a v0.1.0 -m "First operational ARGOS Node"
```
