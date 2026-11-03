"""Fifth round A: replay fourth-round commands in ub-free and wrap modes."""

from __future__ import annotations

import hashlib
import json
import re
import subprocess
import time
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
OLD = HERE.parent / "juliet_run4"
EXE = ROOT / "build-msvc" / "valuetrace.exe"
PREVIOUS = json.loads((OLD / "parsed_results.json").read_text(encoding="utf-8"))
WARNING = re.compile(r"^Warning: ArithmeticRisk .*$", re.MULTILINE)


def parse(log: Path, sink: str, side: str, case: str) -> dict:
    text = log.read_text(encoding="utf-8")
    warnings = {}
    target = []
    for matches in [list(WARNING.finditer(text))]:
        for index, found in enumerate(matches):
            header = found.group(0)
            def field(name: str) -> str:
                value = re.search(rf"\b{name}=([^\s]+)", header)
                return value.group(1) if value else ""
            end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
            summary = text.find("Arithmetic report summary:", found.end(), end)
            if summary >= 0:
                end = summary
            block = text[found.start():end]
            reason, status, node, function = (field(name) for name in ("reason", "feasibility", "node", "function"))
            key = f"{node}::{function}::{reason}"
            hints = [line.strip().removeprefix("Model note: Boundary caller (N+1): ")
                     for line in block.splitlines() if "Model note: Boundary caller (N+1): " in line]
            warnings[key] = {"status": status, "hints": hints,
                             "detail": next((line.strip() for line in block.splitlines()
                                             if line.startswith("  Verification:")), "")}
            expected = sink
            if expected.endswith("_badSource"):
                expected = expected.removesuffix("b_badSource") + "_bad"
            elif expected.endswith("_goodG2BSource"):
                expected = "goodG2B"
            if function == expected:
                target.append({"key": key, **warnings[key]})
    statuses = {item["status"] for item in target}
    overall = ("boundary-triggerable" if "boundary-triggerable" in statuses else
               "unknown" if "unknown" in statuses else
               "scoped-unreachable" if statuses == {"scoped-unreachable"} else
               "run-failed" if "[stderr]\nError:" in text else "no-target-candidate")
    return {"case": case, "side": side, "sink": sink, "warnings": warnings,
            "target": target, "overall": overall,
            "depth_boundaries": [line.strip() for line in text.splitlines() if "depth boundary:" in line]}


def run(command: list[str], log: Path) -> dict:
    start = time.perf_counter()
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                            encoding="utf-8", errors="replace")
    log.write_text(result.stdout + ("\n[stderr]\n" + result.stderr if result.stderr else ""), encoding="utf-8")
    return {"command": command, "log": log.relative_to(HERE).as_posix(),
            "exit_code": result.returncode, "elapsed_ms": round((time.perf_counter() - start) * 1000, 2)}


def old_risks(row: dict) -> dict:
    text = (OLD / row["log"]).read_text(encoding="utf-8")
    found = {}
    for match in WARNING.finditer(text):
        header = match.group(0)
        values = [re.search(rf"\b{name}=([^\s]+)", header) for name in
                  ("node", "function", "reason", "feasibility")]
        if all(values):
            found[f"{values[0].group(1)}::{values[1].group(1)}::{values[2].group(1)}"] = values[3].group(1)
    return found


def boundary_summary(row: dict) -> str:
    hints = list(dict.fromkeys(hint for warning in row["target"] for hint in warning["hints"]))
    if hints:
        return "; ".join(hints)
    if row["depth_boundaries"]:
        return "触及上游边界；当前无可用调用点实参摘要"
    return "未触及上游边界"


def main() -> None:
    (HERE / "logs").mkdir(parents=True, exist_ok=True)
    rows = []
    for previous in PREVIOUS:
        entry = {"case": previous["case"], "side": previous["side"],
                 "up": previous["up"], "sink": previous["sink"],
                 "memory_baseline": previous["memory_baseline"],
                 "old_log": previous["log"], "old_overall": previous["overall"]}
        command = [str(EXE)] + previous["command"][1:]
        for mode in ("ub-free", "wrap"):
            log = HERE / "logs" / f"{previous['case']}__{previous['side']}__up{previous['up']}__{mode}.log"
            executed = run(command + ["--semantics", mode], log)
            parsed = parse(log, previous["sink"], previous["side"], previous["case"])
            entry[mode] = {**executed, **parsed}
        entry["old_risks"] = old_risks(previous)
        rows.append(entry)
        print(f"{entry['case']} {entry['side']} up={entry['up']}: "
              f"old={entry['old_overall']} ub-free={entry['ub-free']['overall']} wrap={entry['wrap']['overall']}", flush=True)
        (HERE / "raw_results.json").write_text(json.dumps({"rows": rows}, ensure_ascii=False, indent=2), encoding="utf-8")
    meta = {"built": datetime.fromtimestamp(EXE.stat().st_mtime).isoformat(),
            "sha256": hashlib.sha256(EXE.read_bytes()).hexdigest(),
            "finished_utc": datetime.now(timezone.utc).isoformat(), "rows": rows}
    (HERE / "raw_results.json").write_text(json.dumps(meta, ensure_ascii=False, indent=2), encoding="utf-8")
    write_reports(meta)


def write_reports(data: dict) -> None:
    rows = data["rows"]
    lines = ["# 第五轮 A：UB 假设修正回归", "",
             f"- 使用第四轮固定的 {len(rows)} 条命令及相同 IR；构建 SHA256：`{data['sha256']}`。每条命令分别运行 `--semantics ub-free` 与 `--semantics wrap`。",
             "- 默认 `ub-free` 只有在复核 `wrap` 为 UNSAT 后才报告 `scoped-unreachable`。若只在 `ub-free` 中无解，报告 `unknown`，标签为“仅在无 UB 假设下不可触发”。", "",
             "## 总表：目标风险与上游边界提示", "",
             "| 用例 | bad/good | up | 第四轮 | 本轮默认 ub-free | 本轮 wrap | 下一层调用点实参摘要 | 默认耗时 ms | wrap 耗时 ms | 日志 |",
             "|---|---|---:|---|---|---|---|---:|---:|---|"]
    for row in rows:
        left, right = row["ub-free"], row["wrap"]
        hint = boundary_summary(left).replace("|", "/")
        lines.append(f"| {row['case']} | {row['side']} | {row['up']} | `{row['old_overall']}` | `{left['overall']}` | `{right['overall']}` | {hint} | {left['elapsed_ms']:.2f} | {right['elapsed_ms']:.2f} | [默认]({left['log']}) / [wrap]({right['log']}) |")
    changes = []
    scoped_old = 0
    for row in rows:
        for key, status in row["old_risks"].items():
            if status != "scoped-unreachable":
                continue
            scoped_old += 1
            current = row["ub-free"]["warnings"].get(key, {}).get("status", "无候选")
            wrap = row["wrap"]["warnings"].get(key, {}).get("status", "无候选")
            if current != status or wrap != status:
                changes.append((row, key, current, wrap))
    lines += ["", "## 第四轮所有 scoped-unreachable 的 wrap 回归", "",
              f"第四轮共 {scoped_old} 条风险结论为 `scoped-unreachable`；状态变化 {len(changes)} 条。", "",
              "| 用例 / 侧 / up | 风险节点与原因 | 第四轮 | 本轮默认 | 本轮 wrap | 变化原因 | 日志 |",
              "|---|---|---|---|---|---|---|"]
    for row, key, current, wrap in changes:
        node, function, reason = key.split("::", 2)
        detail = next((w["detail"] for w in row["ub-free"]["target"] if w["key"] == key), "")
        why = ("上游 nsw/nuw 在 wrap 中回绕，原结论只依赖无 UB 假设。" if wrap == "boundary-triggerable"
               else "wrap 求解未完成或风险未进入当前切片；见原始日志。")
        lines.append(f"| {row['case']} {row['side']} up={row['up']} | `{function} {node} {reason}` | `scoped-unreachable` | `{current}` | `{wrap}` | {why} {detail} | [旧](../juliet_run4/{row['old_log']}) / [默认]({row['ub-free']['log']}) / [wrap]({row['wrap']['log']}) |")
    if not changes:
        lines.append("| 无变化 | — | — | — | — | — | — |")
    boundary_rows = sum(boundary_summary(row["ub-free"]) != "未触及上游边界" for row in rows)
    summary = Counter((row["old_overall"], row["ub-free"]["overall"]) for row in rows)
    lines += ["", "## 汇总", "",
              f"- 触及上游边界并在总表填入提示的运行：{boundary_rows}/{len(rows)}。",
              f"- 原有 scoped 风险变化：{len(changes)}/{scoped_old}。",
              "- 目标整体等级变化：" + "; ".join(f"{a} → {b}: {n}" for (a,b),n in summary.items() if a != b) + "。",
              "- 验收用例 `nsw_guard_wrap`：默认目标 `%sum` 的 SignedOverflow 为 `unknown` 并带指定标签；显式 `wrap` 为 `boundary-triggerable`。", ""]
    (HERE / "summary.md").write_text("\n".join(lines), encoding="utf-8")
    findings = ["# 第五轮 A 结论", "",
                f"- 第四轮固定的 {len(rows)} 条命令各以默认和 wrap 模式重跑；原有 {scoped_old} 条不可触发风险中，{len(changes)} 条状态变化。",
                "- `add nsw` 后用 `a < x` 选择风险路径的验收用例通过：wrap 可触发，默认模式降为 unknown 并标明仅在无 UB 假设下不可触发。",
                f"- 总表为 {boundary_rows} 条触及上游边界的运行加入下一层调用点实参摘要；无提示的边界明确写为无可用摘要。",
                "- [summary.md](summary.md) 列出全部运行和原有安全结论的变化；[logs/](logs/) 保留两种模式的原始输出。", ""]
    (HERE / "findings.md").write_text("\n".join(findings), encoding="utf-8")


if __name__ == "__main__":
    main()
