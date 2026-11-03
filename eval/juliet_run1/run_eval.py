"""Fixed Juliet run 1. Evaluation harness only; does not modify ValueTrace."""

from __future__ import annotations

import json
import subprocess
import time
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
OUT = Path(__file__).resolve().parent
JULIET = Path(r"E:\Juliet\C")
SUPPORT = JULIET / "testcasesupport"
TESTCASES = JULIET / "testcases"
PREPARE = ROOT / "scripts" / "prepare-ir.ps1"
EXE = ROOT / "build-msvc" / "valuetrace.exe"
CLANG = ROOT / "third_party" / "llvm-23.1.4" / "bin" / "clang++.exe"

# Keep the user's ten choices and the already established CWE190 goodB2G pair.
CASES = [
    (190, "s04", "int_rand_add_51"),
    (190, "s04", "int_rand_multiply_52"),
    (190, "s05", "unsigned_int_rand_add_54"),
    (190, "s05", "short_rand_square_51"),
    (190, "s06", "int_rand_postinc_54"),
    (197, "s01", "int_rand_to_char_51"),
    (197, "s01", "int_rand_to_char_54"),
    (197, "s01", "int_rand_to_short_52"),
    (197, "s01", "int_rand_to_short_54"),
    (197, "s02", "short_rand_51"),
]


def execute(command: list[str], log: Path) -> dict:
    started = time.perf_counter()
    try:
        process = subprocess.run(command, capture_output=True, text=True,
                                 encoding="utf-8", errors="replace", cwd=ROOT)
        duration_ms = round((time.perf_counter() - started) * 1000, 2)
        # Preserve each captured stream verbatim; label stderr only if present.
        content = process.stdout
        if process.stderr:
            content += "\n[stderr]\n" + process.stderr
        log.write_text(content, encoding="utf-8")
        return {"command": command, "log": str(log.relative_to(OUT)),
                "exit_code": process.returncode, "elapsed_ms": duration_ms}
    except OSError as error:
        duration_ms = round((time.perf_counter() - started) * 1000, 2)
        log.write_text(f"[launch error]\n{error}\n", encoding="utf-8")
        return {"command": command, "log": str(log.relative_to(OUT)),
                "exit_code": None, "elapsed_ms": duration_ms,
                "launch_error": str(error)}


def ps_quote(value: object) -> str:
    return "'" + str(value).replace("'", "''") + "'"


def main() -> None:
    for directory in (OUT / "logs", OUT / "ir", OUT / "prep"):
        directory.mkdir(parents=True, exist_ok=True)
    if not (SUPPORT / "std_testcase.h").is_file() or not EXE.is_file():
        raise SystemExit("Juliet headers or current ValueTrace executable missing")
    clang_version = subprocess.run([str(CLANG), "--version"], capture_output=True,
                                   text=True, encoding="utf-8", errors="replace").stdout.splitlines()[0]
    report = {
        "started_utc": datetime.now(timezone.utc).isoformat(),
        "clang_version": clang_version,
        "valuetrace_exe": str(EXE),
        "valuetrace_build_local_time": datetime.fromtimestamp(EXE.stat().st_mtime).isoformat(),
        "cases": [],
    }
    for cwe, suite, short_name in CASES:
        category = "CWE190_Integer_Overflow" if cwe == 190 else "CWE197_Numeric_Truncation_Error"
        stem = f"{category}__{short_name}"
        variant = short_name.rsplit("_", 1)[1]
        case_id = f"{stem}"
        source_dir = TESTCASES / category / suite
        sources = sorted(source_dir.glob(f"{stem}[a-e].c"))
        expected_count = {"51": 2, "52": 3, "54": 5}[variant]
        if len(sources) != expected_count:
            raise SystemExit(f"{case_id}: expected {expected_count} files, found {sources}")
        final_letter = {"51": "b", "52": "c", "54": "e"}[variant]
        case = {"name": case_id, "cwe": cwe, "variant": variant,
                "sources": [str(source) for source in sources], "runs": []}
        report["cases"].append(case)
        for side in ("bad", "good"):
            macro = "OMITGOOD" if side == "bad" else "OMITBAD"
            sink_suffix = "badSink" if side == "bad" else (
                "goodB2GSink" if cwe == 190 else "goodG2BSink")
            sink = f"{stem}{final_letter}_{sink_suffix}"
            ir = OUT / "ir" / f"{case_id}__{side}.ll"
            prep_expression = (f"& {ps_quote(PREPARE)} "
                               f"-Source {ps_quote(','.join(str(source) for source in sources))} "
                               f"-Output {ps_quote(ir)} -Link -Define {ps_quote(macro)} "
                               f"-Extra @({ps_quote('-I' + str(SUPPORT))})")
            prep_command = ["pwsh", "-NoProfile", "-Command", prep_expression]
            prep = execute(prep_command, OUT / "prep" / f"{case_id}__{side}.log")
            case[f"{side}_prepare"] = prep
            print(f"PREP {case_id} {side}: exit={prep['exit_code']} {prep['elapsed_ms']} ms", flush=True)
            if prep["exit_code"] != 0:
                for up in (0, 3):
                    log = OUT / "logs" / f"{case_id}__{side}__up{up}.log"
                    log.write_text(f"[not run: IR preparation failed]\nSee {prep['log']}\n", encoding="utf-8")
                    case["runs"].append({"side": side, "up": up, "sink": sink,
                                         "parameter": "data", "log": str(log.relative_to(OUT)),
                                         "exit_code": None, "elapsed_ms": None})
                continue
            inspect_command = [str(EXE), "inspect", str(ir), "--function", sink]
            inspect = execute(inspect_command, OUT / "prep" / f"{case_id}__{side}__inspect.log")
            case[f"{side}_inspect"] = inspect
            print(f"INSPECT {case_id} {side}: exit={inspect['exit_code']}", flush=True)
            for up in (0, 3):
                log = OUT / "logs" / f"{case_id}__{side}__up{up}.log"
                command = [str(EXE), "verify", str(ir), "--function", sink,
                           "--param-name", "data", "--up", str(up),
                           "--down", "3", "--report-level", "5"]
                run = execute(command, log)
                run.update({"side": side, "up": up, "sink": sink, "parameter": "data"})
                case["runs"].append(run)
                print(f"RUN {case_id} {side} up={up}: exit={run['exit_code']} "
                      f"{run['elapsed_ms']} ms", flush=True)
        (OUT / "raw_results.json").write_text(json.dumps(report, ensure_ascii=False, indent=2),
                                                  encoding="utf-8")
    report["finished_utc"] = datetime.now(timezone.utc).isoformat()
    (OUT / "raw_results.json").write_text(json.dumps(report, ensure_ascii=False, indent=2),
                                              encoding="utf-8")


if __name__ == "__main__":
    main()
