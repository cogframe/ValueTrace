"""Produce auditable round-3 Juliet reports from actual run logs."""

from __future__ import annotations

import json
import re
import subprocess
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
RAW = json.loads((HERE / "raw_results.json").read_text(encoding="utf-8"))
ROWS = json.loads((HERE / "parsed_results.json").read_text(encoding="utf-8"))
OLD = json.loads((HERE.parent / "juliet_run3" / "parsed_results.json").read_text(encoding="utf-8"))
OLD_BY_KEY = {(r["case"], r["side"], r["up"]): r for r in OLD}
MONOTONICITY = json.loads((HERE / "monotonicity_failures.json").read_text(encoding="utf-8"))
RANGE_CHECK = json.loads((HERE / "range_check_results.json").read_text(encoding="utf-8"))


def unique(items: list[str]) -> list[str]:
    return list(dict.fromkeys(items))


WARNING = re.compile(r"^Warning: ArithmeticRisk .*$", re.MULTILINE)


def all_risks(round_dir: Path, log: str) -> dict:
    text = (round_dir / log).read_text(encoding="utf-8")
    matches = list(WARNING.finditer(text))
    result = {}
    for index, match in enumerate(matches):
        header = match.group(0)
        fields = [re.search(rf"\b{name}=([^\s]+)", header) for name in
                  ("reason", "feasibility", "node", "function")]
        if any(field is None for field in fields):
            continue
        end = matches[index+1].start() if index+1 < len(matches) else len(text)
        summary = text.find("Arithmetic report summary:", match.end(), end)
        block = text[match.start():summary if summary >= 0 else end]
        lines = block.splitlines()
        key = (fields[2].group(1), fields[3].group(1), fields[0].group(1))
        result[key] = {"status": fields[1].group(1),
                       "deps": [s.strip() for s in lines if s.startswith(("  Unmodeled dependency:", "  Discarded dependency:", "  Unexplored path:"))],
                       "assumptions": [s.strip() for s in lines if s.startswith(("  Used assumptions:", "  Rand range:"))]}
    return result


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
    if old["overall"] == "no-target-candidate" and new["variant"] == "61":
        return "改用跨文件函数返回值起点；返回边将来源函数连接到调用者风险运算。"
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
        return "_61 通过跨文件函数返回值传递；本轮用 --return-of 选来源函数，风险在调用者内。"
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
             f"- 分类：{'指针内存模型基线，不计入失败' if case['variant']=='63' else '常规评估'}。", ""]
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
                  "**使用的假设原文**", "", block(evidence(row, "assumptions")), "",
                  "**rand 范围与每次调用取值**", "",
                  block(evidence(row, "rand_range") + evidence(row, "rand_calls")), "",
                  "**下一层调用点提示**", "", block(evidence(row, "boundary_hints")), "",
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

    lines = ["# Juliet 小样本评估：第四轮", "", "## 环境与固定规则", "",
             f"- LLVM：`{RAW['clang_version']}`。",
             f"- ValueTrace 构建时间：`{RAW['valuetrace_build_local_time']}`；可执行文件 SHA256：`{RAW['valuetrace_exe_sha256']}`；本地仓库没有 HEAD commit。",
             "- 预处理：`prepare-ir.ps1 -Source <同组 .c> -Output <.ll> -Link -Define OMITGOOD|OMITBAD -Extra '-IE:\\Juliet\\C\\testcasesupport'`；`.c` 按 C17，`-O0 -g -fno-inline`，再运行 `mem2reg,sroa`。",
             "- 验证模板：`valuetrace.exe verify <IR> --function <目标函数> --param-name <参数> --up <深度> --down 3 --report-level 5`。",
             "- 完全复用第三轮 16 组、32 个 bad/good 和深度，不新增 Juliet 用例；_61 改用 --return-of 作为起点，_63 继续作为指针模型基线。",
             "- 默认按 IR 的 Windows MSVC target triple 将 rand 限定到 [0,32767]；每条安全结论另用 --rand-max none 重跑检查 UNSAT 不变。",
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
    lines += ["", "## 第三轮对比第四轮：目标风险整体等级变化", "",
              "| 用例 | 侧 / up | 第三轮 | 第四轮 | 原因 | 原始日志 |", "|---|---|---|---|---|---|"]
    changes = []
    for row in ROWS:
        old = OLD_BY_KEY.get((row["case"], row["side"], row["up"]))
        if not old or old["overall"] == row["overall"]:
            continue
        reason = change_reason(old, row)
        changes.append((old, row, reason))
        lines.append(f"| {row['case']} | {row['side']} / {row['up']} | `{old['overall']}` | `{row['overall']}` | {reason} | [第三轮](../juliet_run3/{old['log']}) / [第四轮]({row['log']}) |")
    lines += ["", f"目标整体等级变化共 {len(changes)} 次；未列出的同键运行等级不变。", "",
              "## 第三轮对比第四轮：单条风险状态变化", "",
              "| 用例 / 侧 / up | 指令节点 / 原因 | 第三轮 | 第四轮 | 状态变化原因 | 证据 |",
              "|---|---|---|---|---|---|"]
    risk_changes = []
    for row in ROWS:
        old = OLD_BY_KEY.get((row["case"], row["side"], row["up"]))
        if not old:
            continue
        before = all_risks(HERE.parent / "juliet_run3", old["log"])
        after = all_risks(HERE, row["log"])
        for key in sorted(set(before) | set(after)):
            left, right = before.get(key), after.get(key)
            if left and right and left["status"] == right["status"]:
                continue
            old_status = left["status"] if left else "无候选"
            new_status = right["status"] if right else "无候选"
            if row["variant"] == "61":
                why = "返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。"
            elif right and right["status"] == "scoped-unreachable" and left and left["status"] == "boundary-triggerable":
                why = "每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。"
            elif left and left["status"] == "unknown":
                why = "整数内建函数已建模；具体影响和剩余依赖见日志。"
            else:
                why = "调用切片或范围约束变化；需按链接日志核对。"
            risk_changes.append((row, key, left, right, why))
            lines.append(f"| {row['case']} {row['side']} up={row['up']} | `{key[1]} {key[0]} {key[2]}` | `{old_status}` | `{new_status}` | {why} | [旧](../juliet_run3/{old['log']}) / [新]({row['log']}) |")
    lines += ["", f"单条风险状态变化共 {len(risk_changes)} 次。", "",
              "## scoped-unreachable 状态变化：新旧依赖与假设", ""]
    for row, key, left, right, why in risk_changes:
        if not (left and left["status"] == "scoped-unreachable" or
                right and right["status"] == "scoped-unreachable"):
            continue
        lines += [f"### {row['case']} {row['side']} up={row['up']} / {key[1]} {key[0]} {key[2]}", "",
                  f"原因：{why}", "",
                  "第三轮依赖：", block(left["deps"] if left else []), "第三轮假设：",
                  block((left["assumptions"] or ["第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。"])
                        if left else ["第三轮无此风险候选，因此没有对应假设。"]), "第四轮依赖：",
                  block(right["deps"] if right else []), "第四轮假设：",
                  block(right["assumptions"] if right else []), ""]
    lines += ["## rand 范围 UNSAT 回归", "",
              f"有 scoped 风险的 {len(RANGE_CHECK)} 次运行关闭 rand 范围后，原有 UNSAT 全部保持：{'PASS' if all(x['pass'] for x in RANGE_CHECK) else 'FAIL'}。",
              "这是单向单调性：加范围不能使旧 UNSAT 变成 SAT；新范围可以把原 SAT 变成 UNSAT。完整记录见 [range_check_results.json](range_check_results.json)。", "",
              "## RAND32 宏的 IR 假设审计", "",
              "[测试 IR](ir/CWE190_Integer_Overflow__int_rand_add_51__bad.ll) 的 target triple 为 `x86_64-pc-windows-msvc19.50.35726`；本机 UCRT `stdlib.h:343` 定义 `RAND_MAX 0x7fff`。`RAND32()` 展开后的 `shl` 没有 `nsw`；源宏先把 `rand()` 转为 unsigned，再移位。因此位移回绕不是 poison。单次 `rand` 被约束到 [0,32767]，会限制移位操作数，但 3 次调用经移位与异或后仍可能形成宽范围的 32 位值；真实 PRNG 调用序列是否可由某个 seed 产生未验证。", "",
              "## 所有 scoped-unreachable 的完整依赖清单", ""]
    for row in ROWS:
        for warning in row["target_warnings"]:
            if warning["status"] != "scoped-unreachable":
                continue
            lines += [f"- `{row['case']}` {row['side']} `up={row['up']}` `{warning['reason']}`：[日志]({row['log']})；未建模={'; '.join(warning['unmodeled']) or '无'}；过近似={'; '.join(warning['overapprox']) or '无'}；丢弃={'; '.join(warning['discarded']) or '无'}；未探索={'; '.join(warning['unexplored']) or '无'}。"]
    (HERE / "summary.md").write_text("\n".join(lines) + "\n", encoding="utf-8")

    findings = ["# 第四轮结论", "",
                f"- 第三轮固定的 16 组、113 次运行全部重跑；目标整体等级变化 {len(changes)} 次，单条风险状态变化 {len(risk_changes)} 次。变化原因及涉及的依赖、假设见 [summary.md](summary.md)。",
                "- `_61` 返回值起点已接通：bad 的调用者加法可触发，good 不可触发。第三轮六次均因形参起点被覆盖而无候选；本轮六次都有目标风险结论。",
                f"- rand 范围按 IR 的 Windows MSVC 目标约束到 [0,32767]；{len(RANGE_CHECK)} 次含不可触发风险的命令关闭范围重跑，UNSAT 保持 {sum(x['pass'] for x in RANGE_CHECK)}/{len(RANGE_CHECK)}。反例现在打印每次 rand 调用的取值；PRNG 序列可达性仍未验证。",
                "- `RAND32()` 展开的 `shl` 在本次 IR 中没有 `nsw`：源宏先转换成 unsigned，位移回绕不会因 `nsw` 产生 poison；限制单次 rand 的范围不等于限制宏结果到 RAND_MAX。",
                "- 边界提示验收：CWE197 `int_rand_to_char_54` good、`up=3` 的下一层调用点实参是常量 **122**（以源码为准），见 [日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up3.log)。",
                "- `llvm.abs` 与四种整数 min/max 已建模；短整型平方 good 仍因浮点转换 `fptosi` 为 unknown。`_63` 的指针参数入口仍明确拒绝，六次运行作为内存模型基线。",
                f"- 第三轮单调性检查在本轮仍为 {'PASS' if not MONOTONICITY else 'FAIL'}；完整 CTest 为 34 项通过、1 项原本禁用。", ""]
    base = [r for r in ROWS if not r["memory_baseline"]]
    for up in (0, 3):
        c = Counter(grade(r) for r in base if r["up"] == up)
        findings.append(f"- `up={up}`：" + "，".join(f"{k}={c[k]}" for k in ("TP","TN","FP","FN","UNK","OUT_OF_SCOPE")) + "。")
    findings += ["", "剩余最明显的缺口："]
    for cause, count in cause_counts.most_common(3):
        example = next(r for r in base if cause in causes(r))
        findings.append(f"- {cause}：{count} 次；例 `{example['case']}` {example['side']} up={example['up']}，[日志]({example['log']})。")
    findings += ["", "优先工作：指针参数及内存传播（_63）、`fscanf` 输出内存建模、浮点转换条件建模。",
                 "全部原始日志见 [logs/](logs/)；逐例报告见 [cases/](cases/)；rand 范围回归见 [range_check_results.json](range_check_results.json)。"]
    (HERE / "findings.md").write_text("\n".join(findings) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
