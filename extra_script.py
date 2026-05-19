#!/usr/bin/env python3

"""extra_script.py

Utility script used by PlatformIO pre-build to generate embedded HTML blocks
inside the C++ source file.

Overview
--------
- Reads HTML template fragments from the input file.
- Detects markers such as: <!-- const char HTTP_... -->
- Converts each marked block to one-line C++ string declarations.
- Replaces only the generated block delimited by:
    - //---- Start Generated
    - //---- End Generated
- In PlatformIO mode, supports one or multiple input/output pairs from
    custom_in_html/custom_out_h (sequential mapping: input[i] -> output[i]).

Execution modes
---------------
1) PlatformIO pre-script mode (automatic)
     Imported by PlatformIO from platformio.ini via extra_scripts.

2) CLI mode (manual)
     - Find template keys:
         python3 extra_script.py -f src/FrameWeb.html
     - Generate compressed C++ lines to stdout:
         python3 extra_script.py -i src/FrameWeb.html
     - Generate FrameWeb-compatible zipped payloads:
         python3 extra_script.py -z -i src/FrameWeb.html
     - Generate smallest payload per block (plain vs zipped):
         python3 extra_script.py -zib -i src/FrameWeb.html
     - Generate compressed C++ lines to file:
         python3 extra_script.py -i src/FrameWeb.html -o out.cpp
     - Build a standalone wrapper skeleton:
         python3 extra_script.py -i src/FrameWeb.html -b out.h

Notes
-----
- The generated region in the C++ file is overwritten on each run.
- Edit the HTML source file, not the generated block in the C++ file.
- For multi-file PlatformIO generation, custom_in_html and custom_out_h must
    contain the same number of entries.
"""

from __future__ import annotations

import argparse
import datetime
import hashlib
import os
import re
import sys
import zlib
from pathlib import Path
from tempfile import mkstemp
from shutil import move
from os import close, remove

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

try:
    Import("env")
except Exception:
    env = None


PROJECT_ROOT = Path("platformio.ini").resolve().parent

# Pattern around KEY here %%Key%%
REGPAT = r"%%(\w+)%%"
REGS = "%%"
REGE = "%%"
HTML_MARKER_PREFIX = "<!-- const char HTTP_"
CPP_START_MARKER = "//---- Start Generated"
CPP_END_MARKER = "//---- End Generated"


def _build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="extra_script.py",
        description="Extract template keys and generate C++ strings from HTML blocks.",
    )
    parser.add_argument("-i", "--ifile", "--inputfile", dest="inputfile", default="")
    parser.add_argument("-o", "--ofile", "--outputfile", dest="outputfile", default="")
    parser.add_argument("-f", "--ffile", "--findfile", dest="findfile", default="")
    parser.add_argument("-b", "--bfile", "--buildfile", dest="buildfile", default="")
    parser.add_argument(
        "-z",
        "--zip",
        dest="zip_output",
        action="store_true",
        help="compress HTML payloads using the FrameWeb size:hex-zlib format",
    )
    parser.add_argument(
        "-zib",
        "--zip-if-best",
        dest="zip_if_best",
        action="store_true",
        help="for each block, keep the smallest payload between plain and size:hex-zlib",
    )
    return parser


def help(opt: int = 0) -> None:
    parser = _build_parser()
    parser.print_help()
    raise SystemExit(opt)


def _resolve_path(path_text: str) -> Path:
    path = Path(path_text)
    if path.is_absolute():
        return path
    return PROJECT_ROOT / path


def _require_existing_file(path_text: str) -> Path:
    candidates = []
    raw_path = Path(path_text)
    if raw_path.is_absolute():
        candidates.append(raw_path)
    else:
        candidates.append(raw_path)
        candidates.append(PROJECT_ROOT / raw_path)

    for candidate in candidates:
        if candidate.is_file():
            return candidate

    checked = ", ".join(str(c) for c in candidates)
    raise FileNotFoundError(f"file not found: {path_text} (checked: {checked})")


def _to_cpp_identifier(text: str) -> str:
    ident = re.sub(r"\W+", "_", text)
    if not ident:
        ident = "item"
    if ident[0].isdigit():
        ident = f"_{ident}"
    return ident


def _to_header_guard(path: Path) -> str:
    stem = _to_cpp_identifier(path.stem).upper()
    return f"{stem}_H"


def _extract_template_keys(content: str) -> list[str]:
    pattern = re.compile(REGPAT, re.IGNORECASE)
    tags = sorted(set(match.group(1) for match in pattern.finditer(content)))
    return tags


def _frameweb_zip_string(text: str) -> str:
    """Compress text in the same size:hex-zlib format expected by FrameWeb."""
    raw = text.encode("utf-8")
    compressed = zlib.compress(raw, level=9)
    return f"{len(raw)}:{compressed.hex().upper()}"


def _compute_file_sha256(path_text: str) -> str:
    path = _require_existing_file(path_text)
    digest = hashlib.sha256()
    with open(path, "rb") as source_file:
        while True:
            chunk = source_file.read(8192)
            if not chunk:
                break
            digest.update(chunk)
    return digest.hexdigest()


def _extract_generated_hash_from_cpp(path_text: str) -> str | None:
    path = _require_existing_file(path_text)
    hash_pattern = re.compile(r"\bsha256=([0-9a-fA-F]{64})\b")

    with open(path, "r", encoding="utf-8") as source_file:
        for line in source_file:
            if line.startswith(CPP_START_MARKER):
                match = hash_pattern.search(line)
                return match.group(1).lower() if match else None
    return None


def _extract_generated_meta_from_cpp(path_text: str) -> tuple[str | None, str | None]:
        """Read stored generation metadata from Start marker.

        Expected format contains optional fields:
            ... | sha256=<64-hex> | zip_mode=<plain|zip|zip_if_best>
        """
        path = _require_existing_file(path_text)
        hash_pattern = re.compile(r"\bsha256=([0-9a-fA-F]{64})\b")
        mode_pattern = re.compile(r"\bzip_mode=([a-zA-Z0-9_]+)\b")

        with open(path, "r", encoding="utf-8") as source_file:
                for line in source_file:
                        if line.startswith(CPP_START_MARKER):
                                hash_match = hash_pattern.search(line)
                                mode_match = mode_pattern.search(line)
                                source_hash = hash_match.group(1).lower() if hash_match else None
                                zip_mode = mode_match.group(1).strip().lower() if mode_match else None
                                return source_hash, zip_mode

        return None, None


def replace(source_file_path: str, code: list[str]) -> None:
    """Replace only the generated block in a C++ file."""
    source_path = _require_existing_file(source_file_path)
    fd, target_file_path = mkstemp()
    close(fd)

    found_start = False
    found_end = False
    in_generated_section = False

    with open(target_file_path, "w", encoding="utf-8") as target_file:
        with open(source_path, "r", encoding="utf-8") as source_file:
            for line in source_file:
                if line.startswith(CPP_START_MARKER):
                    found_start = True
                    in_generated_section = True
                    for entry in code:
                        target_file.write(entry + "\n")
                    continue

                if in_generated_section:
                    if line.startswith(CPP_END_MARKER):
                        found_end = True
                        in_generated_section = False
                    continue

                target_file.write(line)

    if not (found_start and found_end) or in_generated_section:
        remove(target_file_path)
        raise ValueError(f"cannot replace generated block in: {source_path}")

    remove(source_path)
    move(target_file_path, source_path)


def GetListOfSubstrings(stringSubject: str, string1: str, string2: str) -> list[str]:
    """Legacy helper kept for compatibility with previous script API."""
    my_list: list[str] = []
    intstart = 0
    strlength = len(stringSubject)
    continueloop = 1
    while intstart < strlength and continueloop == 1:
        intindex1 = stringSubject.find(string1, intstart)
        if intindex1 != -1:
            intindex1 = intindex1 + len(string1)
            intindex2 = stringSubject.find(string2, intindex1)
            if intindex2 != -1:
                subsequence = stringSubject[intindex1:intindex2]
                my_list.append(subsequence)
                intstart = intindex2 + len(string2)
            else:
                continueloop = 0
        else:
            continueloop = 0
    return my_list


def findPattern(filename: str) -> tuple[list[str], int]:
    """Extract template placeholders of the form %%Key%% from a source file.

    Returns a sorted list of unique keys and the maximum key length.
    """
    path = _require_existing_file(filename)
    content = path.read_text(encoding="utf-8")
    tags = _extract_template_keys(content)
    strlen = max((len(tag) for tag in tags), default=0)
    return tags, strlen


def conpressHtml(
    inputfile: str,
    zip_output: bool = False,
    zip_if_best: bool = False,
    source_hash: str | None = None,
    zip_mode: str = "plain",
) -> list[str]:
    """Legacy typo kept for backward compatibility.

    zip_output=True forces FrameWeb compressed format:
    <original_size>:<hex_zlib_payload>.

    zip_if_best=True compares plain and compressed payload sizes per block,
    then keeps the smallest one.
    """
    path = _require_existing_file(inputfile)
    lines = path.read_text(encoding="utf-8").splitlines()

    ret: list[str] = []
    total_str_length = 0
    total_raw_length = 0
    total_payload_length = 0
    if source_hash is None:
        source_hash = _compute_file_sha256(inputfile)
    ret.append(
        "//---- Start Generated from "
        + str(inputfile)
        + " file --- "
        + str(datetime.datetime.now().replace(microsecond=0))
        + " | sha256="
        + source_hash
        + " | zip_mode="
        + zip_mode
    )

    current_decl = ""
    current_payload: list[str] = []

    def flush_block() -> None:
        nonlocal current_decl, current_payload, total_str_length, total_raw_length, total_payload_length
        if not current_decl:
            return
        raw_payload = "".join(current_payload)
        zipped_payload = _frameweb_zip_string(raw_payload)
        raw_payload_len = len(raw_payload.encode("utf-8"))
        zipped_payload_len = len(zipped_payload.encode("utf-8"))

        if zip_if_best:
            payload = zipped_payload if zipped_payload_len < raw_payload_len else raw_payload
        elif zip_output:
            payload = zipped_payload
        else:
            payload = raw_payload

        payload_cpp = payload.replace("\\", "\\\\").replace('"', '\\"')
        cpp_line = f"{current_decl} \"{payload_cpp}\";"
        str_len = len(payload_cpp.encode("utf-8"))
        raw_len = len(raw_payload.encode("utf-8"))
        payload_len = len(payload.encode("utf-8"))
        gain_len = raw_len - payload_len
        gain_pct = (gain_len * 100.0 / raw_len) if raw_len else 0.0

        ret.append(cpp_line)
        if zip_output or zip_if_best:
            ret.append(
                f"//---- len : {str_len} bytes | source={raw_len} | payload={payload_len} | gain={gain_len} ({gain_pct:.1f}%)"
            )
        else:
            ret.append(
                f"//---- len : {str_len} bytes | source={raw_len} | payload={payload_len}"
            )

        total_str_length += str_len
        total_raw_length += raw_len
        total_payload_length += payload_len
        current_decl = ""
        current_payload = []

    for raw_line in lines:
        stripped = raw_line.strip()
        if stripped.startswith(HTML_MARKER_PREFIX):
            flush_block()
            current_decl = stripped.replace("<!--", "", 1).replace("-->", "", 1).strip()
            if not current_decl.endswith("="):
                raise ValueError(f"invalid declaration marker: {stripped}")
            continue

        if current_decl:
            current_payload.append(stripped)

    flush_block()
    if len(ret) == 1:
        raise ValueError(f"no HTML marker found ({HTML_MARKER_PREFIX}) in {path}")

    if zip_output or zip_if_best:
        total_gain = total_raw_length - total_payload_length
        total_gain_pct = (total_gain * 100.0 / total_raw_length) if total_raw_length else 0.0
        ret.append(
            f"//---- total len : {total_str_length} bytes | source={total_raw_length} | payload={total_payload_length} | gain={total_gain} ({total_gain_pct:.1f}%)"
        )
    else:
        ret.append(
            f"//---- total len : {total_str_length} bytes | source={total_raw_length} | payload={total_payload_length}"
        )
    ret.append("//---- End Generated")
    return ret
def compressHtml(
    inputfile: str,
    outputfile: str,
    zip_output: bool = False,
    zip_if_best: bool = False,
    zip_mode: str = "plain",
) -> None:
    """Convert HTML marker blocks into C++ declarations.

    If outputfile is empty, lines are printed to stdout.
    Otherwise they are written to the target file.
    """
    lines = conpressHtml(
        inputfile,
        zip_output=zip_output,
        zip_if_best=zip_if_best,
        source_hash=_compute_file_sha256(inputfile),
        zip_mode=zip_mode,
    )
    if outputfile.strip():
        out_path = _resolve_path(outputfile)
        out_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
        print(f"Generated C++ HTML written to: {out_path}")
    else:
        for line in lines:
            print(line)


def buildEsp32Cpp(
    inputfile: str,
    outputfile: str,
    zip_output: bool = False,
    zip_if_best: bool = False,
) -> None:
    """Generate a standalone C++ wrapper file from the HTML source.

    The output includes:
    - include guard
    - generated HTTP_* string declarations
    - placeholder getter/setter mapping skeleton
    """
    if not outputfile.strip():
        raise ValueError("output file is required for build mode (-b)")

    html_lines = conpressHtml(
        inputfile,
        zip_output=zip_output,
        zip_if_best=zip_if_best,
    )
    tags, _ = findPattern(inputfile)
    out_path = _resolve_path(outputfile)
    guard = _to_header_guard(out_path)
    func_suffix = _to_cpp_identifier(out_path.stem)

    generated: list[str] = []
    generated.append(f"#ifndef {guard}")
    generated.append(f"#define {guard}")
    generated.append("")
    generated.extend(html_lines)
    generated.append("")
    generated.append("// -------- Web template wrappers Get/Set -------------")

    for tag in tags:
        cpp_tag = _to_cpp_identifier(tag)
        generated.append(f"String get_{cpp_tag}() {{ /* TODO */ return String(); }}")
        generated.append(f"void set_{cpp_tag}(const String& value) {{ (void)value; /* TODO */ }}")

    generated.append("")
    generated.append("struct Equiv {")
    generated.append("  const char* key;")
    generated.append("  String (*get_ptr)();")
    generated.append("  void (*set_ptr)(const String&);")
    generated.append("};")
    generated.append("")
    generated.append(f"constexpr size_t NBRITEMINDICO = {len(tags)};")
    generated.append("")
    generated.append("Equiv dico[] = {")
    for tag in tags:
        cpp_tag = _to_cpp_identifier(tag)
        generated.append(f'  {{"{REGS}{tag}{REGE}", &get_{cpp_tag}, &set_{cpp_tag}}},')
    generated.append("};")
    generated.append("")
    generated.append(f"String sendHtml_{func_suffix}() {{")
    generated.append("  String rt;")
    generated.append("  // TODO: Build `rt` from HTTP_* chunks and replace placeholders using `dico`.")
    generated.append("  return rt;")
    generated.append("}")
    generated.append("")
    generated.append(f"#endif  // {guard}")

    out_path.write_text("\n".join(generated) + "\n", encoding="utf-8")
    print(f"Generated C++ wrapper written to: {out_path}")


def selesctApp(argv: list[str]) -> int:
    """Dispatch CLI mode based on command-line arguments."""
    parser = _build_parser()
    args = parser.parse_args(argv)

    if args.findfile.strip():
        tg, ln = findPattern(args.findfile.strip())
        print("Key list   :", tg)
        print("Number Key :", len(tg))
        print("Max Key len:", ln)
        return 0

    if args.buildfile.strip():
        if not args.inputfile.strip():
            parser.error("-i/--ifile is required with -b/--bfile")
        buildEsp32Cpp(
            args.inputfile.strip(),
            args.buildfile.strip(),
            zip_output=args.zip_output,
            zip_if_best=args.zip_if_best,
        )
        return 0

    if args.inputfile.strip():
        compressHtml(
            args.inputfile.strip(),
            args.outputfile.strip(),
            zip_output=(args.zip_output or args.zip_if_best),
            zip_if_best=args.zip_if_best,
        )
        return 0

    parser.print_help()
    return 2


def _load_platformio_config() -> tuple[configparser.ConfigParser, Path]:
    config_path = PROJECT_ROOT / "platformio.ini"
    if not config_path.is_file():
        raise FileNotFoundError(f"platformio.ini not found at {config_path}")

    config = configparser.ConfigParser()
    loaded = config.read(config_path)
    if not loaded:
        raise ValueError(f"cannot read {config_path}")
    return config, config_path


def _detect_env_section(config: configparser.ConfigParser) -> str:
    pio_env = ""
    if env is not None:
        try:
            pio_env = str(env.subst("$PIOENV")).strip()
        except Exception:
            pio_env = ""

    if not pio_env:
        pio_env = os.environ.get("PIOENV", "").strip()

    if pio_env:
        section = f"env:{pio_env}"
        if config.has_section(section):
            return section

    if config.has_section("env:esp32dev"):
        return "env:esp32dev"

    for section in config.sections():
        if section.startswith("env:"):
            return section

    raise ValueError("no [env:*] section found in platformio.ini")


def _resolve_zip_mode(config: configparser.ConfigParser, section: str) -> tuple[bool, bool, str]:
    """Resolve zip mode from custom_out_zip.

    Returns:
        (zip_output, zip_if_best, normalized_mode)
    """
    value = config.get(section, "custom_out_zip", fallback="plain").strip().lower()
    if value in {"zip_if_best", "zib", "best", "auto"}:
        return True, True, "zip_if_best"
    if value in {"zip", "zipped", "true", "on", "yes", "1"}:
        return True, False, "zip"
    return False, False, "plain"


def _get_platformio_list_option(
    config: configparser.ConfigParser,
    section: str,
    key: str,
) -> list[str]:
    """Read a PlatformIO-style list option (one or multiple lines)."""
    raw_value = config.get(section, key, fallback="")
    items: list[str] = []
    for raw_line in raw_value.splitlines():
        line = raw_line.strip()
        if not line or line.startswith((";", "#")):
            continue
        items.append(line)
    return items


def run_platformio_pre_script() -> int:
    """Run automatic pre-build generation using values from platformio.ini.

    Uses the active environment section and resolves custom_in_html/custom_out_h.
    Each input/output pair is processed sequentially:
    custom_in_html[i] -> custom_out_h[i].
    """
    config, _ = _load_platformio_config()
    section = _detect_env_section(config)
    pio_env = section.removeprefix("env:")

    inputfiles = _get_platformio_list_option(config, section, "custom_in_html")
    outputfiles = _get_platformio_list_option(config, section, "custom_out_h")
    if not inputfiles or not outputfiles:
        raise ValueError(
            f"custom_in_html/custom_out_h missing in [{section}] of platformio.ini"
        )
    if len(inputfiles) != len(outputfiles):
        raise ValueError(
            f"custom_in_html/custom_out_h count mismatch in [{section}] "
            f"({len(inputfiles)} vs {len(outputfiles)})"
        )

    _B = "\033[94m"; _R = "\033[0m"
    print(_B + "=" * 72 + _R)
    print(_B + "=== FrameWeb extra_script.py START (PlatformIO pre-build generation) ===" + _R)
    print(_B + "-" * 72 + _R)
    print(f"PlatformIO env: {pio_env}")
    print(f"PlatformIO section: [{section}]")
    print(f"Input/output pairs: {len(inputfiles)}")

    zip_output, zip_if_best, zip_mode = _resolve_zip_mode(config, section)
    if zip_if_best:
        print("custom_out_zip=zip_if_best -> best-size generation ON")
    else:
        print(f"custom_out_zip={zip_mode} -> zip generation {'ON' if zip_output else 'OFF'}")

    for idx, (inputfile, outputfile) in enumerate(zip(inputfiles, outputfiles), start=1):
        print(f"---> [{idx}/{len(inputfiles)}] EXTRACT HTML FILE :{inputfile}--------------------")
        tg, ln = findPattern(inputfile)
        print("Key list   :", tg)
        print("Number Key :", len(tg))
        print("Max Key len:", ln)

        source_hash = _compute_file_sha256(inputfile)
        existing_hash, existing_zip_mode = _extract_generated_meta_from_cpp(outputfile)
        if existing_hash == source_hash and existing_zip_mode == zip_mode:
            print(
                f"[INFO] HTML unchanged for {inputfile}. "
                f"Skip update."
            )
            print(f"---> [{idx}/{len(inputfiles)}] END OF HTML FILE :{outputfile}--------------------")
            continue

        if existing_hash == source_hash and existing_zip_mode != zip_mode:
            print(
                f"[INFO] Zip mode changed for {inputfile}: "
                f"{existing_zip_mode or 'unknown'} -> {zip_mode}. Force regeneration."
            )

        code = conpressHtml(
            inputfile,
            zip_output=zip_output,
            zip_if_best=zip_if_best,
            source_hash=source_hash,
            zip_mode=zip_mode,
        )
        replace(outputfile, code)
        print(f"---> [{idx}/{len(inputfiles)}] END OF HTML FILE :{outputfile}--------------------")

    print(_B + "-" * 72 + _R)
    print(_B + "=== FrameWeb extra_script.py END (PlatformIO pre-build generation) =====" + _R)
    print(_B + "=" * 72 + _R)
    return 0


def main(argv: list[str] | None = None) -> int:
    """CLI entrypoint with consistent error handling and return code."""
    try:
        return selesctApp(argv if argv is not None else sys.argv[1:])
    except Exception as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
else:
    run_platformio_pre_script()
