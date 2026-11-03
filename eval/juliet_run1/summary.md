# Juliet 小样本试跑：第 1 轮

## 环境与固定方法

- LLVM/Clang：`clang version 23.1.4 (https://github.com/llvm/llvm-project 7a941dfb2877a0b8cf30323fefa85ad019e0b0f5)`。
- ValueTrace：`E:\ValueTrace\build-msvc\valuetrace.exe`；可执行文件本地构建时间：`2026-11-02T20:30:46.632470`。 仓库没有 HEAD commit（文件尚未纳入提交），故无法提供 commit。
- 数据：`E:\Juliet\C\testcases`；十组均为 `.c` 文件。
- `prepare-ir.ps1`：对每组 a/b/... 文件分别编译两次，`-Source <该组全部源文件> -Output <ir路径> -Link -Define OMITGOOD|OMITBAD -Extra '-IE:\Juliet\C\testcasesupport'`；脚本内部用 `clang++.exe -std=c++20 -O0 -g -Xclang -disable-O0-optnone -fno-inline`，之后 `mem2reg,sroa`。 每次具体编译命令及结果在 [raw_results.json](raw_results.json) 与 `prep/`。
- 注意：所选为 Juliet 的 `.c` 源文件，但当前脚本固定使用 `clang++.exe`/C++20；本次衡量的是该编译方式得到的 IR，不能直接视为原生 C 模式测量。
- verify 模板：`valuetrace.exe verify <bad或good.ll> --function <最后一级sink> --param-name data --up <0或3> --down 3 --report-level 5`。运行均从源码中预定的 sink 开始，未因结果修改深度、预算、用例或参数。
- 评分依据：只取目标 sink 上与该 CWE 对应的风险；上游其他运算的警告保留在原始日志，但不用于此表等级。多个目标原因同时出现时，可触发优先、其次 unknown、全部排除才为 scoped-unreachable。
- 固定判定：bad 的 boundary-triggerable 为 TP，bad 的非 unknown 未报为 FN；good 的 scoped-unreachable 为 TN，good 的 boundary-triggerable 为 FP；unknown 为 UNK。FP 是本轮评分标签，不等于已证实 good 的真实来源可触发。

## 结果总表

| 用例名 | CWE | 变体 | bad/good | up | sink / 参数 | 期望 | 实际等级 | 判定 | 耗时 ms | 依赖清单摘要（≤80字） | 失败原因分类 |
|---|---:|---:|---|---:|---|---|---|---|---:|---|---|
| [<code>CWE190_Integer_Overflow__int_rand_add_51</code>](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__int_rand_add_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 21.71 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__int_rand_add_51</code>](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__int_rand_add_51b_badSink.data` | 应报 | `unknown` | UNK | 129.91 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [<code>CWE190_Integer_Overflow__int_rand_add_51</code>](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 25.50 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__int_rand_add_51</code>](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `unknown` | UNK | 61.26 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [<code>CWE190_Integer_Overflow__int_rand_multiply_52</code>](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 0 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 23.70 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__int_rand_multiply_52</code>](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 3 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `unknown` | UNK | 271.57 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [<code>CWE190_Integer_Overflow__int_rand_multiply_52</code>](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 0 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 31.76 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__int_rand_multiply_52</code>](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 3 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `unknown` | UNK | 61.20 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [<code>CWE190_Integer_Overflow__unsigned_int_rand_add_54</code>](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 0 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 18.32 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__unsigned_int_rand_add_54</code>](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 3 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 19.05 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__unsigned_int_rand_add_54</code>](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 0 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 23.43 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__unsigned_int_rand_add_54</code>](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 3 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 22.31 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__short_rand_square_51</code>](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__short_rand_square_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 1163.77 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__short_rand_square_51</code>](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__short_rand_square_51b_badSink.data` | 应报 | `unknown` | UNK | 161.75 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [<code>CWE190_Integer_Overflow__short_rand_square_51</code>](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink.data` | 不应报 | `unknown` | UNK | 40.31 | risk depends on unmodeled control or operands before the target operation; un... | 间接或外部调用；不支持的指令 |
| [<code>CWE190_Integer_Overflow__short_rand_square_51</code>](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink.data` | 不应报 | `unknown` | UNK | 74.06 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [<code>CWE190_Integer_Overflow__int_rand_postinc_54</code>](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 0 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 21.44 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__int_rand_postinc_54</code>](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 3 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 22.20 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__int_rand_postinc_54</code>](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 0 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 25.90 | 无 | 无 |
| [<code>CWE190_Integer_Overflow__int_rand_postinc_54</code>](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 3 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 26.47 | 无 | 无 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_char_51</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 21.31 | 无 | 无 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_char_51</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data` | 应报 | `unknown` | UNK | 127.64 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_char_51</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data` | 不应报 | `boundary-triggerable` | FP | 19.08 | 无 | 其他（安全来源在 --up 边界外） |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_char_51</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data` | 不应报 | `unknown` | UNK | 15.50 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_char_54</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.94 | 无 | 无 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_char_54</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 18.85 | 无 | 无 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_char_54</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | FP | 20.54 | 无 | 其他（安全来源在 --up 边界外） |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_char_54</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | FP | 20.00 | 无 | 其他（安全来源在 --up 边界外） |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_short_52</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 19.46 | 无 | 无 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_short_52</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `unknown` | UNK | 133.12 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_short_52</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `boundary-triggerable` | FP | 18.10 | 无 | 其他（安全来源在 --up 边界外） |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_short_52</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `unknown` | UNK | 15.77 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_short_54</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 18.40 | 无 | 无 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_short_54</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 19.13 | 无 | 无 |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_short_54</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | FP | 19.22 | 无 | 其他（安全来源在 --up 边界外） |
| [<code>CWE197_Numeric_Truncation_Error__int_rand_to_short_54</code>](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | FP | 18.58 | 无 | 其他（安全来源在 --up 边界外） |
| [<code>CWE197_Numeric_Truncation_Error__short_rand_51</code>](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | bad | 0 | `CWE197_Numeric_Truncation_Error__short_rand_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 18.62 | 无 | 无 |
| [<code>CWE197_Numeric_Truncation_Error__short_rand_51</code>](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | bad | 3 | `CWE197_Numeric_Truncation_Error__short_rand_51b_badSink.data` | 应报 | `unknown` | UNK | 139.15 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [<code>CWE197_Numeric_Truncation_Error__short_rand_51</code>](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | good | 0 | `CWE197_Numeric_Truncation_Error__short_rand_51b_goodG2BSink.data` | 不应报 | `boundary-triggerable` | FP | 18.09 | 无 | 其他（安全来源在 --up 边界外） |
| [<code>CWE197_Numeric_Truncation_Error__short_rand_51</code>](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | good | 3 | `CWE197_Numeric_Truncation_Error__short_rand_51b_goodG2BSink.data` | 不应报 | `unknown` | UNK | 15.33 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |

## 汇总统计

| up | TP | FN | TN | FP | UNK | 合计 |
|---:|---:|---:|---:|---:|---:|---:|
| 0 | 10 | 0 | 4 | 5 | 1 | 20 |
| 3 | 4 | 0 | 2 | 2 | 12 | 20 |

失败原因出现次数（按运行去重；同一次运行可计入多个原因）：

- 其他（安全来源在 --up 边界外）：7 次。
- 外部输入源未建模：6 次。
- 上游上下文无法展开：6 次。
- 间接或外部调用：1 次。
- 不支持的指令：1 次。

## 单调性检查

以下列出同一用例、同一 bad/good 路径在 up=0 和 up=3 的目标等级变化。

| 用例 | 方向 | up=0 | up=3 | 备注 |
|---|---|---|---|---|
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | bad | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | good | `scoped-unreachable` | `unknown` | **安全结论退化** |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | bad | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | good | `scoped-unreachable` | `unknown` | **安全结论退化** |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | bad | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | bad | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | good | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | bad | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | good | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | bad | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | good | `boundary-triggerable` | `unknown` | 范围扩大后的变化 |
