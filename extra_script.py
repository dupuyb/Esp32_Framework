#!/usr/bin/env python3

from __future__ import annotations

import argparse
import datetime
import os
import re
import sys
from pathlib import Path
from tempfile import mkstemp
from shutil import move
from os import close, remove

try:
    import configparser
except ImportError:
    import ConfigParser as configparser


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
    path = _require_existing_file(filename)
    content = path.read_text(encoding="utf-8")
    tags = _extract_template_keys(content)
    strlen = max((len(tag) for tag in tags), default=0)
    return tags, strlen


def conpressHtml(inputfile: str) -> list[str]:
    """Legacy typo kept for backward compatibility."""
    path = _require_existing_file(inputfile)
    lines = path.read_text(encoding="utf-8").splitlines()

    ret: list[str] = []
    ret.append(
        "//---- Start Generated from "
        + str(inputfile)
        + " file --- "
        + str(datetime.datetime.now().replace(microsecond=0))
    )

    current_decl = ""
    current_payload: list[str] = []

    def flush_block() -> None:
        nonlocal current_decl, current_payload
        if not current_decl:
            return
        payload = "".join(current_payload)
        cpp_line = f"{current_decl} \"{payload}\";"
        ret.append(cpp_line)
        ret.append(f"//---- len : {len(cpp_line)} bytes")
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
            escaped = stripped.replace("\\", "\\\\").replace('"', '\\"')
            current_payload.append(escaped)

    flush_block()
    if len(ret) == 1:
        raise ValueError(f"no HTML marker found ({HTML_MARKER_PREFIX}) in {path}")

    ret.append("//---- End Generated")
    return ret


def compressHtml(inputfile: str, outputfile: str) -> None:
    lines = conpressHtml(inputfile)
    if outputfile.strip():
        out_path = _resolve_path(outputfile)
        out_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
        print(f"Generated C++ HTML written to: {out_path}")
    else:
        for line in lines:
            print(line)


def buildEsp32Cpp(inputfile: str, outputfile: str) -> None:
    if not outputfile.strip():
        raise ValueError("output file is required for build mode (-b)")

    html_lines = conpressHtml(inputfile)
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
        buildEsp32Cpp(args.inputfile.strip(), args.buildfile.strip())
        return 0

    if args.inputfile.strip():
        compressHtml(args.inputfile.strip(), args.outputfile.strip())
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


def run_platformio_pre_script() -> int:
    config, _ = _load_platformio_config()
    section = _detect_env_section(config)

    inputfile = config.get(section, "custom_in_html", fallback="").strip()
    outputfile = config.get(section, "custom_out_h", fallback="").strip()
    if not inputfile or not outputfile:
        raise ValueError(
            f"custom_in_html/custom_out_h missing in [{section}] of platformio.ini"
        )

    print(f"---> EXTRACT HTML FILE :{inputfile}--------------------")
    tg, ln = findPattern(inputfile)
    print("Key list   :", tg)
    print("Number Key :", len(tg))
    print("Max Key len:", ln)

    code = conpressHtml(inputfile)
    replace(outputfile, code)
    print(f"---> END OF HTML FILE :{outputfile}--------------------")
    return 0


def main(argv: list[str] | None = None) -> int:
    try:
        return selesctApp(argv if argv is not None else sys.argv[1:])
    except Exception as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
else:
    run_platformio_pre_script()
