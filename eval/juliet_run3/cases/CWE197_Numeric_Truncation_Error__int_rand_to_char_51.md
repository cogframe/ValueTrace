# CWE197_Numeric_Truncation_Error__int_rand_to_char_51

- 源码：`E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_51a.c` 等 2 个文件。
- 差异：bad 将传来的整数缩窄；goodG2B 将安全常量跨函数传给同一风险操作。
- 最小深度：`up=1`；新增深度：`up=3`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`18.40 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedTruncationLoss=boundary-triggerable；原因：无。

**依赖清单原文**

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
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink arg0 context=root:CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad call=CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call23
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink.data i32 unsigned=128 signed=128
  Operand operand0 i32 unsigned=128 signed=128
  Result: modular-bit-pattern=128 i8 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink:%entry
  Path: Risk operation:   %conv = trunc i32 %data to i8, !dbg !26 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_51b.c:27
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=1

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`157.97 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedTruncationLoss=boundary-triggerable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**过近似清单原文**

```text
Over-approximation dependency: unmodeled return value from external call before target: rand
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
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad/CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call4 call=CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call4
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad/CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call6 call=CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call6
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad/CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call9 call=CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call9
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad/CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call12 call=CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call12
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad/CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call14 call=CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call14
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad/CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call17 call=CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:call17
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=1073709184 signed=1073709184
  Result: modular-bit-pattern=128 i8 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_51a.c:30
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`160.84 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedTruncationLoss=boundary-triggerable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**过近似清单原文**

```text
Over-approximation dependency: unmodeled return value from external call before target: rand
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
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=1073709184 signed=1073709184
  Result: modular-bit-pattern=128 i8 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_51a.c:30
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`18.99 ms`；实际：`boundary-triggerable`；判定：`OUT_OF_SCOPE`。
- 风险：SignedTruncationLoss=boundary-triggerable；原因：无。

**依赖清单原文**

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
  depth boundary: goodG2B:call CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink arg0 context=root:goodG2B call=goodG2B:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink.data i32 unsigned=128 signed=128
  Operand operand0 i32 unsigned=128 signed=128
  Result: modular-bit-pattern=128 i8 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink:%entry
  Path: Risk operation:   %conv = trunc i32 %data to i8, !dbg !25 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_51b.c:41
```

人工核对：安全来源尚在所选深度之外，边界反例不代表真实 good 误报。

## good / up=1

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`15.03 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedTruncationLoss=scoped-unreachable；原因：无。

**依赖清单原文**

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
  Model note: Verification root: goodG2B; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: all call arguments are integer constants
```

**反例与关键值原文**

```text
（无）
```

人工核对：当前切片证明该风险不可触发；完整依赖清单见上。

**单条不可触发结论：SignedTruncationLoss 的依赖清单**

```text
（无）
```

## good / up=3

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`15.36 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedTruncationLoss=scoped-unreachable；原因：无。

**依赖清单原文**

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
  Model note: Verification root: goodG2B; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: all call arguments are integer constants
```

**反例与关键值原文**

```text
（无）
```

人工核对：当前切片证明该风险不可触发；完整依赖清单见上。

**单条不可触发结论：SignedTruncationLoss 的依赖清单**

```text
（无）
```

