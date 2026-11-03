# CWE190_Integer_Overflow__int_rand_add_61

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_61a.c` 等 2 个文件。
- 差异：_61 通过跨文件函数返回值传递；本轮用 --return-of 选来源函数，风险在调用者内。
- 最小深度：`up=1`；新增深度：`up=3`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_61__bad.ll --return-of CWE190_Integer_Overflow__int_rand_add_61b_badSource --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_badSource`，目标参数：`<return>`，期望：应报。
- 退出码：`0`；耗时：`147.45 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_61_bad/CWE190_Integer_Overflow__int_rand_add_61_bad:call0/CWE190_Integer_Overflow__int_rand_add_61b_badSource:call4 call=CWE190_Integer_Overflow__int_rand_add_61b_badSource:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_61_bad/CWE190_Integer_Overflow__int_rand_add_61_bad:call0/CWE190_Integer_Overflow__int_rand_add_61b_badSource:call6 call=CWE190_Integer_Overflow__int_rand_add_61b_badSource:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_61_bad/CWE190_Integer_Overflow__int_rand_add_61_bad:call0/CWE190_Integer_Overflow__int_rand_add_61b_badSource:call9 call=CWE190_Integer_Overflow__int_rand_add_61b_badSource:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_61_bad/CWE190_Integer_Overflow__int_rand_add_61_bad:call0/CWE190_Integer_Overflow__int_rand_add_61b_badSource:call12 call=CWE190_Integer_Overflow__int_rand_add_61b_badSource:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_61_bad/CWE190_Integer_Overflow__int_rand_add_61_bad:call0/CWE190_Integer_Overflow__int_rand_add_61b_badSource:call14 call=CWE190_Integer_Overflow__int_rand_add_61b_badSource:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_61_bad/CWE190_Integer_Overflow__int_rand_add_61_bad:call0/CWE190_Integer_Overflow__int_rand_add_61b_badSource:call17 call=CWE190_Integer_Overflow__int_rand_add_61b_badSource:call17
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_61_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_61b_badSource; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 0: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_add_61_bad
  Path: Block: CWE190_Integer_Overflow__int_rand_add_61_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_rand_add_61b_badSource at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_61a.c:30
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_61__bad.ll --return-of CWE190_Integer_Overflow__int_rand_add_61b_badSource --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_badSource`，目标参数：`<return>`，期望：应报。
- 退出码：`0`；耗时：`151.39 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_61_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 0: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_add_61_bad
  Path: Block: CWE190_Integer_Overflow__int_rand_add_61_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_rand_add_61b_badSource at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_61a.c:30
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_61__bad.ll --return-of CWE190_Integer_Overflow__int_rand_add_61b_badSource --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_badSource`，目标参数：`<return>`，期望：应报。
- 退出码：`0`；耗时：`151.08 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_61_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 0: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_add_61_bad
  Path: Block: CWE190_Integer_Overflow__int_rand_add_61_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_rand_add_61b_badSource at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_61a.c:30
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_61__good.ll --return-of CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource`，目标参数：`<return>`，期望：不应报。
- 退出码：`0`；耗时：`15.46 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 0: source function has no parameters and its caller passes no data
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

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__good__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_61__good.ll --return-of CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource`，目标参数：`<return>`，期望：不应报。
- 退出码：`0`；耗时：`16.09 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 0: source function has no parameters and its caller passes no data
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

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__int_rand_add_61__good.ll --return-of CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource`，目标参数：`<return>`，期望：不应报。
- 退出码：`0`；耗时：`15.86 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  Model note: Data slice closed at upstream depth 0: source function has no parameters and its caller passes no data
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

