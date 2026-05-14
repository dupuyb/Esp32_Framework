# ESP32 FrameWeb

FrameWeb is an ESP32 web framework for PlatformIO (Arduino) with:

- Wi-Fi provisioning (WiFiManager)
- HTTP server (port 80)
- WebSocket server (port 81)
- OTA updates (ArduinoOTA + web update)
- mDNS support
- SPIFFS file hosting
- HTTP Basic authentication for sensitive routes

Reference example: `src/demo1.cpp`.

## Quick Start

1. Build:

```bash
platformio run --environment esp32dev
```

2. Upload firmware:

```bash
platformio run --environment esp32dev --target upload
```

3. Open serial monitor:

```bash
platformio device monitor --environment esp32dev
```

## Minimal Integration

```cpp
#include <Arduino.h>
#define DEBUG_FRAMEWEB
#include "FrameWeb.h"

FrameWeb frame;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {}
void saveConfigCallback() {}
void configModeCallback(WiFiManager *myWiFiManager) {}

void setup() {
  Serial.begin(115200);
  frame.setup();
}

void loop() {
  frame.loop();
}
```

Required callbacks are declared in `src/FrameWeb.h`.

## Project Layout

- `src/FrameWeb.h`: public API, `Config` structure, callback declarations
- `src/FrameWeb.cpp`: framework implementation
- `src/demo1.cpp`: end-to-end usage example
- `src/FrameWeb.html`: editable embedded web templates
- `extra_script.py`: regenerates embedded HTML blocks in `src/FrameWeb.cpp`
- `Data/config.json`: configuration example
- `platformio.ini`: environments, dependencies, pre-build script

## Configuration

Default config file in SPIFFS: `/config.json`

```json
{
  "HostName": "esp32dudu",
  "MacAddress": [48, 174, 164, 144, 253, 200],
  "ResetWifi": false,
  "LoginName": "admin",
  "LoginPassword": "admin",
  "UseToolsLocal": true
}
```

Notes:

- Missing/invalid config is recreated with defaults.
- `UseToolsLocal=true` enables built-in pages when SPIFFS pages are missing.
- `ResetWifi=true` triggers Wi-Fi credentials reset in the next loop cycle.

## HTTP API

- `GET /`: index page (SPIFFS or built-in tools page)
- `GET /ls`: plain text SPIFFS listing
- `GET /upload`: upload page
- `POST /upload`: upload file to SPIFFS
- `GET /explorer`: file explorer (download/delete)
- `POST /update`: OTA firmware upload (`.bin`)
- `POST /post`: system commands (`restart`, `save-config`, `reset-wifi`, `restore`)

Authentication uses `LoginName` and `LoginPassword` from `config.json`.

## API Usage Examples (from sibling projects)

The examples below are inspired by working patterns in this workspace, especially:

- `Esp32_Jeedom` (`src/main.cpp`, `include/eau.h`, `Data/websocket.js`)
- `Esp32_Pricetags` (`RFV3/web.cpp`)

They can be reused directly for FrameWeb routes (`frame.server`) or adapted to custom endpoints.

### 1) Browser WebSocket client (Jeedom pattern)

This pattern matches FrameWeb default WebSocket port `81`:

```javascript
const ws = new WebSocket("ws://" + location.hostname + ":81/", ["arduino"]);

ws.onopen = () => {
  console.log("connected");
};

ws.onmessage = (evt) => {
  console.log("server:", evt.data);
};

function sendHexValue(hexPayload) {
  ws.send(hexPayload); // e.g. "#7f"
}
```

Source inspiration: `Esp32_Jeedom/Data/websocket.js`.

### 2) Mixed GET/POST endpoint with same handler (`/eau` style)

In `Esp32_Jeedom/include/eau.h`, one function handles both `GET` and `POST` and reads query/form arguments from the same request object.

FrameWeb adaptation example:

```cpp
frame.server.on("/api/device", []() {
  if (frame.server.method() == HTTP_GET || frame.server.method() == HTTP_POST) {
    for (uint8_t i = 0; i < frame.server.args(); i++) {
      const String key = frame.server.argName(i);
      const String value = frame.server.arg(i);
      // Apply your key/value update logic here
    }
  }
  frame.server.send(200, "application/json", "{\"ok\":true}");
});
```

Quick tests:

```bash
curl "http://<device-ip>/api/device?Cmd=open"
curl -X POST "http://<device-ip>/api/device" -d "DFE=30&IPL=1.2"
```

Source inspiration: `Esp32_Jeedom/src/main.cpp` (`/eau` route) and `Esp32_Jeedom/include/eau.h` (`sentHtmlEau`).

### 3) Parameter validation style for command endpoints (`/set_file` style)

`Esp32_Pricetags/RFV3/web.cpp` uses explicit parameter checks before action:

```cpp
frame.server.on("/api/cmd", HTTP_POST, []() {
  if (!frame.server.hasArg("id") || !frame.server.hasArg("cmd")) {
    frame.server.send(400, "text/plain", "Wrong parameter");
    return;
  }

  const int id = frame.server.arg("id").toInt();
  const String cmd = frame.server.arg("cmd");
  // Execute action using id/cmd

  frame.server.send(200, "text/plain", "OK");
});
```

Quick test:

```bash
curl -X POST "http://<device-ip>/api/cmd" -d "id=3&cmd=AA55"
```

Source inspiration: `Esp32_Pricetags/RFV3/web.cpp` (`/set_file`, `/set_cmd`).

### 4) Existing FrameWeb endpoints with curl

```bash
# List SPIFFS files
curl -u admin:admin "http://<device-ip>/ls"

# Trigger a system action
curl -u admin:admin -X POST "http://<device-ip>/post" -d "cmd=restart"

# Upload a SPIFFS asset
curl -u admin:admin -F "Choose file=@Data/success.html" "http://<device-ip>/upload"

# Upload firmware (OTA web)
curl -u admin:admin -F "update=@.pio/build/esp32dev/firmware.bin" "http://<device-ip>/update"
```

## Networking

- HTTP: `80`
- WebSocket: `81`
- mDNS services: `http` (80), `ws` (81), `esp32` (8888)

Typical access:

- `http://<device-ip>/`
- `http://<hostname>.local/`

## Embedded HTML Generation

Embedded pages are sourced from `src/FrameWeb.html` and injected into `src/FrameWeb.cpp` by `extra_script.py` (pre-build via `platformio.ini`).

Do not manually edit generated sections between:

- `//---- Start Generated`
- `//---- End Generated`

## Demo Highlights

`src/demo1.cpp` includes:

- LED heartbeat
- NTP synchronization
- serial debug commands (`h`, `i`, `f`, `r`)
- runtime extension of tools page with `frame.externalHtmlTools`

## Troubleshooting

- Serial issues: verify `monitor_port` and `upload_port` in `platformio.ini`.
- Wi-Fi portal issues: enable/check `DEBUG_FRAMEWEB` serial logs.
- Missing web pages: check SPIFFS and keep `UseToolsLocal=true`.
- OTA issues: verify credentials and `.bin` image.

## Security Note

Change default credentials before production deployment.
