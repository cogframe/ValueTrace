# Juliet 小样本评估：第四轮

## 环境与固定规则

- LLVM：`clang version 23.1.4 (https://github.com/llvm/llvm-project 7a941dfb2877a0b8cf30323fefa85ad019e0b0f5)`。
- ValueTrace 构建时间：`2026-11-03T00:29:29.818255`；可执行文件 SHA256：`e6811d8fbb897912c6da6d08c3acb00eb86672e0d8e941d8e88bb0f9864f8e5e`；本地仓库没有 HEAD commit。
- 预处理：`prepare-ir.ps1 -Source <同组 .c> -Output <.ll> -Link -Define OMITGOOD|OMITBAD -Extra '-IE:\Juliet\C\testcasesupport'`；`.c` 按 C17，`-O0 -g -fno-inline`，再运行 `mem2reg,sroa`。
- 验证模板：`valuetrace.exe verify <IR> --function <目标函数> --param-name <参数> --up <深度> --down 3 --report-level 5`。
- 完全复用第三轮 16 组、32 个 bad/good 和深度，不新增 Juliet 用例；_61 改用 --return-of 作为起点，_63 继续作为指针模型基线。
- 默认按 IR 的 Windows MSVC target triple 将 rand 限定到 [0,32767]；每条安全结论另用 --rand-max none 重跑检查 UNSAT 不变。
- 判定：TP=bad 边界可触发；TN=good 范围内不可触发；UNK=未确定；OUT_OF_SCOPE=good 安全来源未进入深度；BASELINE=内存模型基线；其余按 FP/FN/ERROR。

## 结果总表

| 用例 | CWE | 变体 | bad/good | up | sink / 参数 | 期望 | 实际等级 | 判定 | 耗时 ms | 依赖摘要（≤80 字符） | 失败原因 |
|---|---:|---:|---|---:|---|---|---|---|---:|---|---|
| [int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__int_rand_add_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 22.40 | 无 | 无 |
| [int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | bad | 1 | `CWE190_Integer_Overflow__int_rand_add_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 146.71 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__int_rand_add_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 151.02 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 23.67 | 无 | 无 |
| [int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 1 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 192.81 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 2 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 189.60 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_add_51](cases/CWE190_Integer_Overflow__int_rand_add_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 189.41 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 0 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 22.11 | 无 | 无 |
| [int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 2 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 197.67 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 3 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 195.16 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | bad | 4 | `CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 200.49 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 0 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 28.67 | 无 | 无 |
| [int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 2 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 270.92 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 3 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 266.28 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_multiply_52](cases/CWE190_Integer_Overflow__int_rand_multiply_52.md) | 190 | 52 | good | 4 | `CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 273.43 | unmodeled return value from external call before target: rand | 无 |
| [unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 0 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.03 | 无 | 无 |
| [unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 3 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.69 | 无 | 无 |
| [unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 4 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 145.02 | unmodeled return value from external call before target: rand | 无 |
| [unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | bad | 6 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 146.20 | unmodeled return value from external call before target: rand | 无 |
| [unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 0 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 22.04 | 无 | 无 |
| [unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 3 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 21.24 | 无 | 无 |
| [unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 4 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 194.47 | unmodeled return value from external call before target: rand | 无 |
| [unsigned_int_rand_add_54](cases/CWE190_Integer_Overflow__unsigned_int_rand_add_54.md) | 190 | 54 | good | 6 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 194.45 | unmodeled return value from external call before target: rand | 无 |
| [short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__short_rand_square_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 1177.17 | 无 | 无 |
| [short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | bad | 1 | `CWE190_Integer_Overflow__short_rand_square_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 2234.70 | unmodeled return value from external call before target: rand | 无 |
| [short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__short_rand_square_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 2243.31 | unmodeled return value from external call before target: rand | 无 |
| [short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink.data` | 不应报 | `unknown` | UNK | 1664.53 | unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35... | 不支持的指令 |
| [short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | good | 1 | `CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink.data` | 不应报 | `unknown` | UNK | 3677.23 | unmodeled return value from external call before target: rand; unsupported va... | 不支持的指令；外部输入源未建模；间接或外部调用 |
| [short_rand_square_51](cases/CWE190_Integer_Overflow__short_rand_square_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink.data` | 不应报 | `unknown` | UNK | 3671.53 | unmodeled return value from external call before target: rand; unsupported va... | 不支持的指令；外部输入源未建模；间接或外部调用 |
| [int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 0 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 19.95 | 无 | 无 |
| [int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 3 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 21.63 | 无 | 无 |
| [int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 4 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 150.26 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | bad | 6 | `CWE190_Integer_Overflow__int_rand_postinc_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 151.01 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 0 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 24.71 | 无 | 无 |
| [int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 3 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 24.54 | 无 | 无 |
| [int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 4 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 191.55 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_postinc_54](cases/CWE190_Integer_Overflow__int_rand_postinc_54.md) | 190 | 54 | good | 6 | `CWE190_Integer_Overflow__int_rand_postinc_54e_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 197.49 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 16.94 | 无 | 无 |
| [int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | bad | 1 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 149.75 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 150.08 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 16.95 | 无 | 无 |
| [int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | good | 1 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 15.78 | 无 | 无 |
| [int_rand_to_char_51](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_51.md) | 197 | 51 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 15.04 | 无 | 无 |
| [int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.42 | 无 | 无 |
| [int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.24 | 无 | 无 |
| [int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 153.26 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | bad | 6 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 148.80 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 17.37 | 无 | 无 |
| [int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 17.43 | 无 | 无 |
| [int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 16.16 | 无 | 无 |
| [int_rand_to_char_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_char_54.md) | 197 | 54 | good | 6 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 16.81 | 无 | 无 |
| [int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.84 | 无 | 无 |
| [int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 2 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 148.35 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 152.35 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | bad | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 149.61 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 16.90 | 无 | 无 |
| [int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 2 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 15.20 | 无 | 无 |
| [int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 16.66 | 无 | 无 |
| [int_rand_to_short_52](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_52.md) | 197 | 52 | good | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 15.96 | 无 | 无 |
| [int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.11 | 无 | 无 |
| [int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 18.84 | 无 | 无 |
| [int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 152.11 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | bad | 6 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 151.26 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 0 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 17.39 | 无 | 无 |
| [int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 16.85 | 无 | 无 |
| [int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 16.28 | 无 | 无 |
| [int_rand_to_short_54](cases/CWE197_Numeric_Truncation_Error__int_rand_to_short_54.md) | 197 | 54 | good | 6 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54e_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 15.67 | 无 | 无 |
| [short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | bad | 0 | `CWE197_Numeric_Truncation_Error__short_rand_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.87 | 无 | 无 |
| [short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | bad | 1 | `CWE197_Numeric_Truncation_Error__short_rand_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 164.42 | unmodeled return value from external call before target: rand | 无 |
| [short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | bad | 3 | `CWE197_Numeric_Truncation_Error__short_rand_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 164.99 | unmodeled return value from external call before target: rand | 无 |
| [short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | good | 0 | `CWE197_Numeric_Truncation_Error__short_rand_51b_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 17.79 | 无 | 无 |
| [short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | good | 1 | `CWE197_Numeric_Truncation_Error__short_rand_51b_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 16.05 | 无 | 无 |
| [short_rand_51](cases/CWE197_Numeric_Truncation_Error__short_rand_51.md) | 197 | 51 | good | 3 | `CWE197_Numeric_Truncation_Error__short_rand_51b_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 15.97 | 无 | 无 |
| [int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__int_max_add_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 19.98 | 无 | 无 |
| [int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | bad | 1 | `CWE190_Integer_Overflow__int_max_add_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 15.99 | 无 | 无 |
| [int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__int_max_add_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 15.94 | 无 | 无 |
| [int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 20.46 | 无 | 无 |
| [int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | good | 1 | `CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 17.93 | 无 | 无 |
| [int_max_add_51](cases/CWE190_Integer_Overflow__int_max_add_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 16.35 | 无 | 无 |
| [int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | bad | 0 | `CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 20.50 | 无 | 无 |
| [int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | bad | 2 | `CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 18.59 | 无 | 无 |
| [int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | bad | 3 | `CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.74 | 无 | 无 |
| [int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | bad | 4 | `CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data` | 应报 | `boundary-triggerable` | TP | 17.66 | 无 | 无 |
| [int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | good | 0 | `CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 20.85 | 无 | 无 |
| [int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | good | 2 | `CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 19.08 | 无 | 无 |
| [int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | good | 3 | `CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 18.72 | 无 | 无 |
| [int_max_multiply_52](cases/CWE190_Integer_Overflow__int_max_multiply_52.md) | 190 | 52 | good | 4 | `CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 19.87 | 无 | 无 |
| [int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | bad | 0 | `CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 21.26 | 无 | 无 |
| [int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | bad | 3 | `CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 20.85 | 无 | 无 |
| [int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | bad | 4 | `CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 16.45 | 无 | 无 |
| [int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | bad | 6 | `CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data` | 应报 | `boundary-triggerable` | TP | 16.12 | 无 | 无 |
| [int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | good | 0 | `CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 20.88 | 无 | 无 |
| [int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | good | 3 | `CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data` | 不应报 | `boundary-triggerable` | OUT_OF_SCOPE | 21.90 | 无 | 无 |
| [int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | good | 4 | `CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 16.23 | 无 | 无 |
| [int64_t_max_add_54](cases/CWE190_Integer_Overflow__int64_t_max_add_54.md) | 190 | 54 | good | 6 | `CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data` | 不应报 | `scoped-unreachable` | TN | 15.78 | 无 | 无 |
| [int_fscanf_add_51](cases/CWE190_Integer_Overflow__int_fscanf_add_51.md) | 190 | 51 | bad | 0 | `CWE190_Integer_Overflow__int_fscanf_add_51b_badSink.data` | 应报 | `boundary-triggerable` | TP | 20.97 | 无 | 无 |
| [int_fscanf_add_51](cases/CWE190_Integer_Overflow__int_fscanf_add_51.md) | 190 | 51 | bad | 1 | `CWE190_Integer_Overflow__int_fscanf_add_51b_badSink.data` | 应报 | `unknown` | UNK | 21.13 | memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_... | 内存未建模；不支持的指令；间接或外部调用 |
| [int_fscanf_add_51](cases/CWE190_Integer_Overflow__int_fscanf_add_51.md) | 190 | 51 | bad | 3 | `CWE190_Integer_Overflow__int_fscanf_add_51b_badSink.data` | 应报 | `unknown` | UNK | 20.95 | memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_... | 内存未建模；不支持的指令；间接或外部调用 |
| [int_fscanf_add_51](cases/CWE190_Integer_Overflow__int_fscanf_add_51.md) | 190 | 51 | good | 0 | `CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink.data` | 不应报 | `scoped-unreachable` | TN | 25.98 | 无 | 无 |
| [int_fscanf_add_51](cases/CWE190_Integer_Overflow__int_fscanf_add_51.md) | 190 | 51 | good | 1 | `CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink.data` | 不应报 | `unknown` | UNK | 25.54 | memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_... | 内存未建模；不支持的指令；间接或外部调用 |
| [int_fscanf_add_51](cases/CWE190_Integer_Overflow__int_fscanf_add_51.md) | 190 | 51 | good | 3 | `CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink.data` | 不应报 | `unknown` | UNK | 23.12 | memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_... | 内存未建模；不支持的指令；间接或外部调用 |
| [int_rand_add_61](cases/CWE190_Integer_Overflow__int_rand_add_61.md) | 190 | 61 | bad | 0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource.<return>` | 应报 | `boundary-triggerable` | TP | 147.45 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_add_61](cases/CWE190_Integer_Overflow__int_rand_add_61.md) | 190 | 61 | bad | 1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource.<return>` | 应报 | `boundary-triggerable` | TP | 151.39 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_add_61](cases/CWE190_Integer_Overflow__int_rand_add_61.md) | 190 | 61 | bad | 3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource.<return>` | 应报 | `boundary-triggerable` | TP | 151.08 | unmodeled return value from external call before target: rand | 无 |
| [int_rand_add_61](cases/CWE190_Integer_Overflow__int_rand_add_61.md) | 190 | 61 | good | 0 | `CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource.<return>` | 不应报 | `scoped-unreachable` | TN | 15.46 | 无 | 无 |
| [int_rand_add_61](cases/CWE190_Integer_Overflow__int_rand_add_61.md) | 190 | 61 | good | 1 | `CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource.<return>` | 不应报 | `scoped-unreachable` | TN | 16.09 | 无 | 无 |
| [int_rand_add_61](cases/CWE190_Integer_Overflow__int_rand_add_61.md) | 190 | 61 | good | 3 | `CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource.<return>` | 不应报 | `scoped-unreachable` | TN | 15.86 | 无 | 无 |
| [int_rand_add_63](cases/CWE190_Integer_Overflow__int_rand_add_63.md) | 190 | 63 | bad | 0 | `CWE190_Integer_Overflow__int_rand_add_63b_badSink.dataPtr` | 应报 | `run-failed` | BASELINE | 8.98 | 无 | 其他（目标参数类型不支持） |
| [int_rand_add_63](cases/CWE190_Integer_Overflow__int_rand_add_63.md) | 190 | 63 | bad | 1 | `CWE190_Integer_Overflow__int_rand_add_63b_badSink.dataPtr` | 应报 | `run-failed` | BASELINE | 9.10 | 无 | 其他（目标参数类型不支持） |
| [int_rand_add_63](cases/CWE190_Integer_Overflow__int_rand_add_63.md) | 190 | 63 | bad | 3 | `CWE190_Integer_Overflow__int_rand_add_63b_badSink.dataPtr` | 应报 | `run-failed` | BASELINE | 9.13 | 无 | 其他（目标参数类型不支持） |
| [int_rand_add_63](cases/CWE190_Integer_Overflow__int_rand_add_63.md) | 190 | 63 | good | 0 | `CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink.dataPtr` | 不应报 | `run-failed` | BASELINE | 9.98 | 无 | 其他（目标参数类型不支持） |
| [int_rand_add_63](cases/CWE190_Integer_Overflow__int_rand_add_63.md) | 190 | 63 | good | 1 | `CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink.dataPtr` | 不应报 | `run-failed` | BASELINE | 9.75 | 无 | 其他（目标参数类型不支持） |
| [int_rand_add_63](cases/CWE190_Integer_Overflow__int_rand_add_63.md) | 190 | 63 | good | 3 | `CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink.dataPtr` | 不应报 | `run-failed` | BASELINE | 9.54 | 无 | 其他（目标参数类型不支持） |

## 按 up 统计（不含内存模型基线）

| up | TP | FN | TN | FP | UNK | OUT_OF_SCOPE | ERROR |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | 15 | 0 | 6 | 0 | 1 | 8 | 0 |
| 1 | 6 | 0 | 5 | 0 | 3 | 0 | 0 |
| 2 | 3 | 0 | 4 | 0 | 0 | 0 | 0 |
| 3 | 14 | 0 | 10 | 0 | 3 | 3 | 0 |
| 4 | 8 | 0 | 8 | 0 | 0 | 0 | 0 |
| 6 | 5 | 0 | 5 | 0 | 0 | 0 | 0 |

失败原因次数（同一次运行可计多个原因）：不支持的指令 7；间接或外部调用 6；内存未建模 4；外部输入源未建模 2。

## 单调性检查

`scoped-unreachable` 随 up 增大变为 `unknown` 且没有新增丢弃依赖：0 次；PASS。

## 第三轮对比第四轮：目标风险整体等级变化

| 用例 | 侧 / up | 第三轮 | 第四轮 | 原因 | 原始日志 |
|---|---|---|---|---|---|
| CWE190_Integer_Overflow__int_rand_add_61 | bad / 0 | `no-target-candidate` | `boundary-triggerable` | 改用跨文件函数返回值起点；返回边将来源函数连接到调用者风险运算。 | [第三轮](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [第四轮](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 | bad / 1 | `no-target-candidate` | `boundary-triggerable` | 改用跨文件函数返回值起点；返回边将来源函数连接到调用者风险运算。 | [第三轮](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [第四轮](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 | bad / 3 | `no-target-candidate` | `boundary-triggerable` | 改用跨文件函数返回值起点；返回边将来源函数连接到调用者风险运算。 | [第三轮](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [第四轮](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 | good / 0 | `no-target-candidate` | `scoped-unreachable` | 改用跨文件函数返回值起点；返回边将来源函数连接到调用者风险运算。 | [第三轮](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__good__up0.log) / [第四轮](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 | good / 1 | `no-target-candidate` | `scoped-unreachable` | 改用跨文件函数返回值起点；返回边将来源函数连接到调用者风险运算。 | [第三轮](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__good__up1.log) / [第四轮](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 | good / 3 | `no-target-candidate` | `scoped-unreachable` | 改用跨文件函数返回值起点；返回边将来源函数连接到调用者风险运算。 | [第三轮](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__good__up3.log) / [第四轮](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up3.log) |

目标整体等级变化共 6 次；未列出的同键运行等级不变。

## 第三轮对比第四轮：单条风险状态变化

| 用例 / 侧 / up | 指令节点 / 原因 | 第三轮 | 第四轮 | 状态变化原因 | 证据 |
|---|---|---|---|---|---|
| CWE190_Integer_Overflow__int_rand_add_51 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_51_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_51 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_51_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_51 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_51_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_51 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_51_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_51 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_51_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_51 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_51_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_51 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_51_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_51 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_51_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=1 | `goodB2G #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=1 | `goodB2G #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=1 | `goodB2G #20 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=1 | `goodB2G #20 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=2 | `goodB2G #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=2 | `goodB2G #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=2 | `goodB2G #20 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=2 | `goodB2G #20 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=3 | `goodB2G #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=3 | `goodB2G #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=3 | `goodB2G #20 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_51 good up=3 | `goodB2G #20 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=2 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=2 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=2 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=2 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=3 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=3 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=3 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=3 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=4 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=4 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=4 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 bad up=4 | `CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=2 | `goodB2G #17 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=2 | `goodB2G #17 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=2 | `goodB2G #24 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=2 | `goodB2G #24 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=3 | `goodB2G #17 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=3 | `goodB2G #17 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=3 | `goodB2G #24 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=3 | `goodB2G #24 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=4 | `goodB2G #17 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=4 | `goodB2G #17 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=4 | `goodB2G #24 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log) |
| CWE190_Integer_Overflow__int_rand_multiply_52 good up=4 | `goodB2G #24 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=4 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=4 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=4 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=4 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=6 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up6.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up6.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=6 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up6.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up6.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=6 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up6.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up6.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=6 | `CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up6.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up6.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=4 | `goodB2G #19 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=4 | `goodB2G #19 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=4 | `goodB2G #26 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=4 | `goodB2G #26 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=6 | `goodB2G #19 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=6 | `goodB2G #19 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=6 | `goodB2G #26 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log) |
| CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=6 | `goodB2G #26 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log) / [新](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log) |
| CWE190_Integer_Overflow__short_rand_square_51 bad up=1 | `CWE190_Integer_Overflow__short_rand_square_51_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log) |
| CWE190_Integer_Overflow__short_rand_square_51 bad up=1 | `CWE190_Integer_Overflow__short_rand_square_51_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log) |
| CWE190_Integer_Overflow__short_rand_square_51 bad up=1 | `CWE190_Integer_Overflow__short_rand_square_51_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log) |
| CWE190_Integer_Overflow__short_rand_square_51 bad up=1 | `CWE190_Integer_Overflow__short_rand_square_51_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log) |
| CWE190_Integer_Overflow__short_rand_square_51 bad up=3 | `CWE190_Integer_Overflow__short_rand_square_51_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log) |
| CWE190_Integer_Overflow__short_rand_square_51 bad up=3 | `CWE190_Integer_Overflow__short_rand_square_51_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log) |
| CWE190_Integer_Overflow__short_rand_square_51 bad up=3 | `CWE190_Integer_Overflow__short_rand_square_51_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log) |
| CWE190_Integer_Overflow__short_rand_square_51 bad up=3 | `CWE190_Integer_Overflow__short_rand_square_51_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log) |
| CWE190_Integer_Overflow__short_rand_square_51 good up=1 | `goodB2G #16 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log) |
| CWE190_Integer_Overflow__short_rand_square_51 good up=1 | `goodB2G #16 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log) |
| CWE190_Integer_Overflow__short_rand_square_51 good up=1 | `goodB2G #23 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log) |
| CWE190_Integer_Overflow__short_rand_square_51 good up=1 | `goodB2G #23 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log) |
| CWE190_Integer_Overflow__short_rand_square_51 good up=3 | `goodB2G #16 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log) |
| CWE190_Integer_Overflow__short_rand_square_51 good up=3 | `goodB2G #16 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log) |
| CWE190_Integer_Overflow__short_rand_square_51 good up=3 | `goodB2G #23 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log) |
| CWE190_Integer_Overflow__short_rand_square_51 good up=3 | `goodB2G #23 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log) / [新](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 bad up=4 | `CWE190_Integer_Overflow__int_rand_postinc_54_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up4.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 bad up=4 | `CWE190_Integer_Overflow__int_rand_postinc_54_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up4.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 bad up=4 | `CWE190_Integer_Overflow__int_rand_postinc_54_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up4.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 bad up=4 | `CWE190_Integer_Overflow__int_rand_postinc_54_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up4.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 bad up=6 | `CWE190_Integer_Overflow__int_rand_postinc_54_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up6.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up6.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 bad up=6 | `CWE190_Integer_Overflow__int_rand_postinc_54_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up6.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up6.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 bad up=6 | `CWE190_Integer_Overflow__int_rand_postinc_54_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up6.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up6.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 bad up=6 | `CWE190_Integer_Overflow__int_rand_postinc_54_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up6.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__bad__up6.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 good up=4 | `goodB2G #19 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 good up=4 | `goodB2G #19 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 good up=4 | `goodB2G #26 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 good up=4 | `goodB2G #26 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 good up=6 | `goodB2G #19 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 good up=6 | `goodB2G #19 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 good up=6 | `goodB2G #26 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log) |
| CWE190_Integer_Overflow__int_rand_postinc_54 good up=6 | `goodB2G #26 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log) / [新](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=1 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=1 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=1 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=1 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=3 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=6 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up6.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up6.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=6 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up6.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up6.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=6 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up6.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up6.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=6 | `CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up6.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up6.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=2 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=2 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=2 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=2 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=3 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=4 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up4.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up4.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=6 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up6.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up6.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=6 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up6.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up6.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=6 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up6.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up6.log) |
| CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=6 | `CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up6.log) / [新](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__bad__up6.log) |
| CWE197_Numeric_Truncation_Error__short_rand_51 bad up=1 | `CWE197_Numeric_Truncation_Error__short_rand_51_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up1.log) / [新](logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up1.log) |
| CWE197_Numeric_Truncation_Error__short_rand_51 bad up=1 | `CWE197_Numeric_Truncation_Error__short_rand_51_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up1.log) / [新](logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up1.log) |
| CWE197_Numeric_Truncation_Error__short_rand_51 bad up=1 | `CWE197_Numeric_Truncation_Error__short_rand_51_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up1.log) / [新](logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up1.log) |
| CWE197_Numeric_Truncation_Error__short_rand_51 bad up=1 | `CWE197_Numeric_Truncation_Error__short_rand_51_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up1.log) / [新](logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up1.log) |
| CWE197_Numeric_Truncation_Error__short_rand_51 bad up=3 | `CWE197_Numeric_Truncation_Error__short_rand_51_bad #13 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__short_rand_51 bad up=3 | `CWE197_Numeric_Truncation_Error__short_rand_51_bad #13 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__short_rand_51 bad up=3 | `CWE197_Numeric_Truncation_Error__short_rand_51_bad #6 SignedOverflow` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up3.log) |
| CWE197_Numeric_Truncation_Error__short_rand_51 bad up=3 | `CWE197_Numeric_Truncation_Error__short_rand_51_bad #6 UnsignedWrap` | `boundary-triggerable` | `scoped-unreachable` | 每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。 | [旧](../juliet_run3/logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up3.log) / [新](logs/CWE197_Numeric_Truncation_Error__short_rand_51__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61_bad #1 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #14 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #14 UnsignedWrap` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 UnsignedWrap` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #20 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #20 UnsignedUnderflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #21 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #21 UnsignedUnderflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #7 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #7 UnsignedWrap` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=0 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 UnsignedWrap` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61_bad #1 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #14 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #14 UnsignedWrap` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 UnsignedWrap` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #20 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #20 UnsignedUnderflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #21 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #21 UnsignedUnderflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #7 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #7 UnsignedWrap` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=1 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 UnsignedWrap` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61_bad #1 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #14 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #14 UnsignedWrap` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 UnsignedWrap` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #20 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #20 UnsignedUnderflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #21 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #21 UnsignedUnderflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #7 SignedOverflow` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #7 UnsignedWrap` | `无候选` | `boundary-triggerable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 bad up=3 | `CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 UnsignedWrap` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log) |
| CWE190_Integer_Overflow__int_rand_add_61 good up=0 | `goodG2B #1 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__good__up0.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up0.log) |
| CWE190_Integer_Overflow__int_rand_add_61 good up=1 | `goodG2B #1 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__good__up1.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up1.log) |
| CWE190_Integer_Overflow__int_rand_add_61 good up=3 | `goodG2B #1 SignedOverflow` | `无候选` | `scoped-unreachable` | 返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。 | [旧](../juliet_run3/logs/CWE190_Integer_Overflow__int_rand_add_61__good__up3.log) / [新](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up3.log) |

单条风险状态变化共 178 次。

## scoped-unreachable 状态变化：新旧依赖与假设

### CWE190_Integer_Overflow__int_rand_add_51 bad up=1 / CWE190_Integer_Overflow__int_rand_add_51_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 bad up=1 / CWE190_Integer_Overflow__int_rand_add_51_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 bad up=1 / CWE190_Integer_Overflow__int_rand_add_51_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 bad up=1 / CWE190_Integer_Overflow__int_rand_add_51_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 bad up=3 / CWE190_Integer_Overflow__int_rand_add_51_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 bad up=3 / CWE190_Integer_Overflow__int_rand_add_51_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 bad up=3 / CWE190_Integer_Overflow__int_rand_add_51_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 bad up=3 / CWE190_Integer_Overflow__int_rand_add_51_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=1 / goodB2G #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=1 / goodB2G #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=1 / goodB2G #20 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=1 / goodB2G #20 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=2 / goodB2G #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=2 / goodB2G #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=2 / goodB2G #20 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=2 / goodB2G #20 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=3 / goodB2G #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=3 / goodB2G #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=3 / goodB2G #20 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_51 good up=3 / goodB2G #20 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=2 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=2 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=2 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=2 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=3 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=3 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=3 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=3 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=4 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=4 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=4 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 bad up=4 / CWE190_Integer_Overflow__int_rand_multiply_52_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=2 / goodB2G #17 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=2 / goodB2G #17 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=2 / goodB2G #24 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=2 / goodB2G #24 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=3 / goodB2G #17 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=3 / goodB2G #17 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=3 / goodB2G #24 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=3 / goodB2G #24 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=4 / goodB2G #17 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=4 / goodB2G #17 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=4 / goodB2G #24 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_multiply_52 good up=4 / goodB2G #24 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=4 / CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=4 / CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=4 / CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=4 / CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=6 / CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=6 / CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=6 / CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 bad up=6 / CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=4 / goodB2G #19 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=4 / goodB2G #19 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=4 / goodB2G #26 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=4 / goodB2G #26 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=6 / goodB2G #19 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=6 / goodB2G #19 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=6 / goodB2G #26 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__unsigned_int_rand_add_54 good up=6 / goodB2G #26 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 bad up=1 / CWE190_Integer_Overflow__short_rand_square_51_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 bad up=1 / CWE190_Integer_Overflow__short_rand_square_51_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 bad up=1 / CWE190_Integer_Overflow__short_rand_square_51_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 bad up=1 / CWE190_Integer_Overflow__short_rand_square_51_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 bad up=3 / CWE190_Integer_Overflow__short_rand_square_51_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 bad up=3 / CWE190_Integer_Overflow__short_rand_square_51_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 bad up=3 / CWE190_Integer_Overflow__short_rand_square_51_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 bad up=3 / CWE190_Integer_Overflow__short_rand_square_51_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 good up=1 / goodB2G #16 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 good up=1 / goodB2G #16 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 good up=1 / goodB2G #23 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 good up=1 / goodB2G #23 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 good up=3 / goodB2G #16 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 good up=3 / goodB2G #16 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 good up=3 / goodB2G #23 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__short_rand_square_51 good up=3 / goodB2G #23 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 bad up=4 / CWE190_Integer_Overflow__int_rand_postinc_54_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 bad up=4 / CWE190_Integer_Overflow__int_rand_postinc_54_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 bad up=4 / CWE190_Integer_Overflow__int_rand_postinc_54_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 bad up=4 / CWE190_Integer_Overflow__int_rand_postinc_54_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 bad up=6 / CWE190_Integer_Overflow__int_rand_postinc_54_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 bad up=6 / CWE190_Integer_Overflow__int_rand_postinc_54_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 bad up=6 / CWE190_Integer_Overflow__int_rand_postinc_54_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 bad up=6 / CWE190_Integer_Overflow__int_rand_postinc_54_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 good up=4 / goodB2G #19 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 good up=4 / goodB2G #19 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 good up=4 / goodB2G #26 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 good up=4 / goodB2G #26 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 good up=6 / goodB2G #19 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 good up=6 / goodB2G #19 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 good up=6 / goodB2G #26 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_postinc_54 good up=6 / goodB2G #26 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=1 / CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=1 / CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=1 / CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=1 / CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=3 / CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=3 / CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=3 / CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_51 bad up=3 / CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=6 / CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=6 / CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=6 / CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_char_54 bad up=6 / CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=2 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=2 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=2 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=2 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=3 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=3 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=3 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=3 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_52 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=4 / CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=6 / CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=6 / CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=6 / CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__int_rand_to_short_54 bad up=6 / CWE197_Numeric_Truncation_Error__int_rand_to_short_54_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__short_rand_51 bad up=1 / CWE197_Numeric_Truncation_Error__short_rand_51_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__short_rand_51 bad up=1 / CWE197_Numeric_Truncation_Error__short_rand_51_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__short_rand_51 bad up=1 / CWE197_Numeric_Truncation_Error__short_rand_51_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__short_rand_51 bad up=1 / CWE197_Numeric_Truncation_Error__short_rand_51_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__short_rand_51 bad up=3 / CWE197_Numeric_Truncation_Error__short_rand_51_bad #13 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__short_rand_51 bad up=3 / CWE197_Numeric_Truncation_Error__short_rand_51_bad #13 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__short_rand_51 bad up=3 / CWE197_Numeric_Truncation_Error__short_rand_51_bad #6 SignedOverflow

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE197_Numeric_Truncation_Error__short_rand_51 bad up=3 / CWE197_Numeric_Truncation_Error__short_rand_51_bad #6 UnsignedWrap

原因：每次 rand 返回值限制到 Windows MSVC [0,32767] 后，风险条件无解；见新旧依赖与假设。

第三轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第三轮假设：
```text
第三轮日志未输出假设审计，无法完整恢复自由值清单；旧版求解仍按上游无 UB/无 poison 的定义执行。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=0 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=CWE190_Integer_Overflow__int_rand_add_61b_badSource.data i32; rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=0 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 UnsignedWrap

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=CWE190_Integer_Overflow__int_rand_add_61b_badSource.data i32; rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=0 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=CWE190_Integer_Overflow__int_rand_add_61b_badSource.data i32; rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=0 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 UnsignedWrap

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=CWE190_Integer_Overflow__int_rand_add_61b_badSource.data i32; rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=1 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=1 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 UnsignedWrap

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=1 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=1 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 UnsignedWrap

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=3 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=3 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #16 UnsignedWrap

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=3 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 bad up=3 / CWE190_Integer_Overflow__int_rand_add_61b_badSource #9 UnsignedWrap

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```
第四轮假设：
```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

### CWE190_Integer_Overflow__int_rand_add_61 good up=0 / goodG2B #1 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
（无）
```
第四轮假设：
```text
Used assumptions: free-values=none; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
Rand range: not used
```

### CWE190_Integer_Overflow__int_rand_add_61 good up=1 / goodG2B #1 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
（无）
```
第四轮假设：
```text
Used assumptions: free-values=none; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
Rand range: not used
```

### CWE190_Integer_Overflow__int_rand_add_61 good up=3 / goodG2B #1 SignedOverflow

原因：返回值起点把来源函数结果连到调用者运算；第三轮以被覆盖的形参为起点。

第三轮依赖：
```text
（无）
```
第三轮假设：
```text
第三轮无此风险候选，因此没有对应假设。
```
第四轮依赖：
```text
（无）
```
第四轮假设：
```text
Used assumptions: free-values=none; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
Rand range: not used
```

## rand 范围 UNSAT 回归

有 scoped 风险的 41 次运行关闭 rand 范围后，原有 UNSAT 全部保持：PASS。
这是单向单调性：加范围不能使旧 UNSAT 变成 SAT；新范围可以把原 SAT 变成 UNSAT。完整记录见 [range_check_results.json](range_check_results.json)。

## RAND32 宏的 IR 假设审计

[测试 IR](ir/CWE190_Integer_Overflow__int_rand_add_51__bad.ll) 的 target triple 为 `x86_64-pc-windows-msvc19.50.35726`；本机 UCRT `stdlib.h:343` 定义 `RAND_MAX 0x7fff`。`RAND32()` 展开后的 `shl` 没有 `nsw`；源宏先把 `rand()` 转为 unsigned，再移位。因此位移回绕不是 poison。单次 `rand` 被约束到 [0,32767]，会限制移位操作数，但 3 次调用经移位与异或后仍可能形成宽范围的 32 位值；真实 PRNG 调用序列是否可由某个 seed 产生未验证。

## 所有 scoped-unreachable 的完整依赖清单

- `CWE190_Integer_Overflow__int_rand_add_51` good `up=0` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up0.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_add_51` good `up=1` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_add_51` good `up=2` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_add_51` good `up=3` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_multiply_52` good `up=0` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up0.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_multiply_52` good `up=2` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_multiply_52` good `up=3` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_multiply_52` good `up=4` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__unsigned_int_rand_add_54` good `up=0` `UnsignedWrap`：[日志](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up0.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__unsigned_int_rand_add_54` good `up=3` `UnsignedWrap`：[日志](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up3.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__unsigned_int_rand_add_54` good `up=4` `UnsignedWrap`：[日志](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__unsigned_int_rand_add_54` good `up=6` `UnsignedWrap`：[日志](logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up6.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__short_rand_square_51` bad `up=0` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__short_rand_square_51` bad `up=1` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__short_rand_square_51` bad `up=3` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_postinc_54` good `up=0` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up0.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_postinc_54` good `up=3` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up3.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_postinc_54` good `up=4` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up4.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_postinc_54` good `up=6` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_postinc_54__good__up6.log)；未建模=  Unmodeled dependency: unmodeled return value from external call before target: rand；过近似=  Over-approximation dependency: unmodeled return value from external call before target: rand；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_char_51` good `up=1` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up1.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_char_51` good `up=3` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up3.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_char_54` good `up=4` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up4.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_char_54` good `up=6` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up6.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_short_52` good `up=2` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up2.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_short_52` good `up=3` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up3.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_short_52` good `up=4` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up4.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_short_54` good `up=4` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__good__up4.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__int_rand_to_short_54` good `up=6` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_54__good__up6.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__short_rand_51` good `up=1` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__short_rand_51__good__up1.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE197_Numeric_Truncation_Error__short_rand_51` good `up=3` `SignedTruncationLoss`：[日志](logs/CWE197_Numeric_Truncation_Error__short_rand_51__good__up3.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_max_add_51` good `up=1` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_max_add_51__good__up1.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_max_add_51` good `up=3` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_max_add_51__good__up3.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_max_multiply_52` good `up=2` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up2.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_max_multiply_52` good `up=3` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up3.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_max_multiply_52` good `up=4` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up4.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int64_t_max_add_54` good `up=4` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int64_t_max_add_54__good__up4.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int64_t_max_add_54` good `up=6` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int64_t_max_add_54__good__up6.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_fscanf_add_51` good `up=0` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up0.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_add_61` good `up=0` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up0.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_add_61` good `up=1` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up1.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
- `CWE190_Integer_Overflow__int_rand_add_61` good `up=3` `SignedOverflow`：[日志](logs/CWE190_Integer_Overflow__int_rand_add_61__good__up3.log)；未建模=无；过近似=无；丢弃=无；未探索=无。
