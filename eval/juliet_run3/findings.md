# 第三轮结论

- 固定第二轮 26 个 bad/good 变体，新增 3 组共 6 个变体；实际运行 113 次。第二轮同键变化 28 次。
- 单调性自动检查：PASS，无新增丢弃却由 scoped-unreachable 降成 unknown 的次数为 0。
- 最小深度：CWE197 五组 good 均为 `scoped-unreachable`；CWE190 的短整型平方 good 仍为 `unknown`，因为浮点转整数 `fptosi` 未支持。
- `fscanf` good 在 up=0 为 `scoped-unreachable`，up=1/3 为 `unknown`；新增依赖包含未建模内存写入和被分类为丢弃的不支持 `load`，因此不违反本轮单调性规则。
- `_61`：选定来源函数的参数被覆盖，六次运行均无目标风险候选，不能作为有效风险结论。`_63`：指针参数入口不支持，六次运行以错误码 2 结束。两者按内存模型基线单列，不计入失败。

- `up=0`：TP=14，TN=5，FP=0，FN=0，UNK=1，OUT_OF_SCOPE=8。
- `up=3`：TP=13，TN=9，FP=0，FN=0，UNK=3，OUT_OF_SCOPE=3。

失败原因最高的三项：
- 不支持的指令：7 次；例如 `CWE190_Integer_Overflow__short_rand_square_51` good up=0，[日志](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up0.log)。
- 间接或外部调用：7 次；例如 `CWE190_Integer_Overflow__short_rand_square_51` good up=0，[日志](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up0.log)。
- 内存未建模：4 次；例如 `CWE190_Integer_Overflow__int_fscanf_add_51` bad up=1，[日志](logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up1.log)。

严重异常（good 误报或 bad 被判不可触发）：0 次。
未见工具崩溃或超时；本轮没有重复执行相同命令，因此输出一致性无法判断。另有 6 次 `_63` 入口明确拒绝指针参数，[示例日志](logs/CWE190_Integer_Overflow__int_rand_add_63__bad__up0.log）。

下一步最值得修的三项：
1. 支持指针参数选择与解引用传播：`_63` 当前无法进入验证，见 [日志](logs/CWE190_Integer_Overflow__int_rand_add_63__bad__up1.log)。
2. 连接函数返回值到调用者的风险切片：`_61` 当前零候选，见 [日志](logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log)。
3. 支持 `fptosi` 和常见数学辅助调用的条件建模：短整型平方 good 仍为 unknown，见 [日志](logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log)。

完整运行、逐项变化理由及每条安全结论的依赖清单见 [summary.md](summary.md)；各用例的完整证据见 [cases/](cases/)。
