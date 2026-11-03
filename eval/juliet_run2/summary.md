# Juliet 小样本试跑：第 2 轮

## 环境与固定方法

- LLVM/Clang：`clang version 23.1.4 (https://github.com/llvm/llvm-project 7a941dfb2877a0b8cf30323fefa85ad019e0b0f5)`。
- ValueTrace：`E:\ValueTrace\build-msvc\valuetrace.exe`；本地构建时间 `2026-11-02T20:30:46.632470`；可执行文件 SHA256 `f43e8df1b629d49f91b94c7902ecdd73dced349ec6bb99b11c820a41bddf7a01`。仓库没有 HEAD commit。
- 预处理脚本：`prepare-ir.ps1 -Source <同组 a/b/... .c> -Output <ir> -Link -Define OMITGOOD|OMITBAD -Extra '-IE:\Juliet\C\testcasesupport'`。按 `.c` 执行 `clang++.exe -x c -std=c17 -O0 -g -Xclang -disable-O0-optnone -fno-inline`，再执行 `mem2reg,sroa`；具体编译命令和退出码在 [raw_results.json](raw_results.json) 与 `prep/`。
- `prepare-ir.cmake` SHA256 `921853aaa186d896c9f505ffc767f9ef769c0cd8faf8d5a3ddf045fcbe5e86ba`。原 10 组、bad/good sink 选择及 `--down 3` 不变；新 3 组常量用例固定选择 goodG2B。
- verify 命令模板：`valuetrace.exe verify <IR> --function <最后一级sink> --param-name data --up <0|3|最小充分深度> --down 3 --report-level 5`；CWE190/int_rand_add_51 good 另跑 up=2。最小深度按 51/52/54 固定为 1/2/4。
- 评分：bad 的 boundary-triggerable=TP，bad 其他非 unknown=FN；good 的 scoped-unreachable=TN，good 的 boundary-triggerable 且安全常量在深度外=OUT_OF_SCOPE，其他 good boundary-triggerable=FP；unknown=UNK。只计目标 sink；范围内上游其他警告保留在日志。

## 结果总表

| 用例名 | CWE | 变体 | bad/good | up | sink / 参数 | 期望 | 第一轮等级 | 本轮等级 | 判定 | 耗时 ms | 依赖清单摘要（≤80字） | 失败原因分类 |
|---|---:|---:|---|---:|---|---|---|---|---|---:|---|---|
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__int_rand_add_51b_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 23.75 | 无 | 无 |
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | bad | 1 | `CWE190_Integer_Overflow__int_rand_add_51b_badSink.data` | 应报 | `—` | `unknown` | UNK | 154.19 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__int_rand_add_51b_badSink.data` | 应报 | `unknown` | `unknown` | UNK | 153.75 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `scoped-unreachable` | `scoped-unreachable` | TN | 26.45 | 无 | 无 |
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 1 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `—` | `unknown` | UNK | 312.40 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 2 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `—` | `unknown` | UNK | 79.71 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `unknown` | `unknown` | UNK | 78.13 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 0 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 23.62 | 无 | 无 |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 2 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `—` | `unknown` | UNK | 315.63 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 3 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `unknown` | `unknown` | UNK | 311.11 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 0 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `scoped-unreachable` | `scoped-unreachable` | TN | 32.44 | 无 | 无 |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 2 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `—` | `unknown` | UNK | 456.63 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 3 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `unknown` | `unknown` | UNK | 81.45 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE190_Integer_Overflow__unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 0 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 20.21 | 无 | 无 |
| [CWE190_Integer_Overflow__unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 3 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 20.67 | 无 | 无 |
| [CWE190_Integer_Overflow__unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 4 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `—` | `unknown` | UNK | 147.85 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 0 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | `scoped-unreachable` | TN | 23.33 | 无 | 无 |
| [CWE190_Integer_Overflow__unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 3 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | `scoped-unreachable` | TN | 23.11 | 无 | 无 |
| [CWE190_Integer_Overflow__unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 4 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `—` | `unknown` | UNK | 295.51 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__short_rand_square_51b_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 1172.19 | 无 | 无 |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | bad | 1 | `CWE190_Integer_Overflow__short_rand_square_51b_badSink.data` | 应报 | `—` | `unknown` | UNK | 180.56 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__short_rand_square_51b_badSink.data` | 应报 | `unknown` | `unknown` | UNK | 184.00 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink.data` | 不应报 | `unknown` | `unknown` | UNK | 44.93 | risk depends on unmodeled control or operands before the target operation; un... | 间接或外部调用；不支持的指令 |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | good | 1 | `CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink.data` | 不应报 | `—` | `unknown` | UNK | 242.91 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink.data` | 不应报 | `unknown` | `unknown` | UNK | 90.09 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE190_Integer_Overflow__int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 0 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 22.47 | 无 | 无 |
| [CWE190_Integer_Overflow__int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 3 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 23.35 | 无 | 无 |
| [CWE190_Integer_Overflow__int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 4 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `—` | `unknown` | UNK | 157.63 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE190_Integer_Overflow__int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 0 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | `scoped-unreachable` | TN | 27.94 | 无 | 无 |
| [CWE190_Integer_Overflow__int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 3 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | `scoped-unreachable` | TN | 28.09 | 无 | 无 |
| [CWE190_Integer_Overflow__int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 4 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `—` | `unknown` | UNK | 327.43 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 19.72 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | bad | 1 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data` | 应报 | `—` | `unknown` | UNK | 150.59 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data` | 应报 | `unknown` | `unknown` | UNK | 152.53 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data` | 不应报 | `boundary-triggerable` | `boundary-triggerable` | OUT_OF_SCOPE | 19.91 | 无 | 其他（安全来源在深度范围外） |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | good | 1 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data` | 不应报 | `—` | `scoped-unreachable` | TN | 19.26 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data` | 不应报 | `unknown` | `unknown` | UNK | 16.09 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 20.05 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 19.26 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `—` | `unknown` | UNK | 152.46 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | `boundary-triggerable` | OUT_OF_SCOPE | 18.97 | 无 | 其他（安全来源在深度范围外） |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | `boundary-triggerable` | OUT_OF_SCOPE | 19.29 | 无 | 其他（安全来源在深度范围外） |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `—` | `scoped-unreachable` | TN | 19.03 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 19.43 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 2 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `—` | `unknown` | UNK | 156.91 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `unknown` | `unknown` | UNK | 157.21 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `boundary-triggerable` | `boundary-triggerable` | OUT_OF_SCOPE | 20.64 | 无 | 其他（安全来源在深度范围外） |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 2 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `—` | `scoped-unreachable` | TN | 19.26 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `unknown` | `unknown` | UNK | 16.97 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 19.28 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 19.76 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `—` | `unknown` | UNK | 156.31 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | `boundary-triggerable` | OUT_OF_SCOPE | 20.84 | 无 | 其他（安全来源在深度范围外） |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | `boundary-triggerable` | OUT_OF_SCOPE | 20.10 | 无 | 其他（安全来源在深度范围外） |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `—` | `scoped-unreachable` | TN | 20.60 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | bad | 0 | `CWE197_Numeric_Truncation_Error__short_rand_51b_badSink.data` | 应报 | `boundary-triggerable` | `boundary-triggerable` | TP | 19.75 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | bad | 1 | `CWE197_Numeric_Truncation_Error__short_rand_51b_badSink.data` | 应报 | `—` | `unknown` | UNK | 163.56 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | bad | 3 | `CWE197_Numeric_Truncation_Error__short_rand_51b_badSink.data` | 应报 | `unknown` | `unknown` | UNK | 165.31 | risk depends on unmodeled control or operands before the target operation; un... | 外部输入源未建模 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | good | 0 | `CWE197_Numeric_Truncation_Error__short_rand_51b_goodG2BSink.data` | 不应报 | `boundary-triggerable` | `boundary-triggerable` | OUT_OF_SCOPE | 20.52 | 无 | 其他（安全来源在深度范围外） |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | good | 1 | `CWE197_Numeric_Truncation_Error__short_rand_51b_goodG2BSink.data` | 不应报 | `—` | `scoped-unreachable` | TN | 19.11 | 无 | 无 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | good | 3 | `CWE197_Numeric_Truncation_Error__short_rand_51b_goodG2BSink.data` | 不应报 | `unknown` | `unknown` | UNK | 17.82 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE190_Integer_Overflow__int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__int_max_add_51b_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 20.31 | 无 | 无 |
| [CWE190_Integer_Overflow__int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | bad | 1 | `CWE190_Integer_Overflow__int_max_add_51b_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 18.04 | 无 | 无 |
| [CWE190_Integer_Overflow__int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__int_max_add_51b_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 18.46 | 无 | 无 |
| [CWE190_Integer_Overflow__int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink.data` | 不应报 | `—` | `boundary-triggerable` | OUT_OF_SCOPE | 24.07 | 无 | 其他（安全来源在深度范围外） |
| [CWE190_Integer_Overflow__int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | good | 1 | `CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink.data` | 不应报 | `—` | `scoped-unreachable` | TN | 20.27 | 无 | 无 |
| [CWE190_Integer_Overflow__int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink.data` | 不应报 | `—` | `unknown` | UNK | 17.13 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE190_Integer_Overflow__int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | bad | 0 | `CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 23.04 | 无 | 无 |
| [CWE190_Integer_Overflow__int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | bad | 2 | `CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 20.00 | 无 | 无 |
| [CWE190_Integer_Overflow__int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | bad | 3 | `CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 20.04 | 无 | 无 |
| [CWE190_Integer_Overflow__int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | good | 0 | `CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data` | 不应报 | `—` | `boundary-triggerable` | OUT_OF_SCOPE | 23.08 | 无 | 其他（安全来源在深度范围外） |
| [CWE190_Integer_Overflow__int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | good | 2 | `CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data` | 不应报 | `—` | `scoped-unreachable` | TN | 21.52 | 无 | 无 |
| [CWE190_Integer_Overflow__int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | good | 3 | `CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data` | 不应报 | `—` | `unknown` | UNK | 17.05 | unexplored path: target call context cannot be expanded within the selected d... | 上游上下文无法展开 |
| [CWE190_Integer_Overflow__int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | bad | 0 | `CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 24.40 | 无 | 无 |
| [CWE190_Integer_Overflow__int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | bad | 3 | `CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 23.63 | 无 | 无 |
| [CWE190_Integer_Overflow__int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | bad | 4 | `CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data` | 应报 | `—` | `boundary-triggerable` | TP | 18.26 | 无 | 无 |
| [CWE190_Integer_Overflow__int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | good | 0 | `CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data` | 不应报 | `—` | `boundary-triggerable` | OUT_OF_SCOPE | 24.84 | 无 | 其他（安全来源在深度范围外） |
| [CWE190_Integer_Overflow__int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | good | 3 | `CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data` | 不应报 | `—` | `boundary-triggerable` | OUT_OF_SCOPE | 24.27 | 无 | 其他（安全来源在深度范围外） |
| [CWE190_Integer_Overflow__int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | good | 4 | `CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data` | 不应报 | `—` | `scoped-unreachable` | TN | 19.42 | 无 | 无 |

## 汇总统计

| 范围 | TP | FN | TN | FP | UNK | OUT_OF_SCOPE | 合计 |
|---|---:|---:|---:|---:|---:|---:|---:|
| up=0 | 13 | 0 | 4 | 0 | 1 | 8 | 26 |
| up=3 | 7 | 0 | 2 | 0 | 14 | 3 | 26 |
| 最小充分深度 | 3 | 0 | 8 | 0 | 15 | 0 | 26 |

第一轮固定配置：40 次；目标风险类别／等级变化：0 次。评分口径变化另计，原始 `feasibility` 不受影响。

失败原因出现次数（按运行去重，同一次可有多个原因）：

- 外部输入源未建模：21 次。
- 其他（安全来源在深度范围外）：11 次。
- 上游上下文无法展开：9 次。
- 间接或外部调用：1 次。
- 不支持的指令：1 次。

## 固定深度检查

| 用例 | bad 在最小深度 | good 在最小深度 |
|---|---|---|
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) / up=1 | `unknown` (UNK) | `unknown` (UNK) |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) / up=2 | `unknown` (UNK) | `unknown` (UNK) |
| [CWE190_Integer_Overflow__unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) / up=4 | `unknown` (UNK) | `unknown` (UNK) |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) / up=1 | `unknown` (UNK) | `unknown` (UNK) |
| [CWE190_Integer_Overflow__int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) / up=4 | `unknown` (UNK) | `unknown` (UNK) |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) / up=1 | `unknown` (UNK) | `scoped-unreachable` (TN) |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) / up=4 | `unknown` (UNK) | `scoped-unreachable` (TN) |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) / up=2 | `unknown` (UNK) | `scoped-unreachable` (TN) |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) / up=4 | `unknown` (UNK) | `scoped-unreachable` (TN) |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) / up=1 | `unknown` (UNK) | `scoped-unreachable` (TN) |
| [CWE190_Integer_Overflow__int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) / up=1 | `boundary-triggerable` (TP) | `scoped-unreachable` (TN) |
| [CWE190_Integer_Overflow__int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) / up=2 | `boundary-triggerable` (TP) | `scoped-unreachable` (TN) |
| [CWE190_Integer_Overflow__int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) / up=4 | `boundary-triggerable` (TP) | `scoped-unreachable` (TN) |

## 单调性检查：up=0 与 up=3

| 用例 | 方向 | up=0 | up=3 | 备注 |
|---|---|---|---|---|
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | bad | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE190_Integer_Overflow__int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | good | `scoped-unreachable` | `unknown` | **安全结论退化** |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | bad | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE190_Integer_Overflow__int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | good | `scoped-unreachable` | `unknown` | **安全结论退化** |
| [CWE190_Integer_Overflow__short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | bad | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | bad | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | good | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | bad | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE197_Numeric_Truncation_Error__int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | good | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | bad | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE197_Numeric_Truncation_Error__short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | good | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE190_Integer_Overflow__int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | good | `boundary-triggerable` | `unknown` | 范围变化 |
| [CWE190_Integer_Overflow__int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | good | `boundary-triggerable` | `unknown` | 范围变化 |

## short_rand_square_51 bad / up=0 重复测时

| 次数 | 耗时 ms | 等级 | 日志 |
|---:|---:|---|---|
| 1 | 1176.85 | `boundary-triggerable` | [查看](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat1.log) |
| 2 | 1179.93 | `boundary-triggerable` | [查看](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat2.log) |
| 3 | 1170.67 | `boundary-triggerable` | [查看](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat3.log) |
| 4 | 1176.60 | `boundary-triggerable` | [查看](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat4.log) |
| 5 | 1171.65 | `boundary-triggerable` | [查看](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat5.log) |

最小 1170.67；中位数 1176.60；平均 1175.14；最大 1179.93；样本标准差 3.88 ms。

