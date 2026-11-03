"""Produce the requested Juliet reports from the fixed run and raw logs."""

from __future__ import annotations

import json
import subprocess
from collections import Counter
from pathlib import Path


HERE = Path(__file__).resolve().parent
RAW = json.loads((HERE / "raw_results.json").read_text(encoding="utf-8"))
ROWS = json.loads((HERE / "parsed_results.json").read_text(encoding="utf-8"))

CASE_NOTES = {
    "int_rand_add_51": "bad 的 rand 来源传到另一文件做 int + 1；goodB2G 在同一运算前检查 data < INT_MAX。",
    "int_rand_multiply_52": "bad 跨三文件传入后做 int × 2；goodB2G 在乘法前检查 data < INT_MAX/2。",
    "unsigned_int_rand_add_54": "bad 跨五文件传入后做 unsigned int + 1；goodB2G 检查 data < UINT_MAX。",
    "short_rand_square_51": "bad 将 short 的平方存回 short；goodB2G 使用 abs、sqrt 构成上界检查。",
    "int_rand_postinc_54": "bad 跨五文件传入后执行 data++；goodB2G 检查 data < INT_MAX。",
    "int_rand_to_char_51": "bad 将 rand 来源的 int 转成 char；goodG2B 用 CHAR_MAX-5 作来源，sink 仍有相同转换。",
    "int_rand_to_char_54": "bad 将跨五文件的 int 转成 char；goodG2B 用 CHAR_MAX-5 作来源，sink 仍有相同转换。",
    "int_rand_to_short_52": "bad 将跨三文件的 int 转成 short；goodG2B 用 CHAR_MAX-5 作来源，sink 仍有相同转换。",
    "int_rand_to_short_54": "bad 将跨五文件的 int 转成 short；goodG2B 用 CHAR_MAX-5 作来源，sink 仍有相同转换。",
    "short_rand_51": "bad 将 RAND32() 转成 short，再在 sink 转成 char；goodG2B 用 CHAR_MAX-5 作 short 来源。",
}


def grade(row: dict) -> str:
    side, status = row["side"], row["overall"]
    if status == "unknown":
        return "UNK"
    if side == "bad":
        return "TP" if status == "boundary-triggerable" else "FN"
    return "TN" if status == "scoped-unreachable" else "FP"


def causes(row: dict) -> list[str]:
    if grade(row) in ("TP", "TN"):
        return []
    dependencies = "\n".join(
        line for warning in row["target_warnings"]
        for line in warning["unmodeled"] + warning["unexplored"])
    if row["overall"] == "boundary-triggerable" and row["side"] == "good":
        return ["其他（安全来源在 --up 边界外）"]
    if "target call context cannot be expanded" in dependencies:
        return ["上游上下文无法展开"]
    if "unmodeled return value from external call before target: rand" in dependencies:
        return ["外部输入源未建模"]
    result = []
    if "unmodeled return value from external call" in dependencies:
        result.append("间接或外部调用")
    if "unsupported value dependency" in dependencies:
        result.append("不支持的指令")
    if "memory" in dependencies.lower():
        result.append("内存未建模")
    if "budget" in dependencies.lower() or "exhausted" in dependencies.lower():
        result.append("路径或预算耗尽")
    return result or ["其他（无法从目标风险日志判断）"]


def unique(items: list[str]) -> list[str]:
    return list(dict.fromkeys(items))


def exact_lines(row: dict, field: str) -> list[str]:
    return unique([line for warning in row["target_warnings"] for line in warning[field]])


def code_lines(lines: list[str]) -> str:
    if not lines:
        return "无。\n"
    return "```text\n" + "\n".join(lines) + "\n```\n"


def dependency_summary(row: dict) -> str:
    deps = exact_lines(row, "unmodeled")
    if not deps:
        return "无"
    fragments = [line.strip().removeprefix("Unmodeled dependency: ") for line in deps]
    summary = "; ".join(fragments)
    return summary if len(summary) <= 80 else summary[:77] + "..."


def manual_assessment(row: dict) -> str:
    side, status, cwe, up = row["side"], row["overall"], row["cwe"], row["up"]
    short = row["case"].split("__", 1)[1]
    if short == "short_rand_square_51" and side == "bad" and up == 0:
        return ("局部检出成立的是 `SignedTruncationLoss`：short 平方在提升后的 i32 中可表示，"
                "但存回 i16 可丢失信息。`UnsignedWrap` 另是未获源码符号性证实的候选，不能算该源码的无符号回绕证明。")
    if side == "bad" and status == "boundary-triggerable":
        return "目标 sink 的风险在当前自由边界下可触发；只证明边界条件，不证明 rand 或更上游真实来源可生成反例。"
    if side == "bad" and status == "unknown":
        return "局部风险已出现，但上游 rand 返回值未建模；未确定理由合理，不能把它记成未检出或已证实可达。"
    if side == "good" and status == "scoped-unreachable":
        return "目标运算受本地 guard 约束，范围内不可触发；与所选 goodB2G 路径一致。"
    if side == "good" and status == "boundary-triggerable" and cwe == 197:
        if up == 3 and row["variant"] == "54":
            return ("按指定评分为 FP。反例来自 54b 的自由边界，安全常量在 54a，"
                    "与最终 sink 相隔四次调用，未纳入 up=3；不能据此断言 good 源实际会触发。")
        return ("按指定评分为 FP。当前自由边界未包含 goodG2B 的安全常量来源；"
                "反例是边界输入，不证明 Juliet 的 good 执行可触发。")
    if side == "good" and status == "unknown":
        if short == "short_rand_square_51" and up == 0:
            return "本地 guard 使用 sqrt、llvm.abs 和 fptosi；当前模型不足，未确定理由合理。"
        return ("目标在 up=0 可被本地 guard 排除，扩大范围后因调用上下文未能展开变成 unknown；"
                "没有形成错误的安全证明，但这是需要修的稳定性退化。") if cwe == 190 else (
                "上游调用上下文未能展开，安全常量无法用于排除截断；未确定理由与日志一致。")
    return "无法判断；需要核对目标风险日志及源码路径。"


def likely_remedy(row: dict) -> str:
    reasons = causes(row)
    if "上游上下文无法展开" in reasons:
        return "修上下文展开。"
    if "外部输入源未建模" in reasons:
        return "建模外部输入源及其可用约束；必须保留来源未闭合的标记。"
    if "不支持的指令" in reasons:
        return "补 `fptosi` 等浮点到整数依赖，并处理相关外部函数语义。"
    return "无法判断。"


def case_file(case: dict, rows: list[dict]) -> str:
    name = case["name"]
    short = name.split("__", 1)[1]
    lines = [f"# {name}", "", f"- 源文件：`{case['sources'][0]}` 等，共 {len(case['sources'])} 个。",
             f"- 源码差异：{CASE_NOTES[short]}",
             "- 目标：各自最后一个跨文件 sink 的参数 `data`；bad 与 good 的完整函数名见每次命令。",
             "- 期望：bad 应报，good 不应报；good 路径在 CWE190 选 goodB2G，在 CWE197 选 goodG2B。",
             "- 说明：以下只给目标 sink 的风险分级；`up=3` 日志里的上游其他运算不计入本用例目标结论。", ""]
    for row in rows:
        command = subprocess.list2cmdline(row["command"])
        log_link = row["log"].replace("\\", "/")
        target = ", ".join(f"{warning['reason']}={warning['status']}" for warning in row["target_warnings"]) or "无目标候选"
        lines += [f"## {row['side']} / up={row['up']}", "",
                  f"- 原始日志：[查看](../{log_link})。",
                  f"- 完整命令：`{command}`",
                  f"- 退出码：`{row['exit_code']}`；耗时：`{row['elapsed_ms']:.2f} ms`。",
                  f"- 目标风险：{target}；综合等级：`{row['overall']}`；判定：`{grade(row)}`。",
                  f"- 失败原因分类：{'；'.join(causes(row)) if causes(row) else '无'}。", "",
                  "**未建模依赖清单原文（同文去重）**", "", code_lines(exact_lines(row, "unmodeled")),
                  "**未探索路径原文（同文去重）**", "", code_lines(exact_lines(row, "unexplored")),
                  "**停止边界原文**", "", code_lines(unique(row["depth_boundaries"] + row["verification_roots"])),
                  "**反例及路径**", ""]
        witnesses = [warning for warning in row["target_warnings"] if warning["status"] == "boundary-triggerable"]
        if witnesses:
            for witness in witnesses:
                value = witness["boundary_input"] or ["未给出参数值"]
                path = witness["path"]
                if path:
                    summary = unique(path[:3] + [line for line in path if "Risk operation:" in line])
                else:
                    summary = ["未给出路径"]
                lines += [f"- 风险类别 `{witness['reason']}`；参数值：`{' | '.join(value)}`。",
                          f"  路径摘要：`{' | '.join(summary)}`。"]
        else:
            lines.append("- 无可采信反例值。")
        lines += ["", f"**人工核对：**{manual_assessment(row)}", ""]
        if row["overall"] == "unknown":
            lines += [f"**最可能的解法：**{likely_remedy(row)}", ""]
    return "\n".join(lines)


def main() -> None:
    assert len(RAW["cases"]) == 10 and len(ROWS) == 40
    assert all(row["exit_code"] == 0 for row in ROWS), "A verify run failed; inspect raw logs"
    assert all(row["target_warnings"] for row in ROWS), "A target sink has no risk candidate"
    by_case = {case["name"]: [row for row in ROWS if row["case"] == case["name"]]
               for case in RAW["cases"]}
    cases_dir = HERE / "cases"
    cases_dir.mkdir(exist_ok=True)
    for case in RAW["cases"]:
        (cases_dir / f"{case['name']}.md").write_text(case_file(case, by_case[case["name"]]), encoding="utf-8")

    clang = RAW["clang_version"]
    build = RAW["valuetrace_build_local_time"]
    summary = ["# Juliet 小样本试跑：第 1 轮", "",
               "## 环境与固定方法", "",
               f"- LLVM/Clang：`{clang}`。",
               f"- ValueTrace：`{RAW['valuetrace_exe']}`；可执行文件本地构建时间：`{build}`。"
               " 仓库没有 HEAD commit（文件尚未纳入提交），故无法提供 commit。",
               "- 数据：`E:\\Juliet\\C\\testcases`；十组均为 `.c` 文件。",
               "- `prepare-ir.ps1`：对每组 a/b/... 文件分别编译两次，"
               "`-Source <该组全部源文件> -Output <ir路径> -Link -Define OMITGOOD|OMITBAD "
               "-Extra '-IE:\\Juliet\\C\\testcasesupport'`；脚本内部用 `clang++.exe -std=c++20 -O0 -g "
               "-Xclang -disable-O0-optnone -fno-inline`，之后 `mem2reg,sroa`。"
               " 每次具体编译命令及结果在 [raw_results.json](raw_results.json) 与 `prep/`。",
               "- 注意：所选为 Juliet 的 `.c` 源文件，但当前脚本固定使用 `clang++.exe`/C++20；"
               "本次衡量的是该编译方式得到的 IR，不能直接视为原生 C 模式测量。",
               "- verify 模板：`valuetrace.exe verify <bad或good.ll> --function <最后一级sink> "
               "--param-name data --up <0或3> --down 3 --report-level 5`。运行均从源码中预定的 sink 开始，"
               "未因结果修改深度、预算、用例或参数。",
               "- 评分依据：只取目标 sink 上与该 CWE 对应的风险；上游其他运算的警告保留在原始日志，"
               "但不用于此表等级。多个目标原因同时出现时，可触发优先、其次 unknown、全部排除才为 scoped-unreachable。",
               "- 固定判定：bad 的 boundary-triggerable 为 TP，bad 的非 unknown 未报为 FN；"
               "good 的 scoped-unreachable 为 TN，good 的 boundary-triggerable 为 FP；unknown 为 UNK。"
               "FP 是本轮评分标签，不等于已证实 good 的真实来源可触发。", "",
               "## 结果总表", "",
               "| 用例名 | CWE | 变体 | bad/good | up | sink / 参数 | 期望 | 实际等级 | 判定 | 耗时 ms | 依赖清单摘要（≤80字） | 失败原因分类 |",
               "|---|---:|---:|---|---:|---|---|---|---|---:|---|---|"]
    for row in ROWS:
        cwe, variant = row["cwe"], row["variant"]
        cause = "；".join(causes(row)) if causes(row) else "无"
        dep = dependency_summary(row).replace("|", "\\|")
        case_link = f"[<code>{row['case']}</code>](cases/{row['case']}.md)"
        summary.append(f"| {case_link} | {cwe} | {variant} | {row['side']} | {row['up']} | "
                       f"`{row['sink']}.data` | {'应报' if row['side'] == 'bad' else '不应报'} | "
                       f"`{row['overall']}` | {grade(row)} | {row['elapsed_ms']:.2f} | {dep} | {cause} |")
    stats = {up: Counter(grade(row) for row in ROWS if row["up"] == up) for up in (0, 3)}
    cause_counts = Counter(cause for row in ROWS for cause in dict.fromkeys(causes(row)))
    summary += ["", "## 汇总统计", "",
                "| up | TP | FN | TN | FP | UNK | 合计 |", "|---:|---:|---:|---:|---:|---:|---:|"]
    for up in (0, 3):
        stat = stats[up]
        summary.append(f"| {up} | {stat['TP']} | {stat['FN']} | {stat['TN']} | {stat['FP']} | {stat['UNK']} | {sum(stat.values())} |")
    summary += ["", "失败原因出现次数（按运行去重；同一次运行可计入多个原因）：", ""]
    for cause, count in cause_counts.most_common():
        summary.append(f"- {cause}：{count} 次。")
    summary += ["", "## 单调性检查", "",
                "以下列出同一用例、同一 bad/good 路径在 up=0 和 up=3 的目标等级变化。", "",
                "| 用例 | 方向 | up=0 | up=3 | 备注 |", "|---|---|---|---|---|"]
    for case in RAW["cases"]:
        for side in ("bad", "good"):
            zero = next(row for row in by_case[case["name"]] if row["side"] == side and row["up"] == 0)
            three = next(row for row in by_case[case["name"]] if row["side"] == side and row["up"] == 3)
            if zero["overall"] == three["overall"]:
                continue
            highlight = "**安全结论退化**" if zero["overall"] == "scoped-unreachable" and three["overall"] == "unknown" else "范围扩大后的变化"
            summary.append(f"| [{case['name']}](cases/{case['name']}.md) | {side} | "
                           f"`{zero['overall']}` | `{three['overall']}` | {highlight} |")
    (HERE / "summary.md").write_text("\n".join(summary) + "\n", encoding="utf-8")

    representatives = {}
    for cause, _ in cause_counts.most_common():
        representatives[cause] = next(row for row in ROWS if cause in causes(row))
    findings = ["# Juliet 第 1 轮评估结论", "",
                "固定 10 组、40 次 verify 均完成（退出码 0）。目标 sink 的分级统计："
                f"up=0 为 TP {stats[0]['TP']}、FN {stats[0]['FN']}、TN {stats[0]['TN']}、FP {stats[0]['FP']}、UNK {stats[0]['UNK']}；"
                f"up=3 为 TP {stats[3]['TP']}、FN {stats[3]['FN']}、TN {stats[3]['TN']}、FP {stats[3]['FP']}、UNK {stats[3]['UNK']}。", "",
                "## 最常见的三个失败原因", ""]
    for cause, count in cause_counts.most_common(3):
        example = representatives[cause]
        log_link = example["log"].replace("\\", "/")
        findings.append(f"- **{cause}：{count} 次。**例：[日志]({log_link})"
                        f"（{example['case']}，{example['side']}，up={example['up']}）。")
    findings += ["", "## 严重结果与解释", ""]
    fp_rows = [row for row in ROWS if grade(row) == "FP"]
    fn_unreachable = [row for row in ROWS if row["side"] == "bad" and row["overall"] == "scoped-unreachable"]
    findings.append(f"- good 被评为 FP：{len(fp_rows)} 次，全部在 CWE197。"
                    "[51 的 up=0](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up0.log)、"
                    "[54 的 up=3](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up3.log) 为代表。"
                    "这些 SAT 使用所选深度内的自由边界输入；54 的安全常量在第四次上游调用处，"
                    "因此这类 FP 不能解释为 good 的实际来源被证明可触发。")
    findings.append(f"- bad 被判 scoped-unreachable：{len(fn_unreachable)} 次。"
                    "若存在对应记录，应逐条复核；本轮没有。")
    findings.append("- CWE190 的 [51 good](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)"
                    "等在 up=0 已为 scoped-unreachable，up=3 却因调用上下文未能展开退为 unknown；"
                    "这是最明确的结果稳定性问题。")
    findings.append("- 工具崩溃或超时：未观察到；40 次退出码均为 0。"
                    "每组只执行一次，重复运行的一致性**无法判断**。所选样例为 `.c`，但当前脚本按 C++20 生成 IR。")
    findings += ["", "## 下一步最值得修的三件事", "",
                 "1. 修复 `target call context cannot be expanded within the selected data slice`；"
                 "[190/51 good](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)、"
                 "[197/52 good](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up3.log)"
                 "等 6 次 up=3 unknown 直接受其影响。",
                 "2. 给 `rand`/`RAND32()` 来源建立可审计的符号输入约束；"
                 "[190/52 bad](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log)、"
                 "[197/51 bad](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log)"
                 "等 6 次 up=3 unknown 因外部来源未建模。",
                 "3. 在报告中显式说明安全来源是否落在指定深度之外，并区分边界反例与完整 good 路径；"
                 "[197/54 good](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up3.log)"
                 "展示了边界被截在 54b，而安全常量在 54a。固定 up=3 时不能借此给出完整来源结论。", ""]
    (HERE / "findings.md").write_text("\n".join(findings), encoding="utf-8")
    print("STATS", {up: dict(stats[up]) for up in (0, 3)})
    print("CAUSES", cause_counts.most_common())
    print("WROTE", len(list(cases_dir.glob("*.md"))), "case reports")


if __name__ == "__main__":
    main()
