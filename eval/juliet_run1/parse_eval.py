"""Extract target-sink evidence from run_eval.py's raw logs."""

from __future__ import annotations

import json
import re
from collections import Counter
from pathlib import Path


HERE = Path(__file__).resolve().parent
REPORT = json.loads((HERE / "raw_results.json").read_text(encoding="utf-8"))
WARNING = re.compile(r"^Warning: ArithmeticRisk .*", re.MULTILINE)


def parse_run(run: dict) -> dict:
    text = (HERE / run["log"]).read_text(encoding="utf-8")
    matches = list(WARNING.finditer(text))
    target: list[dict] = []
    for index, match in enumerate(matches):
        header = match.group(0)
        if f"function={run['sink']}(" not in header:
            continue
        end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
        summary = text.find("Arithmetic report summary:", match.end(), end)
        if summary >= 0:
            end = summary
        block = text[match.start():end]
        def field(name: str) -> str | None:
            found = re.search(rf"\b{name}=([^\s]+)", header)
            return found.group(1) if found else None
        def lines(prefix: str) -> list[str]:
            return [line for line in block.splitlines() if line.startswith(prefix)]
        target.append({
            "header": header,
            "reason": field("reason"),
            "status": field("feasibility"),
            "unmodeled": lines("  Unmodeled dependency:"),
            "unexplored": lines("  Unexplored path:"),
            "boundary_input": lines("  Boundary input "),
            "path": lines("  Path:"),
            "model_notes": lines("  Model note:"),
        })
    statuses = {warning["status"] for warning in target}
    if "boundary-triggerable" in statuses:
        overall = "boundary-triggerable"
    elif "unknown" in statuses:
        overall = "unknown"
    elif statuses == {"scoped-unreachable"}:
        overall = "scoped-unreachable"
    elif not statuses:
        overall = "no-target-candidate"
    else:
        overall = ",".join(sorted(statuses))
    boundary = [line for line in text.splitlines() if "depth boundary:" in line]
    root = [line for line in text.splitlines() if "Model note: Verification root:" in line]
    return {**run, "target_warnings": target, "overall": overall,
            "depth_boundaries": boundary, "verification_roots": root,
            "all_warning_count": len(matches),
            "candidate_line": next((line for line in text.splitlines()
                                    if line.startswith("Arithmetic verification:")), None)}


def main() -> None:
    rows = []
    for case in REPORT["cases"]:
        for run in case["runs"]:
            parsed = parse_run(run)
            parsed["case"] = case["name"]
            parsed["cwe"] = case["cwe"]
            parsed["variant"] = case["variant"]
            rows.append(parsed)
            reasons = ",".join(f"{w['reason']}:{w['status']}" for w in parsed["target_warnings"])
            deps = Counter(dep for w in parsed["target_warnings"] for dep in w["unmodeled"])
            print(f"{case['name']} {run['side']} up{run['up']}: {parsed['overall']} "
                  f"target={reasons or '-'} all={parsed['all_warning_count']} "
                  f"deps={'; '.join(deps)[:130]}")
    (HERE / "parsed_results.json").write_text(json.dumps(rows, ensure_ascii=False, indent=2),
                                                 encoding="utf-8")


if __name__ == "__main__":
    main()
