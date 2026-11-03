# CWE190_Integer_Overflow__int_rand_add_63

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_63a.c` 等 2 个文件。
- 差异：_63 通过指针参数传递；sink 先解引用 dataPtr，再对 data 加 1。
- 最小深度：`up=1`；新增深度：`up=3`。
- 分类：指针内存模型基线，不计入失败。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_63__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_63__bad.ll --function CWE190_Integer_Overflow__int_rand_add_63b_badSink --param-name dataPtr --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_63b_badSink`，目标参数：`dataPtr`，期望：应报。
- 退出码：`2`；耗时：`8.98 ms`；实际：`run-failed`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（目标参数类型不支持）。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
（无）
```

**rand 范围与每次调用取值**

```text
（无）
```

**下一层调用点提示**

```text
（无）
```

**过近似清单原文**

```text
（无）
```

**丢弃清单原文**

```text
（无）
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：工具拒绝指针参数，详见原始错误；这是入口能力不足，未作风险判断。

```text
  E:/ValueTrace/eval/juliet_run4/ir/CWE190_Integer_Overflow__int_rand_add_63__bad.ll

[stderr]
Error: [unsupported-type] 'dataPtr': Mapped to LLVM argument 0; only scalar integer source parameters are traceable (pointer/this, aggregate or unknown source types are unsupported).
```

## bad / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_63__bad__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_63__bad.ll --function CWE190_Integer_Overflow__int_rand_add_63b_badSink --param-name dataPtr --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_63b_badSink`，目标参数：`dataPtr`，期望：应报。
- 退出码：`2`；耗时：`9.10 ms`；实际：`run-failed`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（目标参数类型不支持）。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
（无）
```

**rand 范围与每次调用取值**

```text
（无）
```

**下一层调用点提示**

```text
（无）
```

**过近似清单原文**

```text
（无）
```

**丢弃清单原文**

```text
（无）
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：工具拒绝指针参数，详见原始错误；这是入口能力不足，未作风险判断。

```text
  E:/ValueTrace/eval/juliet_run4/ir/CWE190_Integer_Overflow__int_rand_add_63__bad.ll

[stderr]
Error: [unsupported-type] 'dataPtr': Mapped to LLVM argument 0; only scalar integer source parameters are traceable (pointer/this, aggregate or unknown source types are unsupported).
```

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_63__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_63__bad.ll --function CWE190_Integer_Overflow__int_rand_add_63b_badSink --param-name dataPtr --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_63b_badSink`，目标参数：`dataPtr`，期望：应报。
- 退出码：`2`；耗时：`9.13 ms`；实际：`run-failed`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（目标参数类型不支持）。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
（无）
```

**rand 范围与每次调用取值**

```text
（无）
```

**下一层调用点提示**

```text
（无）
```

**过近似清单原文**

```text
（无）
```

**丢弃清单原文**

```text
（无）
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：工具拒绝指针参数，详见原始错误；这是入口能力不足，未作风险判断。

```text
  E:/ValueTrace/eval/juliet_run4/ir/CWE190_Integer_Overflow__int_rand_add_63__bad.ll

[stderr]
Error: [unsupported-type] 'dataPtr': Mapped to LLVM argument 0; only scalar integer source parameters are traceable (pointer/this, aggregate or unknown source types are unsupported).
```

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_63__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_63__good.ll --function CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink --param-name dataPtr --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink`，目标参数：`dataPtr`，期望：不应报。
- 退出码：`2`；耗时：`9.98 ms`；实际：`run-failed`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（目标参数类型不支持）。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
（无）
```

**rand 范围与每次调用取值**

```text
（无）
```

**下一层调用点提示**

```text
（无）
```

**过近似清单原文**

```text
（无）
```

**丢弃清单原文**

```text
（无）
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：工具拒绝指针参数，详见原始错误；这是入口能力不足，未作风险判断。

```text
  E:/ValueTrace/eval/juliet_run4/ir/CWE190_Integer_Overflow__int_rand_add_63__good.ll

[stderr]
Error: [unsupported-type] 'dataPtr': Mapped to LLVM argument 0; only scalar integer source parameters are traceable (pointer/this, aggregate or unknown source types are unsupported).
```

## good / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_63__good__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_63__good.ll --function CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink --param-name dataPtr --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink`，目标参数：`dataPtr`，期望：不应报。
- 退出码：`2`；耗时：`9.75 ms`；实际：`run-failed`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（目标参数类型不支持）。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
（无）
```

**rand 范围与每次调用取值**

```text
（无）
```

**下一层调用点提示**

```text
（无）
```

**过近似清单原文**

```text
（无）
```

**丢弃清单原文**

```text
（无）
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：工具拒绝指针参数，详见原始错误；这是入口能力不足，未作风险判断。

```text
  E:/ValueTrace/eval/juliet_run4/ir/CWE190_Integer_Overflow__int_rand_add_63__good.ll

[stderr]
Error: [unsupported-type] 'dataPtr': Mapped to LLVM argument 0; only scalar integer source parameters are traceable (pointer/this, aggregate or unknown source types are unsupported).
```

## good / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_63__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_63__good.ll --function CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink --param-name dataPtr --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_63b_goodB2GSink`，目标参数：`dataPtr`，期望：不应报。
- 退出码：`2`；耗时：`9.54 ms`；实际：`run-failed`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（目标参数类型不支持）。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
（无）
```

**rand 范围与每次调用取值**

```text
（无）
```

**下一层调用点提示**

```text
（无）
```

**过近似清单原文**

```text
（无）
```

**丢弃清单原文**

```text
（无）
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：工具拒绝指针参数，详见原始错误；这是入口能力不足，未作风险判断。

```text
  E:/ValueTrace/eval/juliet_run4/ir/CWE190_Integer_Overflow__int_rand_add_63__good.ll

[stderr]
Error: [unsupported-type] 'dataPtr': Mapped to LLVM argument 0; only scalar integer source parameters are traceable (pointer/this, aggregate or unknown source types are unsupported).
```

