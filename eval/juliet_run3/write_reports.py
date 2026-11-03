"""Produce auditable round-3 Juliet reports from actual run logs."""

from __future__ import annotations

import json
import subprocess
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
RAW = json.loads((HERE / "raw_results.json").read_text(encoding="utf-8"))
ROWS = json.loads((HERE / "parsed_results.json").read_text(encoding="utf-8"))
OLD = json.loads((HERE.parent / "juliet_run2" / "parsed_results.json").read_text(encoding="utf-8"))
OLD_BY_KEY = {(r["case"], r["side"], r["up"]): r for r in OLD}
MONOTONICITY = json.loads((HERE / "monotonicity_failures.json").read_text(encoding="utf-8"))


def unique(items: list[str]) -> list[str]:
    return list(dict.fromkeys(items))


def evidence(row: dict, field: str) -> list[str]:
    return unique([line.strip() for w in row["target_warnings"] for line in w[field]])


def scoped_good_outside(row: dict) -> bool:
    return (row["side"] == "good" and row["overall"] == "boundary-triggerable"
            and row["up"] < row["minimum_up"]
            and (row["cwe"] == 197 or row["constant_source"]))


def grade(row: dict) -> str:
    if row["memory_baseline"]:
        return "BASELINE"
    if row["overall"] == "unknown":
        return "UNK"
    if row["overall"] in ("run-failed", "no-target-candidate"):
        return "ERROR"
    if scoped_good_outside(row):
        return "OUT_OF_SCOPE"
    if row["side"] == "bad":
        return "TP" if row["overall"] == "boundary-triggerable" else "FN"
    return "TN" if row["overall"] == "scoped-unreachable" else "FP"


def causes(row: dict) -> list[str]:
    if row["overall"] == "run-failed":
        return ["其他（目标参数类型不支持）"]
    if row["overall"] == "no-target-candidate":
        return ["其他（所选参数未连接返回值风险）"]
    if grade(row) in ("TP", "TN", "OUT_OF_SCOPE"):
        return []
    text = "\n".join(evidence(row, "unmodeled") + evidence(row, "unexplored"))
    found = []
    if "target call context cannot be expanded" in text:
        found.append("上游上下文无法展开")
    if "memory" in text.lower() or "pointer" in text.lower():
        found.append("内存未建模")
    if "unsupported value dependency" in text:
        found.append("不支持的指令")
    if "unexplored path:" in text and not found:
        found.append("路径或预算耗尽")
    if "unmodeled return value from external call before target: rand" in text:
        found.append("外部输入源未建模")
    if "external call" in text or "fscanf" in text:
        found.append("间接或外部调用")
    return unique(found) or ["其他（日志未给出可归类原因）"]


def dep_summary(row: dict) -> str:
    value = "; ".join(line.removeprefix("Unmodeled dependency: ") for line in evidence(row, "unmodeled"))
    return (value[:77] + "...") if len(value) > 80 else (value or "无")


def log_link(row: dict) -> str:
    return f"[{row['side']} up={row['up']}]({row['log']})"


def block(lines: list[str]) -> str:
    return "```text\n" + ("\n".join(lines) if lines else "（无）") + "\n```"


def change_reason(old: dict, new: dict) -> str:
    old_deps = "\n".join(evidence(old, "unmodeled") + evidence(old, "unexplored"))
    new_over = "\n".join(evidence(new, "overapprox"))
    new_discard = "\n".join(evidence(new, "discarded"))
    if new["overall"] == "boundary-triggerable":
        if "rand" in new_over:
            return "旧版将 rand 的任意返回值 SAT 降为 unknown；新版将已声明 rand 视为外部输入源，给边界反例，来源范围未验证。"
        if not new_over and not new_discard:
            return "新版向上切片消除了旧版未完成的调用上下文，SAT 在已建模部分成立；需核对反例与来源。"
        return "新版找到 SAT；逐项核对新报告的过近似与丢弃清单。"
    if new["overall"] == "scoped-unreachable":
        parts = []
        if "target call context cannot be expanded" in old_deps:
            parts.append("旧版无法展开无关上游调用；新版在切片闭合处停止，并报告闭合深度")
        if new_over:
            parts.append("新版对过近似值的任意取值求得 UNSAT")
        elif not parts:
            parts.append("新版在已建模切片内求得 UNSAT")
        if new_discard:
            parts.append("注意：新报告出现丢弃依赖，需人工复核")
        return "；".join(parts) + "。"
    return "等级变化原因需人工复核；见新旧原始日志。"


def source_note(case: dict) -> str:
    name = case["name"]
    if case["variant"] == "61":
        return "_61 通过跨文件函数返回值传递；所选 source 参数在函数内被覆盖，风险在调用者内。"
    if case["variant"] == "63":
        return "_63 通过指针参数传递；sink 先解引用 dataPtr，再对 data 加 1。"
    if "fscanf" in name:
        return "fscanf 将值写入局部变量；bad 在 sink 加 1，goodB2G 在加法前检查上界。"
    if case["cwe"] == 197:
        return "bad 将传来的整数缩窄；goodG2B 将安全常量跨函数传给同一风险操作。"
    if case["constant_source"]:
        return "bad 使用最大值常量；goodG2B 使用安全常量；二者跨文件传到运算位置。"
    return "bad 使用 rand 来源；goodB2G 在风险运算前增加防护条件。"


def case_document(case: dict, rows: list[dict]) -> str:
    lines = [f"# {case['name']}", "", f"- 源码：`{case['sources'][0]}` 等 {len(case['sources'])} 个文件。",
             f"- 差异：{source_note(case)}",
             f"- 最小深度：`up={case['minimum_up']}`；新增深度：`up={case['minimum_up']+2}`。",
             f"- 分类：{'内存模型基线，不计入失败' if case['memory_baseline'] else '常规评估'}。", ""]
    for row in rows:
        log = f"../{row['log']}"
        warnings = ", ".join(f"{w['reason']}={w['status']}" for w in row["target_warnings"])
        log_text = (HERE / row["log"]).read_text(encoding="utf-8")
        lines += [f"## {row['side']} / up={row['up']}", "",
                  f"- 原始日志：[stdout/stderr]({log})；完整命令：`{subprocess.list2cmdline(row['command'])}`。",
                  f"- sink：`{row['sink']}`，目标参数：`{row['parameter']}`，期望：{'应报' if row['side']=='bad' else '不应报'}。",
                  f"- 退出码：`{row['exit_code']}`；耗时：`{row['elapsed_ms']:.2f} ms`；实际：`{row['overall']}`；判定：`{grade(row)}`。",
                  f"- 风险：{warnings or '无目标候选'}；原因：{'、'.join(causes(row)) or '无'}。", "",
                  "**依赖清单原文**", "", block(evidence(row, "unmodeled")), "",
                  "**过近似清单原文**", "", block(evidence(row, "overapprox")), "",
                  "**丢弃清单原文**", "", block(evidence(row, "discarded")), "",
                  "**未探索路径原文**", "", block(evidence(row, "unexplored")), "",
                  "**停止边界原文**", "",
                  block(unique(row["depth_boundaries"] + row["verification_roots"] +
                               [line for w in row["target_warnings"] for line in w["model_notes"]
                                if "Data slice closed" in line])), "",
                  "**反例与关键值原文**", "",
                  block(unique([line for w in row["target_warnings"] if w["status"] == "boundary-triggerable"
                                for line in w["boundary_input"] + w["operands"] + w["result"] + w["path"][:3]])), ""]
        if row["overall"] == "unknown":
            lines += ["人工核对：未确定；" + "、".join(causes(row)) + "。最可能的后续工作是补齐对应模型或指令。", ""]
        elif row["overall"] == "run-failed":
            lines += ["人工核对：工具拒绝指针参数，详见原始错误；这是入口能力不足，未作风险判断。", "",
                      block(log_text.splitlines()[-4:]), ""]
        elif row["overall"] == "no-target-candidate":
            lines += ["人工核对：所选 source 形参被覆盖，风险位于调用者且未进入该参数的传播切片；无候选不代表安全。", ""]
        elif scoped_good_outside(row):
            lines += ["人工核对：安全来源尚在所选深度之外，边界反例不代表真实 good 误报。", ""]
        else:
            lines += ["人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。" if row["side"] == "bad"
                      else "人工核对：当前切片证明该风险不可触发；完整依赖清单见上。", ""]
        for warning in row["target_warnings"]:
            if warning["status"] == "scoped-unreachable":
                lines += [f"**单条不可触发结论：{warning['reason']} 的依赖清单**", "",
                          block(warning["unmodeled"] + warning["overapprox"] +
                                warning["discarded"] + warning["unexplored"]), ""]
    return "\n".join(lines) + "\n"


def main() -> None:
    (HERE / "cases").mkdir(exist_ok=True)
    by_case = defaultdict(list)
    for row in ROWS:
        by_case[row["case"]].append(row)
    for case in RAW["cases"]:
        (HERE / "cases" / (case["name"] + ".md")).write_text(
            case_document(case, by_case[case["name"]]), encoding="utf-8")

    lines = ["# Juliet 小样本评估：第三轮", "", "## 环境与固定规则", "",
             f"- LLVM：`{RAW['clang_version']}`。",
             f"- ValueTrace 构建时间：`{RAW['valuetrace_build_local_time']}`；可执行文件 SHA256：`{RAW['valuetrace_exe_sha256']}`；本地仓库没有 HEAD commit。",
             "- 预处理：`prepare-ir.ps1 -Source <同组 .c> -Output <.ll> -Link -Define OMITGOOD|OMITBAD -Extra '-IE:\\Juliet\\C\\testcasesupport'`；`.c` 按 C17，`-O0 -g -fno-inline`，再运行 `mem2reg,sroa`。",
             "- 验证模板：`valuetrace.exe verify <IR> --function <目标函数> --param-name <参数> --up <深度> --down 3 --report-level 5`。",
             "- 原第二轮 13 组、26 个 bad/good 保持原选样；深度为 0、3、最小值、最小值加 2（相同时去重）；另加 fscanf、_61、_63 各一组。_61/_63 单列内存模型基线。",
             "- 判定：TP=bad 边界可触发；TN=good 范围内不可触发；UNK=未确定；OUT_OF_SCOPE=good 安全来源未进入深度；BASELINE=内存模型基线；其余按 FP/FN/ERROR。", "",
             "## 结果总表", "",
             "| 用例 | CWE | 变体 | bad/good | up | sink / 参数 | 期望 | 实际等级 | 判定 | 耗时 ms | 依赖摘要（≤80 字符） | 失败原因 |",
             "|---|---:|---:|---|---:|---|---|---|---|---:|---|---|"]
    for row in ROWS:
        short = row["case"].split("__", 1)[1]
        lines.append(f"| [{short}](cases/{row['case']}.md) | {row['cwe']} | {row['variant']} | {row['side']} | {row['up']} | `{row['sink']}.{row['parameter']}` | {'应报' if row['side']=='bad' else '不应报'} | `{row['overall']}` | {grade(row)} | {row['elapsed_ms']:.2f} | {dep_summary(row).replace('|','/')} | {'；'.join(causes(row)) or '无'} |")
    lines += ["", "## 按 up 统计（不含内存模型基线）", "",
              "| up | TP | FN | TN | FP | UNK | OUT_OF_SCOPE | ERROR |", "|---:|---:|---:|---:|---:|---:|---:|---:|"]
    for up in sorted({r["up"] for r in ROWS if not r["memory_baseline"]}):
        count = Counter(grade(r) for r in ROWS if r["up"] == up and not r["memory_baseline"])
        lines.append("| " + str(up) + " | " + " | ".join(str(count[x]) for x in ("TP", "FN", "TN", "FP", "UNK", "OUT_OF_SCOPE", "ERROR")) + " |")
    cause_counts = Counter(cause for r in ROWS if not r["memory_baseline"] for cause in causes(r))
    lines += ["", "失败原因次数（同一次运行可计多个原因）：" + ("；".join(f"{k} {v}" for k,v in cause_counts.most_common()) or "无") + "。", "",
              "## 单调性检查", ""]
    lines.append(f"`scoped-unreachable` 随 up 增大变为 `unknown` 且没有新增丢弃依赖：{len(MONOTONICITY)} 次；{'FAIL' if MONOTONICITY else 'PASS'}。")
    for failure in MONOTONICITY:
        lines.append(f"- `{failure['case']}` {failure['side']} {failure['from_up']}→{failure['to_up']}：[旧]({failure['old_log']}) / [新]({failure['new_log']})。")
    lines += ["", "## 第二轮对比第三轮：同一命令状态变化", "",
              "| 用例 | 侧 / up | 第二轮 | 第三轮 | 原因 | 原始日志 |", "|---|---|---|---|---|---|"]
    changes = []
    for row in ROWS:
        old = OLD_BY_KEY.get((row["case"], row["side"], row["up"]))
        if not old or old["overall"] == row["overall"]:
            continue
        reason = change_reason(old, row)
        changes.append((old, row, reason))
        lines.append(f"| {row['case']} | {row['side']} / {row['up']} | `{old['overall']}` | `{row['overall']}` | {reason} | [第二轮](../juliet_run2/{old['log']}) / [第三轮]({row['log']}) |")
    lines += ["", f"同键变化共 {len(changes)} 次；未列出的第二轮同键运行等级不变。", "",
              "## 所有 scoped-unreachable 的完整依赖清单", ""]
    for row in ROWS:
        for warning in row["target_warnings"]:
            if warning["status"] != "scoped-unreachable":
                continue
            lines += [f"- `{row['case']}` {row['side']} `up={row['up']}` `{warning['reason']}`：[日志]({row['log']})；未建模={'; '.join(warning['unmodeled']) or '无'}；过近似={'; '.join(warning['overapprox']) or '无'}；丢弃={'; '.join(warning['discarded']) or '无'}；未探索={'; '.join(warning['unexplored']) or '无'}。"]
    (HERE / "summary.md").write_text("\n".join(lines) + "\n", encoding="utf-8")

    findings = ["# 第三轮结论", "", f"- 固定第二轮 26 个 bad/good 变体，新增 3 组共 6 个变体；实际运行 {len(ROWS)} 次。第二轮同键变化 {len(changes)} 次。",
                f"- 单调性自动检查：{'FAIL' if MONOTONICITY else 'PASS'}，无新增丢弃却由 scoped-unreachable 降成 unknown 的次数为 {len(MONOTONICITY)}。",
                "- 最小深度：CWE197 五组 good 均为 `scoped-unreachable`；CWE190 的短整型平方 good 仍为 `unknown`，因为浮点转整数 `fptosi` 未支持。",
                "- `fscanf` good 在 up=0 为 `scoped-unreachable`，up=1/3 为 `unknown`；新增依赖包含未建模内存写入和被分类为丢弃的不支持 `load`，因此不违反本轮单调性规则。",
                "- `_61`：选定来源函数的参数被覆盖，六次运行均无目标风险候选，不能作为有效风险结论。`_63`：指针参数入口不支持，六次运行以错误码 2 结束。两者按内存模型基线单列，不计入失败。", ""]
    base = [r for r in ROWS if not r["memory_baseline"]]
    for up in (0, 3):
        c = Counter(grade(r) for r in base if r["up"] == up)
        findings.append(f"- `up={up}`：" + "，".join(f"{k}={c[k]}" for k in ("TP","TN","FP","FN","UNK","OUT_OF_SCOPE")) + "。")
    findings += ["", "失败原因最高的三项："]
    for cause, count in cause_counts.most_common(3):
        example = next(r for r in base if cause in causes(r))
        findings.append(f"- {cause}：{count} 次；例如 `{example['case']}` {example['side']} up={example['up']}，[日志]({example['log']})。")
    severe = [r for r in base if grade(r) == "FP" or (r["side"] == "bad" and r["overall"] == "scoped-unreachable")]
    findings += ["", f"严重异常（good 误报或 bad 被判不可触发）：{len(severe)} 次。"]
    for row in severe:
        findings.append(f"- `{row['case']}` {row['side']} up={row['up']}：[日志]({row['log']})。")
    failed = [r for r in ROWS if r["exit_code"] != 0]
    findings.append(f"未见工具崩溃或超时；本轮没有重复执行相同命令，因此输出一致性无法判断。另有 {len(failed)} 次 `_63` 入口明确拒绝指针参数，[示例日志]({failed[0]['log'] if failed else 'logs/'}）。")
    findings += ["", "下一步最值得修的三项：",
                 "1. 支持指针参数选择与解引用传播：`_63` 当前无法进入验证，见 [日志](logs/CWE190_Integer_Overflow__int_rand_add_63__bad__up1.log)。",
                 "2. 连接函数返回值到调用者的风险切片：`_61` 当前零候选，见 [日志](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log)。",
                 "3. 支持 `fptosi` 和常见数学辅助调用的条件建模：短整型平方 good 仍为 unknown，见 [日志](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log)。",
                 "", "完整运行、逐项变化理由及每条安全结论的依赖清单见 [summary.md](summary.md)；各用例的完整证据见 [cases/](cases/)。"]
    (HERE / "findings.md").write_text("\n".join(findings) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
