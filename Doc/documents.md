# FrameWeb Documentation (Doc)

## Purpose

This Markdown file mirrors the current built-in FrameWeb pages and complements `documents.pages` / `documents.pdf`.

## Source Of Truth

Built-in pages are defined in `src/FrameWeb.html` and generated into `src/FrameWeb.cpp` by `extra_script.py`.

Current definitions:

- `HTTP_HEADAL`: common HTML head and CSS
- `HTTP_BODYUP`: uploader page body
- `HTTP_BODYID` + `HTTP_BODYI0` + `HTTP_BODYI1`: default tools/index page body
- `HTTP_FIRM0`: firmware update page body
- `HTTP_EXPL0`: explorer shell (runtime appends file rows)

Lightweight alternative:

- `src/FrameLight.html` keeps the same marker layout as `src/FrameWeb.html`
- it removes styles and other non-essential markup
- it can reduce flash usage by about 50% depending on the selected pages

## Runtime Routes

- `/` default tools page (when SPIFFS `index.html` is missing)
- `/upload` upload page
- `/update` OTA update page
- `/explorer` file explorer
- `/ls` plain text file list

## Generation Mode

`platformio.ini` controls generated payload format with `custom_out_zip`:

- `plain`: plain HTML payload
- `zip` or `zipped`: FrameWeb zipped payload (`<raw_size>:<hex_zlib_payload>`)
- `zip_if_best` / `zib` / `best` / `auto`: per block, keeps smallest payload between plain and zipped

### Terminal output

During PlatformIO pre-build, `extra_script.py` logs a summary to the build terminal. Example for two input files:

```
========================================================================
=== FrameWeb extra_script.py START (PlatformIO pre-build generation) ===
------------------------------------------------------------------------
PlatformIO env: esp32dev
PlatformIO section: [env:esp32dev]
Input/output pairs: 2
custom_out_zip=plain -> zip generation OFF
---> [1/2] EXTRACT HTML FILE :.pio/libdeps/esp32dev/Esp32_Framework/src/FrameWeb.html--------------------
Key list   : []
Number Key : 0
Max Key len: 0
---> [1/2] END OF HTML FILE :.pio/libdeps/esp32dev/Esp32_Framework/src/FrameWeb.cpp--------------------
---> [2/2] EXTRACT HTML FILE :src/eau.html--------------------
Key list   : ['Cmd', 'DATE', 'DFE', 'FL', 'HOS', 'IP', 'IPL', 'MAC', 'MFREE', 'MT', 'RB', 'REC', 'TITLE', 'TOE', 'Tst', 'VL']
Number Key : 16
Max Key len: 5
---> [2/2] END OF HTML FILE :include/eau.h--------------------
------------------------------------------------------------------------
=== FrameWeb extra_script.py END (PlatformIO pre-build generation) =====
========================================================================
```

The START/END banner is printed in blue (ANSI `\033[94m`) on terminals that support colors.

`platformio.ini` also supports multiple html->cpp/h generations in one pass:

- `custom_in_html` can list multiple HTML files
- `custom_out_h` can list multiple target cpp/h files
- both lists must have the same number of entries
- mapping is sequential: input[i] -> output[i]

Example:

```ini
custom_in_html =
	src/FrameLight.html

custom_out_h =
	src/FrameWeb.cpp
```

## Doc Folder Files

- `Doc/FrameWeb.html` synchronized template copy
- `Doc/index.html` synchronized default tools page
- `Doc/upload.html` synchronized upload page
- `Doc/update.html` synchronized firmware update page
- `Doc/explore.html` synchronized explorer shell with static sample rows

## Visual References

- `defaultTools.png`
- `action_login.png`
- `action_explorer.png`
- `action_Ls.png`
- `action_Config_json.png`
- `action_Uploader.png`
- `action_update.png`
