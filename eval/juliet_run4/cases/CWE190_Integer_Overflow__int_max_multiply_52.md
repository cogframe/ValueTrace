# CWE190_Integer_Overflow__int_max_multiply_52

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52a.c` 等 3 个文件。
- 差异：bad 使用最大值常量；goodG2B 使用安全常量；二者跨文件传到运算位置。
- 最小深度：`up=2`；新增深度：`up=4`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_max_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_badSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`20.50 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
```

**下一层调用点提示**

```text
Model note: Boundary caller (N+1): CWE190_Integer_Overflow__int_max_multiply_52b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52b.c:27 actual0=local value
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
  depth boundary: CWE190_Integer_Overflow__int_max_multiply_52b_badSink:call CWE190_Integer_Overflow__int_max_multiply_52c_badSink arg0 context=root:CWE190_Integer_Overflow__int_max_multiply_52b_badSink call=CWE190_Integer_Overflow__int_max_multiply_52b_badSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52c_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data i32 unsigned=1073741824 signed=1073741824
  Operand operand0 i32 unsigned=1073741824 signed=1073741824
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52c_badSink
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52c_badSink:%entry
  Path: Branch true at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52c.c:24
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=2

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__bad__up2.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_max_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_badSink --param-name data --up 2 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`18.59 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=4294967294 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52_bad
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_max_multiply_52b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52a.c:32
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_max_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_badSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`17.74 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=4294967294 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52_bad
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_max_multiply_52b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52a.c:32
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=4

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__bad__up4.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_max_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_badSink --param-name data --up 4 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`17.66 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=4294967294 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52_bad
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_max_multiply_52b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52a.c:32
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_max_multiply_52__good.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`20.85 ms`；实际：`boundary-triggerable`；判定：`OUT_OF_SCOPE`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
```

**下一层调用点提示**

```text
Model note: Boundary caller (N+1): CWE190_Integer_Overflow__int_max_multiply_52b_goodG2BSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52b.c:39 actual0=local value
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
  depth boundary: CWE190_Integer_Overflow__int_max_multiply_52b_goodG2BSink:call CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink arg0 context=root:CWE190_Integer_Overflow__int_max_multiply_52b_goodG2BSink call=CWE190_Integer_Overflow__int_max_multiply_52b_goodG2BSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data i32 unsigned=1073741824 signed=1073741824
  Operand operand0 i32 unsigned=1073741824 signed=1073741824
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink:%entry
  Path: Branch true at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52c.c:39
```

人工核对：安全来源尚在所选深度之外，边界反例不代表真实 good 误报。

## good / up=2

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up2.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_max_multiply_52__good.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink --param-name data --up 2 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`19.08 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
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

## good / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_max_multiply_52__good.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`18.72 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
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

## good / up=4

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up4.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_max_multiply_52__good.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink --param-name data --up 4 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`19.87 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 2: all call arguments are integer constants
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

