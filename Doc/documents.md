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
