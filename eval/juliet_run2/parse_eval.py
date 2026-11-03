"""Extract the target sink's warnings from each raw second-round log."""

from __future__ import annotations

import json
import re
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
        function = re.search(r"\bfunction=([^\s]+)", header)
        if not function or function.group(1).split("(", 1)[0] != run["sink"]:
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
            "operands": [line for line in block.splitlines() if line.startswith("  Operand operand")],
            "result": lines("  Result:"),
            "path": lines("  Path:"),
            "model_notes": lines("  Model note:"),
            "verification": lines("  Verification:"),
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
    lines = text.splitlines()
    return {**run, "target_warnings": target, "overall": overall,
            "depth_boundaries": [line for line in lines if "depth boundary:" in line],
            "verification_roots": [line for line in lines if "Model note: Verification root:" in line],
            "all_warning_count": len(matches),
            "candidate_line": next((line for line in lines if line.startswith("Arithmetic verification:")), None)}


def main() -> None:
    rows = []
    for case in REPORT["cases"]:
        for run in case["runs"]:
            row = parse_run(run)
            row.update({"case": case["name"], "cwe": case["cwe"],
                        "variant": case["variant"], "constant_source": case["constant_source"],
                        "minimum_up": case["minimum_up"]})
            rows.append(row)
            risks = ",".join(f"{w['reason']}:{w['status']}" for w in row["target_warnings"])
            print(f"{case['name']} {run['side']} up{run['up']}: {row['overall']} "
                  f"target={risks or '-'} all={row['all_warning_count']}")
    (HERE / "parsed_results.json").write_text(json.dumps(rows, ensure_ascii=False, indent=2), encoding="utf-8")
    repeat_rows = [parse_run({**item, "sink": next(run["sink"] for case in REPORT["cases"]
                             if case["name"] == "CWE190_Integer_Overflow__short_rand_square_51"
                             for run in case["runs"] if run["side"] == "bad")})
                   for item in REPORT["repeats"]]
    (HERE / "parsed_repeats.json").write_text(json.dumps(repeat_rows, ensure_ascii=False, indent=2), encoding="utf-8")


if __name__ == "__main__":
    main()
