"""Write the requested round-2 summary, per-case evidence, and findings."""

from __future__ import annotations

import json
import statistics
import subprocess
from collections import Counter
from pathlib import Path


HERE = Path(__file__).resolve().parent
ROUND1 = HERE.parent / "juliet_run1"
RAW = json.loads((HERE / "raw_results.json").read_text(encoding="utf-8"))
ROWS = json.loads((HERE / "parsed_results.json").read_text(encoding="utf-8"))
REPEATS = json.loads((HERE / "parsed_repeats.json").read_text(encoding="utf-8"))
OLD = json.loads((ROUND1 / "parsed_results.json").read_text(encoding="utf-8"))
OLD_BY_KEY = {(row["case"], row["side"], row["up"]): row for row in OLD}

NOTES = {
    "int_rand_add_51": "bad 用 rand 来源做 int + 1；goodB2G 在加法前检查 data < INT_MAX。",
    "int_rand_multiply_52": "bad 用 rand 来源做 int × 2；goodB2G 在乘法前检查 data < INT_MAX/2。",
    "unsigned_int_rand_add_54": "bad 用 rand 来源做 unsigned int + 1；goodB2G 在加法前检查 data < UINT_MAX。",
    "short_rand_square_51": "bad 将 short 的平方存回 short；goodB2G 用 abs/sqrt 检查上界。",
    "int_rand_postinc_54": "bad 对 rand 来源执行 data++；goodB2G 在递增前检查 data < INT_MAX。",
    "int_rand_to_char_51": "bad 将 rand 来源的 int 转 char；goodG2B 从 CHAR_MAX-5 常量传入同类 sink。",
    "int_rand_to_char_54": "bad 将 rand 来源的 int 转 char；goodG2B 从 CHAR_MAX-5 常量传入同类 sink。",
    "int_rand_to_short_52": "bad 将 rand 来源的 int 转 short；goodG2B 从 CHAR_MAX-5 常量传入同类 sink。",
    "int_rand_to_short_54": "bad 将 rand 来源的 int 转 short；goodG2B 从 CHAR_MAX-5 常量传入同类 sink。",
    "short_rand_51": "bad 将 RAND32() 转 short，再在 sink 转 char；goodG2B 从 CHAR_MAX-5 常量传入。",
    "int_max_add_51": "bad 从 INT_MAX 常量传入，sink 做 +1；goodG2B 从常量 2 传入相同算术 sink。",
    "int_max_multiply_52": "bad 从 INT_MAX 常量传入，sink 做 ×2；goodG2B 从常量 2 传入相同算术 sink。",
    "int64_t_max_add_54": "bad 从 LLONG_MAX 常量跨四次调用传入，sink 做 i64 +1；goodG2B 从常量 2 传入。",
}


def unique(values: list[str]) -> list[str]:
    return list(dict.fromkeys(values))


def evidence(row: dict, field: str) -> list[str]:
    return unique([line for warning in row["target_warnings"] for line in warning[field]])


def source_outside(row: dict) -> bool:
    return (row["side"] == "good" and row["overall"] == "boundary-triggerable"
            and row["up"] < row["minimum_up"]
            and (row["cwe"] == 197 or row["constant_source"]))


def grade(row: dict) -> str:
    status = row["overall"]
    if status == "unknown":
        return "UNK"
    if source_outside(row):
        return "OUT_OF_SCOPE"
    if row["side"] == "bad":
        return "TP" if status == "boundary-triggerable" else "FN"
    return "TN" if status == "scoped-unreachable" else "FP"


def causes(row: dict) -> list[str]:
    if grade(row) in ("TP", "TN"):
        return []
    if source_outside(row):
        return ["其他（安全来源在深度范围外）"]
    text = "\n".join(evidence(row, "unmodeled") + evidence(row, "unexplored"))
    if "target call context cannot be expanded" in text:
        return ["上游上下文无法展开"]
    if "unmodeled return value from external call before target: rand" in text:
        return ["外部输入源未建模"]
    result = []
    if "unmodeled return value from external call" in text:
        result.append("间接或外部调用")
    if "unsupported value dependency" in text:
        result.append("不支持的指令")
    if "memory" in text.lower():
        result.append("内存未建模")
    if "budget" in text.lower() or "exhausted" in text.lower():
        result.append("路径或预算耗尽")
    return result or ["其他（无法从目标风险日志判断）"]


def quote_lines(lines: list[str]) -> str:
    return "```text\n" + "\n".join(lines) + "\n```\n" if lines else "无。\n"


def dependency_summary(row: dict) -> str:
    items = [line.strip().removeprefix("Unmodeled dependency: ") for line in evidence(row, "unmodeled")]
    if not items:
        return "无"
    text = "; ".join(items)
    return text if len(text) <= 80 else text[:77] + "..."


def manual_assessment(row: dict) -> str:
    status, side, minimum = row["overall"], row["side"], row["minimum_up"]
    if source_outside(row):
        return ("该 SAT 只证明自由边界可触发，good 常量来源尚未纳入；"
                "按第二轮规则记 OUT_OF_SCOPE，而非实际 good 误报。")
    if side == "bad" and status == "boundary-triggerable" and row["constant_source"] and row["up"] >= minimum:
        return ("源码常量已跨调用传至目标；日志的操作数和路径给出具体触发位模式。"
                "工具仍标 source-closed=false，不能改称 source-confirmed。")
    if side == "good" and status == "scoped-unreachable" and row["constant_source"]:
        return "安全常量 2 已进入选定范围，目标风险被排除；与 goodG2B 源码一致。"
    if side == "good" and status == "scoped-unreachable" and row["cwe"] == 197:
        return "安全常量来源已进入选定范围，目标缩窄风险被排除；与 goodG2B 源码一致。"
    if side == "good" and status == "scoped-unreachable":
        return "本地 guard 排除风险路径；与 goodB2G 源码一致。"
    if side == "bad" and status == "unknown":
        return "目标 sink 的局部风险存在，但 rand 返回值未建模；未确定有依据，不能算已证实的来源反例。"
    if side == "good" and status == "unknown":
        if "short_rand_square_51" in row["case"] and row["up"] == 0:
            return "guard 使用 abs、sqrt 与浮点转整数；未建模依赖阻止安全证明。"
        if row["case"].endswith("int_rand_add_51") and row["up"] == 1:
            return "扩大到第一跳后把 rand 未建模依赖纳入；本地 guard 原可排除风险，现退化为 unknown。"
        if row["case"].endswith("int_rand_add_51") and row["up"] >= 2:
            return "扩大到第二跳后调用上下文无法展开；安全结论进一步受到未探索路径限制。"
        return "未建模来源或调用上下文阻止安全证明；需要核对原文，不能当作真实可触发。"
    if side == "bad" and status == "boundary-triggerable":
        if "short_rand_square_51" in row["case"] and row["up"] == 0:
            return ("有效的源码风险是 short 平方后的 SignedTruncationLoss；"
                    "无符号回绕只是来源符号性未证实的候选。")
        return "目标风险在选定边界下可触发；若来源仍在边界外，此结论仍非完整来源证明。"
    return "无法判断；核对原始日志。"


def remedy(row: dict) -> str:
    why = causes(row)
    if "上游上下文无法展开" in why:
        return "修上下文展开及路径切片。"
    if "外部输入源未建模" in why:
        return "建模 rand/RAND32 外部输入及其可证明范围。"
    if "不支持的指令" in why:
        return "补浮点到整数指令以及 guard 使用的外部函数语义。"
    return "根据依赖清单进一步定位。"


def case_document(case: dict, rows: list[dict]) -> str:
    name = case["name"]
    short = name.split("__", 1)[1]
    lines = [f"# {name}", "", f"- 源文件：`{case['sources'][0]}` 等，共 {len(case['sources'])} 个。",
             f"- 源码差异：{NOTES[short]}",
             "- 目标：最后一级跨文件 sink 的 `data` 参数；CWE190 rand 用 goodB2G，CWE197 与常量组用 goodG2B。",
             f"- 最小充分深度：`up={case['minimum_up']}`（按预先固定的 51/52/54 调用结构）。",
             "- 以下风险等级只取目标 sink；上游运算候选可在原始日志查阅。", ""]
    for row in rows:
        risk = ", ".join(f"{warning['reason']}={warning['status']}" for warning in row["target_warnings"])
        command = subprocess.list2cmdline(row["command"])
        lines += [f"## {row['side']} / up={row['up']}", "",
                  f"- 日志：[原始 stdout/stderr](../{row['log']})。",
                  f"- 完整命令：`{command}`",
                  f"- 退出码：`{row['exit_code']}`；耗时：`{row['elapsed_ms']:.2f} ms`。",
                  f"- sink：`{row['sink']}`；参数：`data`；期望：{'应报' if row['side'] == 'bad' else '不应报'}。",
                  f"- 目标风险：{risk or '无候选'}；实际等级：`{row['overall']}`；判定：`{grade(row)}`。",
                  f"- 失败原因分类：{'；'.join(causes(row)) if causes(row) else '无'}。", "",
                  "**未建模依赖清单原文（同文去重）**", "", quote_lines(evidence(row, "unmodeled")),
                  "**未探索路径原文（同文去重）**", "", quote_lines(evidence(row, "unexplored")),
                  "**停止边界原文**", "", quote_lines(unique(row["depth_boundaries"] + row["verification_roots"])),
                  "**反例值、风险类别与路径摘要**", ""]
        sat = [warning for warning in row["target_warnings"] if warning["status"] == "boundary-triggerable"]
        if not sat:
            lines.append("- 无可采信反例值。")
        for warning in sat:
            values = unique(warning["boundary_input"] + warning["operands"] + warning["result"])
            path = unique(warning["path"][:3] + [line for line in warning["path"] if "Risk operation:" in line])
            lines += [f"- 类别：`{warning['reason']}`。", quote_lines(values),
                      "  路径摘要：", quote_lines(path)]
        lines += ["", f"**人工核对：**{manual_assessment(row)}", ""]
        if row["overall"] == "unknown":
            lines += [f"**最可能的解法：**{remedy(row)}", ""]
    if name == "CWE190_Integer_Overflow__int_rand_add_51":
        lines += ["## good 深度突变：报告原文", "",
                  "目标第一次在 `up=1` 从 scoped-unreachable 变为 unknown；"
                  "`up=2` 才第一次出现调用上下文未探索。以下为目标风险的原始行与对应依赖行：", ""]
        for up in (0, 1, 2, 3):
            row = next(item for item in rows if item["side"] == "good" and item["up"] == up)
            original = [warning["header"] for warning in row["target_warnings"]]
            original += evidence(row, "unmodeled") + evidence(row, "unexplored")
            lines += [f"**up={up}** — [日志](../{row['log']})", "", quote_lines(original)]
    if name == "CWE190_Integer_Overflow__short_rand_square_51":
        samples = [item["elapsed_ms"] for item in REPEATS]
        lines += ["## bad / up=0 五次重复测时", "",
                  "| 次数 | 耗时 ms | 等级 | 原始日志 |", "|---:|---:|---|---|"]
        for index, repeat in enumerate(REPEATS, 1):
            lines.append(f"| {index} | {repeat['elapsed_ms']:.2f} | `{repeat['overall']}` | [查看](../{repeat['log']}) |")
        lines += ["", f"最小 {min(samples):.2f} ms；中位数 {statistics.median(samples):.2f} ms；"
                  f"平均 {statistics.mean(samples):.2f} ms；最大 {max(samples):.2f} ms；"
                  f"样本标准差 {statistics.stdev(samples):.2f} ms。", ""]
    return "\n".join(lines) + "\n"


def main() -> None:
    assert len(RAW["cases"]) == 13 and len(ROWS) == 79 and len(REPEATS) == 5
    assert all(row["exit_code"] == 0 and row["target_warnings"] for row in ROWS)
    assert all(row["exit_code"] == 0 and row["target_warnings"] for row in REPEATS)
    by_case = {case["name"]: [row for row in ROWS if row["case"] == case["name"]]
               for case in RAW["cases"]}
    cases_dir = HERE / "cases"
    cases_dir.mkdir(exist_ok=True)
    for case in RAW["cases"]:
        (cases_dir / f"{case['name']}.md").write_text(case_document(case, by_case[case["name"]]), encoding="utf-8")
    baseline = [row for row in ROWS if (row["case"], row["side"], row["up"]) in OLD_BY_KEY]
    changes = [(row, OLD_BY_KEY[(row["case"], row["side"], row["up"])]) for row in baseline
               if row["overall"] != OLD_BY_KEY[(row["case"], row["side"], row["up"])]["overall"]
               or [(w["reason"], w["status"]) for w in row["target_warnings"]] !=
               [(w["reason"], w["status"]) for w in OLD_BY_KEY[(row["case"], row["side"], row["up"])]["target_warnings"]]]
    assert len(baseline) == 40
    at_minimum = [row for row in ROWS if row["up"] == row["minimum_up"]]
    stats = {label: Counter(grade(row) for row in subset)
             for label, subset in (("up=0", [row for row in ROWS if row["up"] == 0]),
                                   ("up=3", [row for row in ROWS if row["up"] == 3]),
                                   ("最小充分深度", at_minimum))}
    causes_count = Counter(cause for row in ROWS for cause in unique(causes(row)))
    summary = ["# Juliet 小样本试跑：第 2 轮", "", "## 环境与固定方法", "",
               f"- LLVM/Clang：`{RAW['clang_version']}`。",
               f"- ValueTrace：`{RAW['valuetrace_exe']}`；本地构建时间 `{RAW['valuetrace_build_local_time']}`；"
               f"可执行文件 SHA256 `{RAW['valuetrace_exe_sha256']}`。仓库没有 HEAD commit。",
               f"- 预处理脚本：`prepare-ir.ps1 -Source <同组 a/b/... .c> -Output <ir> -Link "
               "-Define OMITGOOD|OMITBAD -Extra '-IE:\\Juliet\\C\\testcasesupport'`。"
               "按 `.c` 执行 `clang++.exe -x c -std=c17 -O0 -g -Xclang -disable-O0-optnone -fno-inline`，"
               "再执行 `mem2reg,sroa`；具体编译命令和退出码在 [raw_results.json](raw_results.json) 与 `prep/`。",
               f"- `prepare-ir.cmake` SHA256 `{RAW['preprocessor_cmake_sha256']}`。"
               "原 10 组、bad/good sink 选择及 `--down 3` 不变；新 3 组常量用例固定选择 goodG2B。",
               "- verify 命令模板：`valuetrace.exe verify <IR> --function <最后一级sink> "
               "--param-name data --up <0|3|最小充分深度> --down 3 --report-level 5`；"
               "CWE190/int_rand_add_51 good 另跑 up=2。最小深度按 51/52/54 固定为 1/2/4。",
               "- 评分：bad 的 boundary-triggerable=TP，bad 其他非 unknown=FN；good 的 scoped-unreachable=TN，"
               "good 的 boundary-triggerable 且安全常量在深度外=OUT_OF_SCOPE，其他 good boundary-triggerable=FP；"
               "unknown=UNK。只计目标 sink；范围内上游其他警告保留在日志。", "",
               "## 结果总表", "",
               "| 用例名 | CWE | 变体 | bad/good | up | sink / 参数 | 期望 | 第一轮等级 | 本轮等级 | 判定 | 耗时 ms | 依赖清单摘要（≤80字） | 失败原因分类 |",
               "|---|---:|---:|---|---:|---|---|---|---|---|---:|---|---|"]
    for row in ROWS:
        old = OLD_BY_KEY.get((row["case"], row["side"], row["up"]))
        old_status = old["overall"] if old else "—"
        dep = dependency_summary(row).replace("|", "\\|")
        cause = "；".join(causes(row)) if causes(row) else "无"
        summary.append(f"| [{row['case']}](cases/{row['case']}.md) | {row['cwe']} | {row['variant']} | "
                       f"{row['side']} | {row['up']} | `{row['sink']}.data` | "
                       f"{'应报' if row['side'] == 'bad' else '不应报'} | `{old_status}` | "
                       f"`{row['overall']}` | {grade(row)} | {row['elapsed_ms']:.2f} | {dep} | {cause} |")
    summary += ["", "## 汇总统计", "",
                "| 范围 | TP | FN | TN | FP | UNK | OUT_OF_SCOPE | 合计 |",
                "|---|---:|---:|---:|---:|---:|---:|---:|"]
    for label, counts in stats.items():
        summary.append(f"| {label} | {counts['TP']} | {counts['FN']} | {counts['TN']} | {counts['FP']} | "
                       f"{counts['UNK']} | {counts['OUT_OF_SCOPE']} | {sum(counts.values())} |")
    summary += ["", f"第一轮固定配置：{len(baseline)} 次；目标风险类别／等级变化：{len(changes)} 次。"
                "评分口径变化另计，原始 `feasibility` 不受影响。", "",
                "失败原因出现次数（按运行去重，同一次可有多个原因）：", ""]
    for cause, count in causes_count.most_common():
        summary.append(f"- {cause}：{count} 次。")
    summary += ["", "## 固定深度检查", "",
                "| 用例 | bad 在最小深度 | good 在最小深度 |",
                "|---|---|---|"]
    for case in RAW["cases"]:
        subset = by_case[case["name"]]
        bad = next(row for row in subset if row["side"] == "bad" and row["up"] == case["minimum_up"])
        good = next(row for row in subset if row["side"] == "good" and row["up"] == case["minimum_up"])
        summary.append(f"| [{case['name']}](cases/{case['name']}.md) / up={case['minimum_up']} | "
                       f"`{bad['overall']}` ({grade(bad)}) | `{good['overall']}` ({grade(good)}) |")
    summary += ["", "## 单调性检查：up=0 与 up=3", "",
                "| 用例 | 方向 | up=0 | up=3 | 备注 |", "|---|---|---|---|---|"]
    for case in RAW["cases"]:
        for side in ("bad", "good"):
            subset = by_case[case["name"]]
            low = next(row for row in subset if row["side"] == side and row["up"] == 0)
            high = next(row for row in subset if row["side"] == side and row["up"] == 3)
            if low["overall"] == high["overall"]:
                continue
            highlight = "**安全结论退化**" if low["overall"] == "scoped-unreachable" and high["overall"] == "unknown" else "范围变化"
            summary.append(f"| [{case['name']}](cases/{case['name']}.md) | {side} | "
                           f"`{low['overall']}` | `{high['overall']}` | {highlight} |")
    samples = [item["elapsed_ms"] for item in REPEATS]
    summary += ["", "## short_rand_square_51 bad / up=0 重复测时", "",
                "| 次数 | 耗时 ms | 等级 | 日志 |", "|---:|---:|---|---|"]
    for index, repeat in enumerate(REPEATS, 1):
        summary.append(f"| {index} | {repeat['elapsed_ms']:.2f} | `{repeat['overall']}` | [查看]({repeat['log']}) |")
    summary += ["", f"最小 {min(samples):.2f}；中位数 {statistics.median(samples):.2f}；"
                f"平均 {statistics.mean(samples):.2f}；最大 {max(samples):.2f}；"
                f"样本标准差 {statistics.stdev(samples):.2f} ms。", ""]
    (HERE / "summary.md").write_text("\n".join(summary) + "\n", encoding="utf-8")

    bad_safe = [row for row in ROWS if row["side"] == "bad" and row["overall"] == "scoped-unreachable"]
    fp = [row for row in ROWS if grade(row) == "FP"]
    constant_min = [row for row in at_minimum if row["constant_source"]]
    c197_good_min = [row for row in at_minimum if row["cwe"] == 197 and row["side"] == "good"]
    representative = {reason: next(row for row in ROWS if reason in causes(row))
                      for reason in causes_count}
    first = by_case["CWE190_Integer_Overflow__int_rand_add_51"]
    one = next(row for row in first if row["side"] == "good" and row["up"] == 1)
    two = next(row for row in first if row["side"] == "good" and row["up"] == 2)
    first_dep = evidence(one, "unmodeled")
    second_dep = evidence(two, "unmodeled") + evidence(two, "unexplored")
    findings = ["# Juliet 第 2 轮评估结论", "",
                f"13 组、{len(ROWS)} 次固定配置 verify 与 5 次重复测时均完成，退出码均为 0。"
                f"第一轮相同的 40 个配置改按 C17 编译后，目标风险类别和等级变化 **{len(changes)} 次**；"
                "仅按新规则把安全来源在深度外的 good 从 FP 重标为 `OUT_OF_SCOPE`。",
                f"up=0：TP {stats['up=0']['TP']}、TN {stats['up=0']['TN']}、UNK {stats['up=0']['UNK']}、"
                f"OUT_OF_SCOPE {stats['up=0']['OUT_OF_SCOPE']}；"
                f"up=3：TP {stats['up=3']['TP']}、TN {stats['up=3']['TN']}、UNK {stats['up=3']['UNK']}、"
                f"OUT_OF_SCOPE {stats['up=3']['OUT_OF_SCOPE']}；"
                f"最小深度：TP {stats['最小充分深度']['TP']}、TN {stats['最小充分深度']['TN']}、"
                f"UNK {stats['最小充分深度']['UNK']}。三组的 FN、FP 均为 0。",
                f"CWE197 的 {len(c197_good_min)} 个 good 在最小充分深度全部为 `scoped-unreachable`；"
                "3 组常量来源在充分深度均为 bad `boundary-triggerable`、good `scoped-unreachable`。"
                "bad 日志给出常量操作数与跨文件路径，仍标 `source-closed=false`。", "",
                "## 最常见的三个失败原因", ""]
    for reason, count in causes_count.most_common(3):
        row = representative[reason]
        findings.append(f"- **{reason}：{count} 次。**代表：[日志]({row['log']})"
                        f"（{row['case']}，{row['side']}，up={row['up']}）。")
    findings += ["", "## 深度突变与严重结果", "",
                 "- `CWE190/int_rand_add_51` good 在 **up=1** 首次由 scoped-unreachable 变为 unknown；"
                 f"[up=1 日志]({one['log']}) 的相关原文：", "",
                 quote_lines(first_dep),
                 f"  [up=2 日志]({two['log']}) 才首次出现调用上下文未探索，原文：", "",
                 quote_lines(unique(second_dep)),
                 f"- good 真正落在范围内仍报 boundary-triggerable（FP）：{len(fp)} 次；"
                 f"bad 被判 scoped-unreachable：{len(bad_safe)} 次。"
                 "所有退出码为 0，未见超时或崩溃；除指定重复项外，同配置输出一致性无法判断。",
                 f"- `short_rand_square_51` bad/up=0 的 5 次耗时为 "
                 f"{', '.join(f'{value:.2f}' for value in samples)} ms；"
                 f"中位数 {statistics.median(samples):.2f} ms，"
                 f"范围 {min(samples):.2f}–{max(samples):.2f} ms。", "",
                 "## 下一步最值得修的三件事", "",
                 "1. 保持本地 guard 的排除结论在扩大上游时不被无关未建模项污染；"
                 f"[190/51 good up=0]({first[3]['log']}) 与 [up=1]({one['log']}) 是直接证据。",
                 "2. 修复上游调用上下文展开；"
                 f"[190/51 good up=2]({two['log']}) 和其他 `_51/_52` 的较深查询出现未探索路径。",
                 "3. 给 rand/RAND32 外部来源建立有依据的模型，同时保留来源未闭合标记；"
                 "CWE190/CWE197 的 rand bad 在充分深度普遍退为 unknown。", ""]
    (HERE / "findings.md").write_text("\n".join(findings), encoding="utf-8")
    print("CHANGES", len(changes), "STATS", {key: dict(value) for key, value in stats.items()})
    print("CAUSES", causes_count.most_common(), "FP", len(fp), "BAD_UNREACHABLE", len(bad_safe))
    print("REPEATS", samples, "CASE_FILES", len(list(cases_dir.glob("*.md"))))


if __name__ == "__main__":
    main()
