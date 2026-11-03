"""Replay every scoped target with rand unbounded and require unchanged UNSAT."""

import json
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from parse_eval import parse_run  # noqa: E402


def main() -> None:
    rows = json.loads((HERE / "parsed_results.json").read_text(encoding="utf-8"))
    directory = HERE / "range_check"
    directory.mkdir(exist_ok=True)
    results = []
    failures = []
    for row in rows:
        expected = {w["reason"] for w in row["target_warnings"] if w["status"] == "scoped-unreachable"}
        if not expected:
            continue
        command = row["command"] + ["--rand-max", "none"]
        path = directory / f"{row['case']}__{row['side']}__up{row['up']}__unbounded.log"
        start = time.perf_counter()
        proc = subprocess.run(command, cwd=HERE.parents[1], capture_output=True, text=True,
                              encoding="utf-8", errors="replace")
        path.write_text(proc.stdout + ("\n[stderr]\n" + proc.stderr if proc.stderr else ""), encoding="utf-8")
        replay = parse_run({**row, "command": command, "log": path.relative_to(HERE).as_posix(),
                            "exit_code": proc.returncode,
                            "elapsed_ms": round((time.perf_counter() - start) * 1000, 2)})
        actual = {w["reason"] for w in replay["target_warnings"] if w["status"] == "scoped-unreachable"}
        item = {"case": row["case"], "side": row["side"], "up": row["up"],
                "bounded_scoped": sorted(expected), "unbounded_scoped": sorted(actual),
                "log": replay["log"], "exit_code": proc.returncode,
                "elapsed_ms": replay["elapsed_ms"], "pass": proc.returncode == 0 and expected <= actual}
        results.append(item)
        if not item["pass"]:
            failures.append(item)
        print(f"UNSAT {row['case']} {row['side']} up={row['up']}: {'PASS' if item['pass'] else 'FAIL'}", flush=True)
    (HERE / "range_check_results.json").write_text(json.dumps(results, indent=2), encoding="utf-8")
    if failures:
        raise SystemExit(f"rand UNSAT invariance FAIL: {len(failures)} of {len(results)} runs")
    print(f"rand UNSAT invariance PASS: {len(results)} runs", flush=True)


if __name__ == "__main__":
    main()
