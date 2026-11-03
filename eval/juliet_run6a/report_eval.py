"""Summarize fixed 6a replays, stage gates, memory provenance and regressions."""
from __future__ import annotations

import importlib.util
import json
import re
import subprocess
import time
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
EXE = ROOT / "build-msvc" / "valuetrace.exe"
spec = importlib.util.spec_from_file_location("replay", ROOT / "eval/juliet_run5a/run_eval.py")
replay = importlib.util.module_from_spec(spec)
spec.loader.exec_module(replay)


def save_run(command: list[str], path: Path) -> tuple[float, int]:
    path.parent.mkdir(parents=True, exist_ok=True)
    started = time.perf_counter()
    process = subprocess.run(command, cwd=ROOT, capture_output=True, text=True,
                             encoding="utf-8", errors="replace")
    path.write_text(process.stdout + ("\n[stderr]\n" + process.stderr if process.stderr else ""), encoding="utf-8")
    return round((time.perf_counter() - started) * 1000, 2), process.returncode


def warning_blocks(path: Path):
    text = path.read_text(encoding="utf-8")
    matches = list(replay.WARNING.finditer(text))
    for index, match in enumerate(matches):
        end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
        block = text[match.start():end]
        header = match.group(0)
        status = re.search(r"\bfeasibility=([^\s]+)", header)
        evidence = [line.strip().removeprefix("Memory evidence: ")
                    for line in block.splitlines() if line.strip().startswith("Memory evidence: ")
                    and not line.strip().startswith("Memory evidence: count=")]
        yield header, status.group(1) if status else "", evidence


def main() -> None:
    all_rows = {}
    for n in (3, 4):
        all_rows[n] = json.loads((HERE / f"round{n}/raw_results.json").read_text(encoding="utf-8"))["rows"]
        assert len(all_rows[n]) == 113, f"round {n} replay incomplete"
    regressions = []
    monotonic = []
    per_case = defaultdict(list)
    for n, rows in all_rows.items():
        for row in rows:
            for mode in ("ub-free", "wrap"):
                result = row[mode]
                if row["old_overall"] in ("boundary-triggerable", "scoped-unreachable") and result["overall"] != row["old_overall"]:
                    regressions.append((n, row, mode, result["overall"]))
                per_case[n, mode, row["case"], row["side"]].append(row)
    for (n, mode, case, side), rows in per_case.items():
        ordered = sorted(rows, key=lambda row: row["up"])
        for left, right in zip(ordered, ordered[1:]):
            if left[mode]["overall"] == "scoped-unreachable" and right[mode]["overall"] == "unknown":
                old_log = HERE / f"round{n}" / left[mode]["log"]
                new_log = HERE / f"round{n}" / right[mode]["log"]
                old_discarded = {line for line in old_log.read_text(encoding="utf-8").splitlines()
                                 if line.startswith("  Discarded dependency: ")}
                new_discarded = {line for line in new_log.read_text(encoding="utf-8").splitlines()
                                 if line.startswith("  Discarded dependency: ")}
                monotonic.append((n, mode, case, side, left["up"], right["up"], bool(new_discarded - old_discarded)))

    fourth = json.loads((ROOT / "eval/juliet_run4/parsed_results.json").read_text(encoding="utf-8"))
    stage = []
    for source in (row for row in fourth if row["case"] == "CWE190_Integer_Overflow__int_rand_add_63"):
        for mode in ("ub-free", "wrap"):
            path = HERE / "stage6a" / f"63_{source['side']}_up{source['up']}_{mode}.log"
            elapsed, code = save_run(source["command"] + ["--semantics", mode], path)
            result = replay.parse(path, source["sink"], source["side"], source["case"])
            expected = "boundary-triggerable" if source["side"] == "bad" else "scoped-unreachable"
            stage.append((source["side"], source["up"], mode, expected, result["overall"], elapsed,
                          path.relative_to(HERE).as_posix(), code))
    for name, function in (("01_pointer_output", "A"), ("07_escaped_global", "EscapeGlobal")):
        for mode in ("ub-free", "wrap"):
            path = HERE / "stage6a" / f"{name}_{mode}.log"
            command = [str(EXE), "verify", str(ROOT / f"build-msvc/fixtures/memory_acceptance/{name}.ll"),
                       "--function", function, "--param-name", "value", "--up", "0", "--down",
                       "2" if name.startswith("01") else "1", "--report-level", "5", "--semantics", mode]
            elapsed, code = save_run(command, path)
            statuses = [s for _, s, _ in warning_blocks(path)]
            actual = "boundary-triggerable" if "boundary-triggerable" in statuses else "unknown" if "unknown" in statuses else "no-candidate"
            expected = "boundary-triggerable" if name.startswith("01") else "unknown"
            stage.append((name, 0, mode, expected, actual, elapsed, path.relative_to(HERE).as_posix(), code))

    fscanf = [row for row in all_rows[4] if row["case"] == "CWE190_Integer_Overflow__int_fscanf_add_51"]
    fscanf_stronger = [(row["side"], row["up"], mode) for row in fscanf for mode in ("ub-free", "wrap")
                      if row["old_overall"] == "unknown" and row[mode]["overall"] != "unknown"]
    rand_checks = []
    for n, rows in all_rows.items():
        for row in rows:
            for mode in ("ub-free", "wrap"):
                expected = {w["key"] for w in row[mode]["target"] if w["status"] == "scoped-unreachable"}
                if not expected: continue
                log = HERE / f"round{n}/rand_range/{row['case']}__{row['side']}__up{row['up']}__{mode}.log"
                elapsed, code = save_run(row[mode]["command"] + ["--rand-max", "none"], log)
                actual = replay.parse(log, row["sink"], row["side"], row["case"])
                unbounded = {w["key"] for w in actual["target"] if w["status"] == "scoped-unreachable"}
                rand_checks.append((n, row["case"], row["side"], row["up"], mode, expected <= unbounded,
                                    elapsed, log.relative_to(HERE).as_posix(), code))
    (HERE / "rand_range_results.json").write_text(json.dumps(rand_checks, ensure_ascii=False, indent=2), encoding="utf-8")

    evidence_lines = ["# 6a 确定结论的内存证据", "", "按每次运行中的每条风险列出；`无` 表示该风险没有读取建模内存。", ""]
    evidence_count = 0
    for n, rows in all_rows.items():
        for row in rows:
            for mode in ("ub-free", "wrap"):
                log = HERE / f"round{n}" / row[mode]["log"]
                for header, status, evidence in warning_blocks(log):
                    if status not in ("scoped-unreachable", "boundary-triggerable"): continue
                    evidence_count += 1
                    reason = re.search(r"\breason=([^\s]+)", header)
                    node = re.search(r"\bnode=([^\s]+)", header)
                    identity = f"{reason.group(1) if reason else '?'} {node.group(1) if node else '?'}"
                    evidence_lines.append(f"- round{n} `{row['case']}` {row['side']} up={row['up']} `{mode}` `{status}` `{identity}`："
                                          + ("；".join(evidence) if evidence else "无（纯标量依赖）")
                                          + f"；[日志](round{n}/{row[mode]['log']})")
    for side, up, mode, expected, actual, elapsed, log, code in stage:
        for header, status, evidence in warning_blocks(HERE / log):
            if status not in ("scoped-unreachable", "boundary-triggerable"): continue
            evidence_count += 1
            reason = re.search(r"\breason=([^\s]+)", header)
            node = re.search(r"\bnode=([^\s]+)", header)
            identity = f"{reason.group(1) if reason else '?'} {node.group(1) if node else '?'}"
            evidence_lines.append(f"- 6a `{side}` up={up} `{mode}` `{status}` `{identity}`："
                                  + ("；".join(evidence) if evidence else "无") + f"；[日志]({log})")
    (HERE / "memory_evidence.md").write_text("\n".join(evidence_lines) + "\n", encoding="utf-8")

    counts = {n: Counter((mode, row[mode]["overall"]) for row in rows for mode in ("ub-free", "wrap"))
              for n, rows in all_rows.items()}
    lines = ["# 第五轮 6a：指针参数与解引用", "",
             "- 固定预期先写入 `examples/memory_acceptance/ACCEPTANCE.md`；仅运行 6a 用例。",
             "- CTest：35/35 个已启用测试通过；`memory_acceptance_later` 禁用。",
             "- 第三、四轮各 113 条固定命令，分别在 `ub-free` 与 `wrap` 下重跑；每轮 226 份日志。",
             "- 结果口径为目标风险的总体等级；内存证据逐条见 [memory_evidence.md](memory_evidence.md)。", "",
             "## 阶段验收表", "",
             "| 用例 | up | 模式 | 预期 | 实际 | 通过 | 耗时 ms | 日志 |",
             "|---|---:|---|---|---|---|---:|---|"]
    for side, up, mode, expected, actual, elapsed, log, code in stage:
        ok = actual == expected and code == 0
        lines.append(f"| {side} | {up} | `{mode}` | `{expected}` | `{actual}` | {'是' if ok else '否'} | {elapsed:.2f} | [日志]({log}) |")
    lines += ["", "## fscanf 阶段 6a 保护回归", "",
              "| bad/good | up | 第四轮 | ub-free | wrap | 日志 |", "|---|---:|---|---|---|---|"]
    for row in fscanf:
        lines.append(f"| {row['side']} | {row['up']} | `{row['old_overall']}` | `{row['ub-free']['overall']}` | "
                     f"`{row['wrap']['overall']}` | [默认](round4/{row['ub-free']['log']}) / [wrap](round4/{row['wrap']['log']}) |")
    lines += ["", "## 固定用例回归汇总", "",
              "| 来源 | 模式 | boundary-triggerable | scoped-unreachable | unknown | 其他 |",
              "|---|---|---:|---:|---:|---:|"]
    for n in (3, 4):
        for mode in ("ub-free", "wrap"):
            c = counts[n]
            lines.append(f"| 第{n}轮 | `{mode}` | {c[mode,'boundary-triggerable']} | {c[mode,'scoped-unreachable']} | "
                         f"{c[mode,'unknown']} | {113-c[mode,'boundary-triggerable']-c[mode,'scoped-unreachable']-c[mode,'unknown']} |")
    lines += ["", f"- 既有 TP/TN 退化：{len(regressions)} 条。",
              f"- `scoped-unreachable` 随 up 增大变 `unknown`：{len(monotonic)} 条，其中无新增丢弃依赖：{sum(not x[6] for x in monotonic)} 条。",
              f"- rand 有界/无界 UNSAT 复核：{sum(x[5] and x[8]==0 for x in rand_checks)}/{len(rand_checks)} 通过。",
              f"- 确定结论的内存证据记录：{evidence_count} 条。", ""]
    if regressions:
        lines += ["### TP/TN 退化", ""]
        for n, row, mode, actual in regressions:
            lines.append(f"- 第{n}轮 `{row['case']}` {row['side']} up={row['up']} `{mode}`："
                         f"`{row['old_overall']}` → `{actual}`；[日志](round{n}/{row[mode]['log']})")
    if monotonic:
        lines += ["### 深度单调性变化", ""]
        for n, mode, case, side, old_up, new_up, discarded in monotonic:
            lines.append(f"- 第{n}轮 `{case}` {side} `{mode}` up={old_up}→{new_up}："
                         f"{'有' if discarded else '无'}新增丢弃依赖。")
    failures = [s for s in stage if s[4] != s[3] or s[7] != 0]
    lines += ["", "## 判定", "",
              f"- 6a 新用例未通过：{len(failures)}；fscanf 原 unknown 变确定：{len(fscanf_stronger)}。", ""]
    (HERE / "summary.md").write_text("\n".join(lines), encoding="utf-8")
    finding = ["# 6a 结论", "",
               f"- 6a 验收：{len(stage)-len(failures)}/{len(stage)} 通过；CTest 35/35 通过，后续内存测试保持禁用。",
               f"- 第三、四轮双模式共 452 次运行；既有 TP/TN 退化 {len(regressions)} 次。",
               f"- 深度单调性异常 {sum(not x[6] for x in monotonic)} 次；rand UNSAT 复核失败 {sum(not x[5] or x[8]!=0 for x in rand_checks)} 次。",
               f"- fscanf 阶段 6a 的 unknown 增强为确定结论 {len(fscanf_stronger)} 次。",
               f"- 确定结论内存证据 {evidence_count} 条，详见 [memory_evidence.md](memory_evidence.md)。", ""]
    if regressions or failures or fscanf_stronger or any(not x[6] for x in monotonic):
        finding += ["未通过项保留在 [summary.md](summary.md)；需要按对应日志审计，不能视为阶段通过。", ""]
    (HERE / "findings.md").write_text("\n".join(finding), encoding="utf-8")
    print("reports written", "regressions", len(regressions), "stage failures", len(failures),
          "monotonic", len(monotonic), "rand", len(rand_checks), flush=True)
    if regressions or failures or fscanf_stronger or any(not x[6] for x in monotonic) or any(not x[5] or x[8] != 0 for x in rand_checks):
        raise SystemExit("6a acceptance or regression gate failed; inspect summary.md")


if __name__ == "__main__":
    main()
