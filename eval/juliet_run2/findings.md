# Juliet 第 2 轮评估结论

13 组、79 次固定配置 verify 与 5 次重复测时均完成，退出码均为 0。第一轮相同的 40 个配置改按 C17 编译后，目标风险类别和等级变化 **0 次**；仅按新规则把安全来源在深度外的 good 从 FP 重标为 `OUT_OF_SCOPE`。
up=0：TP 13、TN 4、UNK 1、OUT_OF_SCOPE 8；up=3：TP 7、TN 2、UNK 14、OUT_OF_SCOPE 3；最小深度：TP 3、TN 8、UNK 15。三组的 FN、FP 均为 0。
CWE197 的 5 个 good 在最小充分深度全部为 `scoped-unreachable`；3 组常量来源在充分深度均为 bad `boundary-triggerable`、good `scoped-unreachable`。bad 日志给出常量操作数与跨文件路径，仍标 `source-closed=false`。

## 最常见的三个失败原因

- **外部输入源未建模：21 次。**代表：[日志](logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log)（CWE190_Integer_Overflow__int_rand_add_51，bad，up=1）。
- **其他（安全来源在深度范围外）：11 次。**代表：[日志](logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up0.log)（CWE197_Numeric_Truncation_Error__int_rand_to_char_51，good，up=0）。
- **上游上下文无法展开：9 次。**代表：[日志](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log)（CWE190_Integer_Overflow__int_rand_add_51，good，up=2）。

## 深度突变与严重结果

- `CWE190/int_rand_add_51` good 在 **up=1** 首次由 scoped-unreachable 变为 unknown；[up=1 日志](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) 的相关原文：

```text
  Unmodeled dependency: risk depends on unmodeled control or operands before the target operation
  Unmodeled dependency: unmodeled return value from external call before target: rand
```

  [up=2 日志](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) 才首次出现调用上下文未探索，原文：

```text
  Unmodeled dependency: unexplored path: target call context cannot be expanded within the selected data slice
  Unexplored path: target call context cannot be expanded within the selected data slice
```

- good 真正落在范围内仍报 boundary-triggerable（FP）：0 次；bad 被判 scoped-unreachable：0 次。所有退出码为 0，未见超时或崩溃；除指定重复项外，同配置输出一致性无法判断。
- `short_rand_square_51` bad/up=0 的 5 次耗时为 1176.85, 1179.93, 1170.67, 1176.60, 1171.65 ms；中位数 1176.60 ms，范围 1170.67–1179.93 ms。

## 下一步最值得修的三件事

1. 保持本地 guard 的排除结论在扩大上游时不被无关未建模项污染；[190/51 good up=0](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up0.log) 与 [up=1](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log) 是直接证据。
2. 修复上游调用上下文展开；[190/51 good up=2](logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log) 和其他 `_51/_52` 的较深查询出现未探索路径。
3. 给 rand/RAND32 外部来源建立有依据的模型，同时保留来源未闭合标记；CWE190/CWE197 的 rand bad 在充分深度普遍退为 unknown。
