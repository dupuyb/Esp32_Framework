# ESP32 FrameWeb

FrameWeb is an ESP32 web framework for PlatformIO (Arduino) with:

- Wi-Fi provisioning (WiFiManager)
- HTTP server (port 80)
- WebSocket server (port 81)
- OTA updates (ArduinoOTA + web update)
- mDNS support
- SPIFFS file hosting
- HTTP Basic authentication for sensitive routes

Reference example: `src/examples/demo1.cpp`.

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
- `src/examples/demo1.cpp`: end-to-end usage example
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

## Debug Logging Flag

FrameWeb debug traces use these macros:

- `FDBX(x)`: print without newline
- `FDBXLN(x)`: print with newline
- `FDBXMF(...)`: formatted `printf`-style print

To enable them, define `LOG_FRAMEWEB_ENABLED` in `platformio.ini` build flags:

```ini
build_flags = -D LOG_FRAMEWEB_ENABLED
```

Current behavior is presence-based: if `LOG_FRAMEWEB_ENABLED` is defined, logs are enabled.
The numeric value is currently not used for log filtering.

### Troubleshooting Logs

- No `[F]` logs on serial output:
  - Check `build_flags` includes `-D LOG_FRAMEWEB_ENABLED` in the active environment.
  - Confirm the code uses `FDBX`, `FDBXLN`, or `FDBXMF` (not plain `Serial.print` if you expect `[F]` prefix).
- Serial monitor shows unreadable characters:
  - Verify monitor speed matches firmware speed (default: `115200`).
- Logs still absent after editing `platformio.ini`:
  - Rebuild the correct environment explicitly (`-e esp32dev` or `-e xiao_esp32s3`).
  - Clean and rebuild if needed.
- Build succeeds but upload target shows no logs:
  - Check `monitor_port`/`upload_port` for the selected board.
  - Ensure the board actually rebooted on the same USB/UART interface you monitor.

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

### custom_out_zip mode

FrameWeb supports two generated payload modes for `HTTP_*` constants:

- `custom_out_zip = plain`: generated chunks are plain HTML strings.
- `custom_out_zip = zip` or `zipped`: generated chunks use the FrameWeb zip format:
  - `<raw_size>:<hex_zlib_payload>`

When enabled, runtime decoding is automatic through `decodeHtmlChunk()` used by built-in pages (`simpleUpload`, `simpleIndex`, `simpleFirmware`, explorer).

To enable the mode for a given PlatformIO environment, set the option in `platformio.ini`:

```ini
custom_out_zip = zip
```

To disable it, set it back to plain HTML:

```ini
custom_out_zip = plain
```

`extra_script.py` pre-build behavior is automatically aligned with this option:

- If `custom_out_zip` is `zip` or `zipped`, generation uses zip mode (equivalent to `-z`).
- Otherwise, generation outputs plain HTML strings.

During pre-build, the script logs:

- `PlatformIO env: xiao_esp32s3`
- `PlatformIO section: [env:xiao_esp32s3]`
- `custom_out_zip=zipped -> zip generation ON`

Suggested configuration:

- `custom_out_zip = plain` for environments that should generate plain HTML.
- `custom_out_zip = zip` or `zipped` for environments that should generate zipped HTML.

Do not manually edit generated sections between:

- `//---- Start Generated`
- `//---- End Generated`

## Demo Highlights

`src/examples/demo1.cpp` includes:

- LED heartbeat
- NTP synchronization
- serial debug commands (`h`, `i`, `f`, `r`)
- runtime extension of tools page with `frame.externalHtmlTools`

## Troubleshooting

- Serial issues: verify `monitor_port` and `upload_port` in `platformio.ini`.
- Wi-Fi portal issues: enable/check `DEBUG_FRAMEWEB` serial logs.
- Missing web pages: check SPIFFS and keep `UseToolsLocal=true`.
- OTA issues: verify credentials and `.bin` image.

If your build picks up demo sources from dependencies, explicitly exclude `demo*.cpp` files in `platformio.ini` using `build_src_filter`, for example:

```ini
build_src_filter = +<*>
  -<.git/>
  -<.svn/>
  -<main_wifi.cpp>
  -<.pio/libdeps/esp32-poe/Esp32_Framework/src/examples/*>
```

## Security Note

Change default credentials before production deployment.
