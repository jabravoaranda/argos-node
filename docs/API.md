# ARGOS Node API

Base URL examples use `http://192.168.1.138`. Replace it with the IP address
printed by the node after WiFi connects.

The API is JSON over HTTP. Responses use:

```http
Content-Type: application/json
```

No authentication is implemented in `v0.1.0`.

## Endpoint Summary

| Method | Path | Purpose |
| --- | --- | --- |
| GET | `/health` | Minimal liveness check |
| GET | `/info` | Static identity and capabilities |
| GET | `/status` | Current operational state |
| GET | `/metrics` | Runtime telemetry for node health monitoring |
| GET | `/outputs` | Current output states |
| PUT | `/outputs/relays/<id>` | Set one relay output |
| GET | `/valves` | Current configured valve states |
| GET | `/valves/1` | Current state for electroválvula 1 |
| PUT | `/valves/1` | Open or close electroválvula 1 |
| POST | `/valves/1/open` | Open electroválvula 1 |
| POST | `/valves/1/close` | Close electroválvula 1 |

## GET /health

Minimal liveness endpoint.

Response `200`:

```json
{
  "status": "ok"
}
```

Example:

```powershell
Invoke-RestMethod http://192.168.1.138/health
```

```sh
curl http://192.168.1.138/health
```

## GET /info

Returns identity, network address information, and declared capabilities.

Response `200`:

```json
{
  "node": {
    "name": "argos-node",
    "firmware_version": "0.1.0",
    "build_date": "<compiler date/time>",
    "board": "Waveshare ESP32-S3-POE-ETH-8DI-8RO"
  },
  "network": {
    "mac": "<mac>",
    "ip": "<ip>"
  },
  "capabilities": {
    "relays": {
      "available": 8,
      "implemented": true
    },
    "digital_inputs": {
      "available": 8,
      "implemented": false
    },
    "flowmeters": {
      "available": 0,
      "implemented": false
    },
    "analog_inputs": {
      "available": 0,
      "implemented": false
    },
    "wifi": {
      "available": true,
      "implemented": true
    },
    "ethernet": {
      "available": true,
      "implemented": false
    },
    "rs485": {
      "available": true,
      "implemented": false
    }
  }
}
```

Example:

```powershell
Invoke-RestMethod http://192.168.1.138/info
```

```sh
curl http://192.168.1.138/info
```

## GET /status

Returns current operational state. This endpoint includes relay states and
explicit placeholders for inputs and flowmeter data that are not implemented.

Response `200`:

```json
{
  "node": {
    "name": "argos-node",
    "firmware_version": "0.1.0",
    "uptime_s": 1234
  },
  "network": {
    "wifi_connected": true,
    "ssid": "<ssid>",
    "ip": "<ip>",
    "rssi_dbm": -50,
    "mac": "<mac>"
  },
  "outputs": {
    "relays": [
      {
        "id": 1,
        "state": false
      }
    ]
  },
  "valves": [
    {
      "id": 1,
      "name": "electrovalvula_1",
      "state": "closed"
    }
  ],
  "inputs": {
    "digital": [
      {
        "id": 1,
        "state": null,
        "implemented": false
      }
    ]
  },
  "flowmeter": {
    "implemented": false,
    "pulse_count": null,
    "flow_l_min": null,
    "total_l": null
  },
  "system": {
    "free_heap_bytes": 0,
    "minimum_free_heap_bytes": 0
  }
}
```

Example:

```powershell
Invoke-RestMethod http://192.168.1.138/status
```

```sh
curl http://192.168.1.138/status
```

## GET /metrics

Returns runtime telemetry for monitoring node health. It does not include relay
commands, irrigation logic, historical data, or business meaning for inputs.

Response `200`:

```json
{
  "system": {
    "uptime_s": 1832,
    "boot_count": null,
    "firmware_version": "0.1.0",
    "build_date": "<compiler date/time>",
    "cpu_frequency_mhz": 240
  },
  "memory": {
    "heap_free_bytes": 0,
    "heap_minimum_bytes": 0,
    "heap_largest_block_bytes": 0
  },
  "wifi": {
    "connected": true,
    "rssi_dbm": -48
  },
  "network": {
    "ip": "<ip>",
    "mac": "<mac>"
  },
  "flash": {
    "size_bytes": 16777216,
    "used_bytes": 0
  },
  "tasks": {
    "main_loop_hz": null
  }
}
```

Example:

```powershell
Invoke-RestMethod http://192.168.1.138/metrics
```

```sh
curl http://192.168.1.138/metrics
```

## GET /outputs

Returns current output states.

Response `200`:

```json
{
  "relays": [
    {
      "id": 1,
      "state": false
    },
    {
      "id": 2,
      "state": false
    }
  ]
}
```

Example:

```powershell
Invoke-RestMethod http://192.168.1.138/outputs
```

```sh
curl http://192.168.1.138/outputs
```

Display all relays in PowerShell:

```powershell
(Invoke-RestMethod http://192.168.1.138/outputs).relays |
    Format-Table id, state
```

## PUT /outputs/relays/<id>

Development relay write endpoint. It sets one relay ON or OFF. It does not
implement irrigation logic, scheduling, automation, or authentication.

Valid relay IDs are `1` through `8`.

Request body:

```json
{
  "state": true
}
```

or:

```json
{
  "state": false
}
```

Response `200`:

```json
{
  "relay": 1,
  "state": true,
  "result": "ok"
}
```

Error responses:

| Status | Cause | Example response |
| --- | --- | --- |
| 400 | Invalid JSON object | `{"error":"invalid_json"}` |
| 400 | Missing `state` | `{"error":"missing_state"}` |
| 400 | Non-boolean `state` | `{"error":"non_boolean_state"}` |
| 404 | Relay id outside `1..8` | `{"error":"invalid_relay"}` |
| 500 | Relay driver write failed | `{"error":"relay_command_failed"}` |

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

curl:

```sh
curl -X PUT http://192.168.1.138/outputs/relays/1 -H "Content-Type: application/json" -d "{\"state\":true}"
curl -X PUT http://192.168.1.138/outputs/relays/1 -H "Content-Type: application/json" -d "{\"state\":false}"
```

## Valve API

Electroválvula 1 is physically wired to relay CH1. Relay CH1 ON means the
valve is open; relay CH1 OFF means the valve is closed. The valve API reuses
the relay driver through the semantic valve controller and does not write to
the TCA9554 directly.

The IP address is assigned by the active network. Replace `192.168.1.138` in
the examples with the IP address printed by the node after WiFi connects.

### GET /valves

Returns configured valve states.

Response `200`:

```json
{
  "valves": [
    {
      "id": 1,
      "name": "electrovalvula_1",
      "relay_id": 1,
      "state": "closed"
    }
  ]
}
```

Example:

```powershell
Invoke-RestMethod http://192.168.1.138/valves
```

### GET /valves/1

Returns electroválvula 1 state. The returned state reflects the relay state
stored by the relay driver.

Response `200`:

```json
{
  "id": 1,
  "name": "electrovalvula_1",
  "relay_id": 1,
  "state": "closed"
}
```

Error responses:

| Status | Cause | Example response |
| --- | --- | --- |
| 404 | Valve id is not configured | `{"error":"invalid_valve"}` |

Example:

```powershell
Invoke-RestMethod http://192.168.1.138/valves/1
```

### PUT /valves/1

Opens or closes electroválvula 1.

Request body:

```json
{
  "state": "open"
}
```

or:

```json
{
  "state": "closed"
}
```

Response `200`:

```json
{
  "id": 1,
  "name": "electrovalvula_1",
  "relay_id": 1,
  "state": "open"
}
```

Error responses:

| Status | Cause | Example response |
| --- | --- | --- |
| 400 | Invalid JSON object | `{"error":"invalid_json"}` |
| 400 | Missing `state` | `{"error":"missing_state"}` |
| 400 | Non-string `state` | `{"error":"non_string_state"}` |
| 400 | Unsupported `state` value | `{"error":"invalid_state"}` |
| 404 | Valve id is not configured | `{"error":"invalid_valve"}` |
| 500 | Valve command failed | `{"error":"valve_command_failed"}` |

Open:

```powershell
Invoke-RestMethod `
  -Method Put `
  -Uri "http://192.168.1.138/valves/1" `
  -ContentType "application/json" `
  -Body '{"state":"open"}'
```

Close:

```powershell
Invoke-RestMethod `
  -Method Put `
  -Uri "http://192.168.1.138/valves/1" `
  -ContentType "application/json" `
  -Body '{"state":"closed"}'
```

### POST /valves/1/open

Shortcut to open electroválvula 1.

```powershell
Invoke-RestMethod -Method Post http://192.168.1.138/valves/1/open
```

### POST /valves/1/close

Shortcut to close electroválvula 1.

```powershell
Invoke-RestMethod -Method Post http://192.168.1.138/valves/1/close
```

## Notes

- Normal firmware boots with all relays OFF.
- Valve initialization also forces configured valves closed.
- No relay is energized automatically in normal builds.
- Digital inputs are exposed as not implemented until the driver exists.
- Flowmeter fields are exposed as not implemented until flowmeter support exists.
- WiFi SSID may appear in `/status`; WiFi password is never returned.
