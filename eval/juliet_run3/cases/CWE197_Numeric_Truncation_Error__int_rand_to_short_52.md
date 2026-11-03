# CWE197_Numeric_Truncation_Error__int_rand_to_short_52

- 源码：`E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52a.c` 等 3 个文件。
- 差异：bad 将传来的整数缩窄；goodG2B 将安全常量跨函数传给同一风险操作。
- 最小深度：`up=2`；新增深度：`up=4`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`20.31 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_badSink:call CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink arg0 context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_badSink call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_badSink:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data i32 unsigned=32768 signed=32768
  Operand operand0 i32 unsigned=32768 signed=32768
  Result: modular-bit-pattern=32768 i16 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink:%entry
  Path: Risk operation:   %conv = trunc i32 %data to i16, !dbg !29 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52c.c:27
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=2

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink --param-name data --up 2 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`190.88 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call4 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call4
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call6 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call6
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call9 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call9
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call12 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call12
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call14 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call14
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call17 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call17
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=1073676288 signed=1073676288
  Result: modular-bit-pattern=0 i16 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52a.c:30
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`185.87 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=1073676288 signed=1073676288
  Result: modular-bit-pattern=0 i16 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52a.c:30
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=4

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up4.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink --param-name data --up 4 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`184.73 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=1073676288 signed=1073676288
  Result: modular-bit-pattern=0 i16 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52a.c:30
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`19.10 ms`；实际：`boundary-triggerable`；判定：`OUT_OF_SCOPE`。
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
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_goodG2BSink:call CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink arg0 context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_goodG2BSink call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_goodG2BSink:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data i32 unsigned=32768 signed=32768
  Operand operand0 i32 unsigned=32768 signed=32768
  Result: modular-bit-pattern=32768 i16 ir-value=defined
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink:%entry
  Path: Risk operation:   %conv = trunc i32 %data to i16, !dbg !27 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52c.c:41
```

人工核对：安全来源尚在所选深度之外，边界反例不代表真实 good 误报。

## good / up=2

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up2.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink --param-name data --up 2 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`18.27 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
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

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`18.29 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
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

## good / up=4

- 原始日志：[stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up4.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink --param-name data --up 4 --down 3 --report-level 5`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`17.81 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
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

