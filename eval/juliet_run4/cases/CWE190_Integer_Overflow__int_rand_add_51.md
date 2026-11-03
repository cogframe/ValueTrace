# CWE190_Integer_Overflow__int_rand_add_51

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_51a.c` 等 2 个文件。
- 差异：bad 使用 rand 来源；goodB2G 在风险运算前增加防护条件。
- 最小深度：`up=1`；新增深度：`up=3`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_51__bad.ll --function CWE190_Integer_Overflow__int_rand_add_51b_badSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`22.40 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__int_rand_add_51b_badSink.data i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
```

**下一层调用点提示**

```text
Model note: Boundary caller (N+1): CWE190_Integer_Overflow__int_rand_add_51_bad at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_51a.c:32 actual0=unable to determine
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
  depth boundary: CWE190_Integer_Overflow__int_rand_add_51_bad:call CWE190_Integer_Overflow__int_rand_add_51b_badSink arg0 context=root:CWE190_Integer_Overflow__int_rand_add_51_bad call=CWE190_Integer_Overflow__int_rand_add_51_bad:call23
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int_rand_add_51b_badSink.data i32 unsigned=2147483647 signed=2147483647
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_add_51b_badSink
  Path: Block: CWE190_Integer_Overflow__int_rand_add_51b_badSink:%entry
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !25 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_51b.c:26
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_51__bad.ll --function CWE190_Integer_Overflow__int_rand_add_51b_badSink --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`146.71 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**使用的假设原文**

```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
Rand call rand.call0 i32 unsigned=1 signed=1
Rand call rand.call1 i32 unsigned=1 signed=1
Rand call rand.call3 i32 unsigned=32767 signed=32767
Rand call rand.call5 i32 unsigned=32767 signed=32767
```

**下一层调用点提示**

```text
（无）
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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call4 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call6 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call9 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call12 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call14 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call17 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call17
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_add_51_bad
  Path: Block: CWE190_Integer_Overflow__int_rand_add_51_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_51a.c:31
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_51__bad.ll --function CWE190_Integer_Overflow__int_rand_add_51b_badSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`151.02 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**使用的假设原文**

```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
Rand call rand.call0 i32 unsigned=1 signed=1
Rand call rand.call1 i32 unsigned=1 signed=1
Rand call rand.call3 i32 unsigned=32767 signed=32767
Rand call rand.call5 i32 unsigned=32767 signed=32767
```

**下一层调用点提示**

```text
（无）
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
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_add_51_bad
  Path: Block: CWE190_Integer_Overflow__int_rand_add_51_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_51a.c:31
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_51__good.ll --function CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`23.67 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink.data i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
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
  depth boundary: goodB2G:call CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink arg0 context=root:goodB2G call=goodB2G:call23
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
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

## good / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_51__good.ll --function CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`192.81 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**使用的假设原文**

```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

**下一层调用点提示**

```text
（无）
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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call4 call=goodB2G:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call6 call=goodB2G:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call9 call=goodB2G:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call12 call=goodB2G:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call14 call=goodB2G:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call17 call=goodB2G:call17
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
（无）
```

人工核对：当前切片证明该风险不可触发；完整依赖清单见上。

**单条不可触发结论：SignedOverflow 的依赖清单**

```text
  Unmodeled dependency: unmodeled return value from external call before target: rand
  Over-approximation dependency: unmodeled return value from external call before target: rand
```

## good / up=2

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_51__good.ll --function CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink --param-name data --up 2 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`189.60 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**使用的假设原文**

```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

**下一层调用点提示**

```text
（无）
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
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
（无）
```

人工核对：当前切片证明该风险不可触发；完整依赖清单见上。

**单条不可触发结论：SignedOverflow 的依赖清单**

```text
  Unmodeled dependency: unmodeled return value from external call before target: rand
  Over-approximation dependency: unmodeled return value from external call before target: rand
```

## good / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_51__good.ll --function CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`189.41 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**使用的假设原文**

```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
```

**下一层调用点提示**

```text
（无）
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
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
（无）
```

人工核对：当前切片证明该风险不可触发；完整依赖清单见上。

**单条不可触发结论：SignedOverflow 的依赖清单**

```text
  Unmodeled dependency: unmodeled return value from external call before target: rand
  Over-approximation dependency: unmodeled return value from external call before target: rand
```

