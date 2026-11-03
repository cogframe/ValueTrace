# CWE190_Integer_Overflow__int64_t_max_add_54

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54a.c` 等 5 个文件。
- 差异：bad 使用最大值常量；goodG2B 使用安全常量；二者跨文件传到运算位置。
- 最小深度：`up=4`；新增深度：`up=6`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int64_t_max_add_54__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int64_t_max_add_54__bad.ll --function CWE190_Integer_Overflow__int64_t_max_add_54e_badSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int64_t_max_add_54e_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`21.26 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data i64; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
```

**下一层调用点提示**

```text
Model note: Boundary caller (N+1): CWE190_Integer_Overflow__int64_t_max_add_54d_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54d.c:27 actual0=local value
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
  depth boundary: CWE190_Integer_Overflow__int64_t_max_add_54d_badSink:call CWE190_Integer_Overflow__int64_t_max_add_54e_badSink arg0 context=root:CWE190_Integer_Overflow__int64_t_max_add_54d_badSink call=CWE190_Integer_Overflow__int64_t_max_add_54d_badSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__int64_t_max_add_54e_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int64_t_max_add_54e_badSink.data i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand0 i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand1 i64 unsigned=1 signed=1
  Result: modular-bit-pattern=9223372036854775808 i64 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int64_t_max_add_54e_badSink
  Path: Block: CWE190_Integer_Overflow__int64_t_max_add_54e_badSink:%entry
  Path: Risk operation:   %add = add i64 %data, 1, !dbg !31 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54e.c:26
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int64_t_max_add_54__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int64_t_max_add_54__bad.ll --function CWE190_Integer_Overflow__int64_t_max_add_54e_badSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int64_t_max_add_54e_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`20.85 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__int64_t_max_add_54b_badSink.data i64; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
```

**下一层调用点提示**

```text
Model note: Boundary caller (N+1): CWE190_Integer_Overflow__int64_t_max_add_54_bad at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54a.c:31 actual0=constant 9223372036854775807
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
  depth boundary: CWE190_Integer_Overflow__int64_t_max_add_54_bad:call CWE190_Integer_Overflow__int64_t_max_add_54b_badSink arg0 context=root:CWE190_Integer_Overflow__int64_t_max_add_54_bad call=CWE190_Integer_Overflow__int64_t_max_add_54_bad:call0
  Model note: Verification root: CWE190_Integer_Overflow__int64_t_max_add_54b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int64_t_max_add_54b_badSink.data i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand0 i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand1 i64 unsigned=1 signed=1
  Result: modular-bit-pattern=9223372036854775808 i64 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int64_t_max_add_54b_badSink
  Path: Block: CWE190_Integer_Overflow__int64_t_max_add_54b_badSink:%entry
  Path: Call: CWE190_Integer_Overflow__int64_t_max_add_54c_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54b.c:27
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=4

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int64_t_max_add_54__bad__up4.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int64_t_max_add_54__bad.ll --function CWE190_Integer_Overflow__int64_t_max_add_54e_badSink --param-name data --up 4 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int64_t_max_add_54e_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`16.45 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=none; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
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
  Model note: Verification root: CWE190_Integer_Overflow__int64_t_max_add_54_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 4: all call arguments are integer constants
```

**反例与关键值原文**

```text
  Operand operand0 i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand1 i64 unsigned=1 signed=1
  Result: modular-bit-pattern=9223372036854775808 i64 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int64_t_max_add_54_bad
  Path: Block: CWE190_Integer_Overflow__int64_t_max_add_54_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int64_t_max_add_54b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54a.c:31
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=6

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int64_t_max_add_54__bad__up6.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int64_t_max_add_54__bad.ll --function CWE190_Integer_Overflow__int64_t_max_add_54e_badSink --param-name data --up 6 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int64_t_max_add_54e_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`16.12 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=none; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
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
  Model note: Verification root: CWE190_Integer_Overflow__int64_t_max_add_54_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 4: all call arguments are integer constants
```

**反例与关键值原文**

```text
  Operand operand0 i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand1 i64 unsigned=1 signed=1
  Result: modular-bit-pattern=9223372036854775808 i64 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int64_t_max_add_54_bad
  Path: Block: CWE190_Integer_Overflow__int64_t_max_add_54_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int64_t_max_add_54b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54a.c:31
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int64_t_max_add_54__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int64_t_max_add_54__good.ll --function CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`20.88 ms`；实际：`boundary-triggerable`；判定：`OUT_OF_SCOPE`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data i64; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
```

**下一层调用点提示**

```text
Model note: Boundary caller (N+1): CWE190_Integer_Overflow__int64_t_max_add_54d_goodG2BSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54d.c:39 actual0=local value
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
  depth boundary: CWE190_Integer_Overflow__int64_t_max_add_54d_goodG2BSink:call CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink arg0 context=root:CWE190_Integer_Overflow__int64_t_max_add_54d_goodG2BSink call=CWE190_Integer_Overflow__int64_t_max_add_54d_goodG2BSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink.data i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand0 i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand1 i64 unsigned=1 signed=1
  Result: modular-bit-pattern=9223372036854775808 i64 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink
  Path: Block: CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink:%entry
  Path: Risk operation:   %add = add i64 %data, 1, !dbg !38 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54e.c:40
```

人工核对：安全来源尚在所选深度之外，边界反例不代表真实 good 误报。

## good / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int64_t_max_add_54__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int64_t_max_add_54__good.ll --function CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`21.90 ms`；实际：`boundary-triggerable`；判定：`OUT_OF_SCOPE`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__int64_t_max_add_54b_goodG2BSink.data i64; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
```

**下一层调用点提示**

```text
Model note: Boundary caller (N+1): goodG2B at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54a.c:47 actual0=constant 2
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
  depth boundary: goodG2B:call CWE190_Integer_Overflow__int64_t_max_add_54b_goodG2BSink arg0 context=root:goodG2B call=goodG2B:call0
  Model note: Verification root: CWE190_Integer_Overflow__int64_t_max_add_54b_goodG2BSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int64_t_max_add_54b_goodG2BSink.data i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand0 i64 unsigned=9223372036854775807 signed=9223372036854775807
  Operand operand1 i64 unsigned=1 signed=1
  Result: modular-bit-pattern=9223372036854775808 i64 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int64_t_max_add_54b_goodG2BSink
  Path: Block: CWE190_Integer_Overflow__int64_t_max_add_54b_goodG2BSink:%entry
  Path: Call: CWE190_Integer_Overflow__int64_t_max_add_54c_goodG2BSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s02\CWE190_Integer_Overflow__int64_t_max_add_54b.c:39
```

人工核对：安全来源尚在所选深度之外，边界反例不代表真实 good 误报。

## good / up=4

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int64_t_max_add_54__good__up4.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int64_t_max_add_54__good.ll --function CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink --param-name data --up 4 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`16.23 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=none; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
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
  Model note: Verification root: goodG2B; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 4: all call arguments are integer constants
```

**反例与关键值原文**

```text
（无）
```

人工核对：当前切片证明该风险不可触发；完整依赖清单见上。

**单条不可触发结论：SignedOverflow 的依赖清单**

```text
（无）
```

## good / up=6

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int64_t_max_add_54__good__up6.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int64_t_max_add_54__good.ll --function CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink --param-name data --up 6 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int64_t_max_add_54e_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`15.78 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=none; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
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
  Model note: Verification root: goodG2B; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 4: all call arguments are integer constants
```

**反例与关键值原文**

```text
（无）
```

人工核对：当前切片证明该风险不可触发；完整依赖清单见上。

**单条不可触发结论：SignedOverflow 的依赖清单**

```text
（无）
```

