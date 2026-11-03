# Juliet 第 1 轮评估结论

固定 10 组、40 次 verify 均完成（退出码 0）。目标 sink 的分级统计：up=0 为 TP 10、FN 0、TN 4、FP 5、UNK 1；up=3 为 TP 4、FN 0、TN 2、FP 2、UNK 12。

## 最常见的三个失败原因

- **其他（安全来源在 --up 边界外）：7 次。**例：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up0.log)（CWE197_Numeric_Truncation_Error__int_rand_to_char_51，good，up=0）。
- **外部输入源未建模：6 次。**例：[日志](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log)（CWE190_Integer_Overflow__int_rand_add_51，bad，up=3）。
- **上游上下文无法展开：6 次。**例：[日志](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)（CWE190_Integer_Overflow__int_rand_add_51，good，up=3）。

## 严重结果与解释

- good 被评为 FP：7 次，全部在 CWE197。[51 的 up=0](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up0.log)、[54 的 up=3](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up3.log) 为代表。这些 SAT 使用所选深度内的自由边界输入；54 的安全常量在第四次上游调用处，因此这类 FP 不能解释为 good 的实际来源被证明可触发。
- bad 被判 scoped-unreachable：0 次。若存在对应记录，应逐条复核；本轮没有。
- CWE190 的 [51 good](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)等在 up=0 已为 scoped-unreachable，up=3 却因调用上下文未能展开退为 unknown；这是最明确的结果稳定性问题。
- 工具崩溃或超时：未观察到；40 次退出码均为 0。每组只执行一次，重复运行的一致性**无法判断**。所选样例为 `.c`，但当前脚本按 C++20 生成 IR。

## 下一步最值得修的三件事

1. 修复 `target call context cannot be expanded within the selected data slice`；[190/51 good](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)、[197/52 good](logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up3.log)等 6 次 up=3 unknown 直接受其影响。
2. 给 `rand`/`RAND32()` 来源建立可审计的符号输入约束；[190/52 bad](logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log)、[197/51 bad](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log)等 6 次 up=3 unknown 因外部来源未建模。
3. 在报告中显式说明安全来源是否落在指定深度之外，并区分边界反例与完整 good 路径；[197/54 good](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up3.log)展示了边界被截在 54b，而安全常量在 54a。固定 up=3 时不能借此给出完整来源结论。
