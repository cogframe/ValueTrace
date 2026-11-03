"""Juliet round 2: C frontend, fixed depths, constant sources, timing repeats."""

from __future__ import annotations

import hashlib
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

# First ten cases unchanged; the final three explicitly test constant propagation.
CASES = [
    (190, "s04", "int_rand_add_51", False),
    (190, "s04", "int_rand_multiply_52", False),
    (190, "s05", "unsigned_int_rand_add_54", False),
    (190, "s05", "short_rand_square_51", False),
    (190, "s06", "int_rand_postinc_54", False),
    (197, "s01", "int_rand_to_char_51", False),
    (197, "s01", "int_rand_to_char_54", False),
    (197, "s01", "int_rand_to_short_52", False),
    (197, "s01", "int_rand_to_short_54", False),
    (197, "s02", "short_rand_51", False),
    (190, "s03", "int_max_add_51", True),
    (190, "s03", "int_max_multiply_52", True),
    (190, "s02", "int64_t_max_add_54", True),
]


def execute(command: list[str], log: Path) -> dict:
    start = time.perf_counter()
    try:
        completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                                   encoding="utf-8", errors="replace")
        elapsed = round((time.perf_counter() - start) * 1000, 2)
        content = completed.stdout
        if completed.stderr:
            content += "\n[stderr]\n" + completed.stderr
        log.write_text(content, encoding="utf-8")
        return {"command": command, "log": log.relative_to(OUT).as_posix(),
                "exit_code": completed.returncode, "elapsed_ms": elapsed,
                "stderr_present": bool(completed.stderr)}
    except OSError as error:
        elapsed = round((time.perf_counter() - start) * 1000, 2)
        log.write_text(f"[launch error]\n{error}\n", encoding="utf-8")
        return {"command": command, "log": log.relative_to(OUT).as_posix(),
                "exit_code": None, "elapsed_ms": elapsed, "launch_error": str(error)}


def ps_quote(value: object) -> str:
    return "'" + str(value).replace("'", "''") + "'"


def checksum(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as file:
        for chunk in iter(lambda: file.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> None:
    for directory in (OUT / "logs", OUT / "ir", OUT / "prep"):
        directory.mkdir(parents=True, exist_ok=True)
    assert EXE.is_file() and (SUPPORT / "std_testcase.h").is_file()
    clang_version = subprocess.run([str(CLANG), "--version"], capture_output=True,
                                   text=True, encoding="utf-8", errors="replace").stdout.splitlines()[0]
    report = {
        "started_utc": datetime.now(timezone.utc).isoformat(),
        "clang_version": clang_version,
        "valuetrace_exe": str(EXE),
        "valuetrace_exe_sha256": checksum(EXE),
        "valuetrace_build_local_time": datetime.fromtimestamp(EXE.stat().st_mtime).isoformat(),
        "preprocessor_cmake_sha256": checksum(ROOT / "scripts" / "prepare-ir.cmake"),
        "cases": [], "repeats": [],
    }
    for cwe, suite, short_name, constant in CASES:
        category = "CWE190_Integer_Overflow" if cwe == 190 else "CWE197_Numeric_Truncation_Error"
        name = f"{category}__{short_name}"
        variant = short_name.rsplit("_", 1)[1]
        minimum = {"51": 1, "52": 2, "54": 4}[variant]
        final_letter = {"51": "b", "52": "c", "54": "e"}[variant]
        source_dir = TESTCASES / category / suite
        sources = sorted(source_dir.glob(f"{name}[a-e].c"))
        expected_count = {"51": 2, "52": 3, "54": 5}[variant]
        if len(sources) != expected_count:
            raise SystemExit(f"{name}: expected {expected_count} files, got {sources}")
        case = {"name": name, "cwe": cwe, "variant": variant,
                "constant_source": constant, "minimum_up": minimum,
                "sources": [str(source) for source in sources], "runs": []}
        report["cases"].append(case)
        for side in ("bad", "good"):
            macro = "OMITGOOD" if side == "bad" else "OMITBAD"
            suffix = "badSink" if side == "bad" else (
                "goodG2BSink" if constant or cwe == 197 else "goodB2GSink")
            sink = f"{name}{final_letter}_{suffix}"
            ir = OUT / "ir" / f"{name}__{side}.ll"
            expression = (f"& {ps_quote(PREPARE)} "
                          f"-Source {ps_quote(','.join(str(source) for source in sources))} "
                          f"-Output {ps_quote(ir)} -Link -Define {ps_quote(macro)} "
                          f"-Extra @({ps_quote('-I' + str(SUPPORT))})")
            prep = execute(["pwsh", "-NoProfile", "-Command", expression],
                           OUT / "prep" / f"{name}__{side}.log")
            case[f"{side}_prepare"] = prep
            print(f"PREP {name} {side}: exit={prep['exit_code']} {prep['elapsed_ms']} ms", flush=True)
            if prep["exit_code"] == 0:
                inspect = execute([str(EXE), "inspect", str(ir), "--function", sink],
                                  OUT / "prep" / f"{name}__{side}__inspect.log")
                case[f"{side}_inspect"] = inspect
                print(f"INSPECT {name} {side}: exit={inspect['exit_code']}", flush=True)
            depths = sorted({0, 3, minimum} | ({2} if name.endswith("int_rand_add_51") and side == "good" else set()))
            for up in depths:
                log = OUT / "logs" / f"{name}__{side}__up{up}.log"
                command = [str(EXE), "verify", str(ir), "--function", sink,
                           "--param-name", "data", "--up", str(up),
                           "--down", "3", "--report-level", "5"]
                if prep["exit_code"] == 0:
                    run = execute(command, log)
                else:
                    log.write_text(f"[not run: IR preparation failed]\nSee {prep['log']}\n", encoding="utf-8")
                    run = {"command": command, "log": log.relative_to(OUT).as_posix(),
                           "exit_code": None, "elapsed_ms": None}
                run.update({"side": side, "up": up, "sink": sink, "parameter": "data"})
                case["runs"].append(run)
                print(f"RUN {name} {side} up={up}: exit={run['exit_code']} "
                      f"{run['elapsed_ms']} ms", flush=True)
        (OUT / "raw_results.json").write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")
    name = "CWE190_Integer_Overflow__short_rand_square_51"
    case = next(item for item in report["cases"] if item["name"] == name)
    bad_sink = next(run["sink"] for run in case["runs"] if run["side"] == "bad")
    bad_ir = OUT / "ir" / f"{name}__bad.ll"
    for repeat in range(1, 6):
        command = [str(EXE), "verify", str(bad_ir), "--function", bad_sink,
                   "--param-name", "data", "--up", "0", "--down", "3", "--report-level", "5"]
        result = execute(command, OUT / "logs" / f"{name}__bad__up0__repeat{repeat}.log")
        result["repeat"] = repeat
        report["repeats"].append(result)
        print(f"REPEAT {repeat}: exit={result['exit_code']} {result['elapsed_ms']} ms", flush=True)
    report["finished_utc"] = datetime.now(timezone.utc).isoformat()
    (OUT / "raw_results.json").write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")


if __name__ == "__main__":
    main()
