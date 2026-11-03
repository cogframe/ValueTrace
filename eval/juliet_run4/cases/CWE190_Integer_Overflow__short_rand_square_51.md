# CWE190_Integer_Overflow__short_rand_square_51

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51a.c` 等 2 个文件。
- 差异：bad 使用 rand 来源；goodB2G 在风险运算前增加防护条件。
- 最小深度：`up=1`；新增深度：`up=3`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__short_rand_square_51__bad.ll --function CWE190_Integer_Overflow__short_rand_square_51b_badSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`1177.17 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=scoped-unreachable, UnsignedWrap=boundary-triggerable, SignedTruncationLoss=boundary-triggerable；原因：无。

**依赖清单原文**

```text
（无）
```

**使用的假设原文**

```text
Used assumptions: free-values=CWE190_Integer_Overflow__short_rand_square_51b_badSink.data i16; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Used assumptions: free-values=CWE190_Integer_Overflow__short_rand_square_51b_badSink.data i16; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: not used
```

**下一层调用点提示**

```text
Model note: Boundary caller (N+1): CWE190_Integer_Overflow__short_rand_square_51_bad at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51a.c:33 actual0=local value
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
  depth boundary: CWE190_Integer_Overflow__short_rand_square_51_bad:call CWE190_Integer_Overflow__short_rand_square_51b_badSink arg0 context=root:CWE190_Integer_Overflow__short_rand_square_51_bad call=CWE190_Integer_Overflow__short_rand_square_51_bad:call24
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__short_rand_square_51b_badSink.data i16 unsigned=62844 signed=-2692
  Operand operand0 i32 unsigned=4294964604 signed=-2692
  Operand operand1 i32 unsigned=4294964604 signed=-2692
  Result: modular-bit-pattern=7246864 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__short_rand_square_51b_badSink
  Path: Block: CWE190_Integer_Overflow__short_rand_square_51b_badSink:%entry
  Path: Risk operation:   %mul = mul i32 %conv, %conv1, !dbg !26 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:28
  Boundary input CWE190_Integer_Overflow__short_rand_square_51b_badSink.data i16 unsigned=62047 signed=-3489
  Operand operand0 i32 unsigned=12173121 signed=12173121
  Result: modular-bit-pattern=48961 i16 ir-value=defined
  Path: Risk operation:   %conv2 = trunc i32 %mul to i16, !dbg !26 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:28
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

**单条不可触发结论：SignedOverflow 的依赖清单**

```text
（无）
```

## bad / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__short_rand_square_51__bad.ll --function CWE190_Integer_Overflow__short_rand_square_51b_badSink --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`2234.70 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=scoped-unreachable, UnsignedWrap=boundary-triggerable, SignedTruncationLoss=boundary-triggerable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**使用的假设原文**

```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
Rand call rand.call0 i32 unsigned=1 signed=1
Rand call rand.call1 i32 unsigned=0 signed=0
Rand call rand.call3 i32 unsigned=1 signed=1
Rand call rand.call5 i32 unsigned=24576 signed=24576
Rand call rand.call5 i32 unsigned=26603 signed=26603
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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call4 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call6 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call9 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call12 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call14 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call17 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call17
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=4294959104 signed=-8192
  Operand operand1 i32 unsigned=4294959104 signed=-8192
  Result: modular-bit-pattern=67108864 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__short_rand_square_51_bad
  Path: Block: CWE190_Integer_Overflow__short_rand_square_51_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51a.c:32
  Operand operand0 i32 unsigned=38007225 signed=38007225
  Result: modular-bit-pattern=61881 i16 ir-value=defined
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

**单条不可触发结论：SignedOverflow 的依赖清单**

```text
  Unmodeled dependency: unmodeled return value from external call before target: rand
  Over-approximation dependency: unmodeled return value from external call before target: rand
```

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__short_rand_square_51__bad.ll --function CWE190_Integer_Overflow__short_rand_square_51b_badSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`2243.31 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=scoped-unreachable, UnsignedWrap=boundary-triggerable, SignedTruncationLoss=boundary-triggerable；原因：无。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
```

**使用的假设原文**

```text
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=no; debug-bitvector-domain-narrowing=no
Used assumptions: free-values=rand.call0 i32; rand.call1 i32; rand.call2 i32; rand.call3 i32; rand.call4 i32; rand.call5 i32; rand.call6 i32; upstream-no-UB-or-poison=enforced; debug-type-narrowing=risk-kind-only; debug-bitvector-domain-narrowing=no
```

**rand 范围与每次调用取值**

```text
Rand range: range verified against Windows MSVC RAND_MAX=32767 (target triple)
Rand call rand.call0 i32 unsigned=1 signed=1
Rand call rand.call1 i32 unsigned=0 signed=0
Rand call rand.call3 i32 unsigned=1 signed=1
Rand call rand.call5 i32 unsigned=24576 signed=24576
Rand call rand.call5 i32 unsigned=26603 signed=26603
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
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=4294959104 signed=-8192
  Operand operand1 i32 unsigned=4294959104 signed=-8192
  Result: modular-bit-pattern=67108864 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__short_rand_square_51_bad
  Path: Block: CWE190_Integer_Overflow__short_rand_square_51_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51a.c:32
  Operand operand0 i32 unsigned=38007225 signed=38007225
  Result: modular-bit-pattern=61881 i16 ir-value=defined
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

**单条不可触发结论：SignedOverflow 的依赖清单**

```text
  Unmodeled dependency: unmodeled return value from external call before target: rand
  Over-approximation dependency: unmodeled return value from external call before target: rand
```

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__short_rand_square_51__good.ll --function CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`1664.53 ms`；实际：`unknown`；判定：`UNK`。
- 风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；原因：不支持的指令。

**依赖清单原文**

```text
Unmodeled dependency: unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
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
Discarded dependency: unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
  depth boundary: goodB2G:call CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink arg0 context=root:goodB2G call=goodB2G:call24
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
（无）
```

人工核对：未确定；不支持的指令。最可能的后续工作是补齐对应模型或指令。

## good / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__short_rand_square_51__good.ll --function CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`3677.23 ms`；实际：`unknown`；判定：`UNK`。
- 风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；原因：不支持的指令、外部输入源未建模、间接或外部调用。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
Unmodeled dependency: unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
```

**使用的假设原文**

```text
（无）
```

**rand 范围与每次调用取值**

```text
Rand call rand.call0 i32 unsigned=1 signed=1
Rand call rand.call1 i32 unsigned=0 signed=0
Rand call rand.call3 i32 unsigned=1 signed=1
Rand call rand.call5 i32 unsigned=18753 signed=18753
Rand call rand.call5 i32 unsigned=32562 signed=32562
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
Discarded dependency: unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call4 call=goodB2G:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call6 call=goodB2G:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call9 call=goodB2G:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call12 call=goodB2G:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call14 call=goodB2G:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call17 call=goodB2G:call17
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
（无）
```

人工核对：未确定；不支持的指令、外部输入源未建模、间接或外部调用。最可能的后续工作是补齐对应模型或指令。

## good / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run4\ir\CWE190_Integer_Overflow__short_rand_square_51__good.ll --function CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`3671.53 ms`；实际：`unknown`；判定：`UNK`。
- 风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；原因：不支持的指令、外部输入源未建模、间接或外部调用。

**依赖清单原文**

```text
Unmodeled dependency: unmodeled return value from external call before target: rand
Unmodeled dependency: unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
```

**使用的假设原文**

```text
（无）
```

**rand 范围与每次调用取值**

```text
Rand call rand.call0 i32 unsigned=1 signed=1
Rand call rand.call1 i32 unsigned=0 signed=0
Rand call rand.call3 i32 unsigned=1 signed=1
Rand call rand.call5 i32 unsigned=18753 signed=18753
Rand call rand.call5 i32 unsigned=32562 signed=32562
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
Discarded dependency: unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
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

人工核对：未确定；不支持的指令、外部输入源未建模、间接或外部调用。最可能的后续工作是补齐对应模型或指令。

