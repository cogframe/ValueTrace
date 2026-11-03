# 第四轮结论

- 第三轮固定的 16 组、113 次运行全部重跑；目标整体等级变化 6 次，单条风险状态变化 178 次。变化原因及涉及的依赖、假设见 [summary.md](summary.md)。
- `_61` 返回值起点已接通：bad 的调用者加法可触发，good 不可触发。第三轮六次均因形参起点被覆盖而无候选；本轮六次都有目标风险结论。
- rand 范围按 IR 的 Windows MSVC 目标约束到 [0,32767]；41 次含不可触发风险的命令关闭范围重跑，UNSAT 保持 41/41。反例现在打印每次 rand 调用的取值；PRNG 序列可达性仍未验证。
- `RAND32()` 展开的 `shl` 在本次 IR 中没有 `nsw`：源宏先转换成 unsigned，位移回绕不会因 `nsw` 产生 poison；限制单次 rand 的范围不等于限制宏结果到 RAND_MAX。
- 边界提示验收：CWE197 `int_rand_to_char_54` good、`up=3` 的下一层调用点实参是常量 **122**（以源码为准），见 [日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up3.log)。
- `llvm.abs` 与四种整数 min/max 已建模；短整型平方 good 仍因浮点转换 `fptosi` 为 unknown。`_63` 的指针参数入口仍明确拒绝，六次运行作为内存模型基线。
- 第三轮单调性检查在本轮仍为 PASS；完整 CTest 为 34 项通过、1 项原本禁用。

- `up=0`：TP=15，TN=6，FP=0，FN=0，UNK=1，OUT_OF_SCOPE=8。
- `up=3`：TP=14，TN=10，FP=0，FN=0，UNK=3，OUT_OF_SCOPE=3。

剩余最明显的缺口：
- 不支持的指令：7 次；例 `CWE190_Integer_Overflow__short_rand_square_51` good up=0，[日志](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up0.log)。
- 间接或外部调用：6 次；例 `CWE190_Integer_Overflow__short_rand_square_51` good up=1，[日志](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log)。
- 内存未建模：4 次；例 `CWE190_Integer_Overflow__int_fscanf_add_51` bad up=1，[日志](logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up1.log)。

优先工作：指针参数及内存传播（_63）、`fscanf` 输出内存建模、浮点转换条件建模。
全部原始日志见 [logs/](logs/)；逐例报告见 [cases/](cases/)；rand 范围回归见 [range_check_results.json](range_check_results.json)。
