# CWE190_Integer_Overflow__int_rand_multiply_52

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_multiply_52a.c` 等 3 个文件。
- 差异：bad 使用 rand 来源；goodB2G 在风险运算前增加防护条件。
- 最小深度：`up=2`；新增深度：`up=4`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_rand_multiply_52c_badSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_multiply_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`21.99 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

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
  depth boundary: CWE190_Integer_Overflow__int_rand_multiply_52b_badSink:call CWE190_Integer_Overflow__int_rand_multiply_52c_badSink arg0 context=root:CWE190_Integer_Overflow__int_rand_multiply_52b_badSink call=CWE190_Integer_Overflow__int_rand_multiply_52b_badSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_multiply_52c_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int_rand_multiply_52c_badSink.data i32 unsigned=1073741824 signed=1073741824
  Operand operand0 i32 unsigned=1073741824 signed=1073741824
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_multiply_52c_badSink
  Path: Block: CWE190_Integer_Overflow__int_rand_multiply_52c_badSink:%entry
  Path: Branch true at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_multiply_52c.c:24
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=2

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up2.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_rand_multiply_52c_badSink --param-name data --up 2 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_multiply_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`183.64 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_bad/CWE190_Integer_Overflow__int_rand_multiply_52_bad:call4 call=CWE190_Integer_Overflow__int_rand_multiply_52_bad:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_bad/CWE190_Integer_Overflow__int_rand_multiply_52_bad:call6 call=CWE190_Integer_Overflow__int_rand_multiply_52_bad:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_bad/CWE190_Integer_Overflow__int_rand_multiply_52_bad:call9 call=CWE190_Integer_Overflow__int_rand_multiply_52_bad:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_bad/CWE190_Integer_Overflow__int_rand_multiply_52_bad:call12 call=CWE190_Integer_Overflow__int_rand_multiply_52_bad:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_bad/CWE190_Integer_Overflow__int_rand_multiply_52_bad:call14 call=CWE190_Integer_Overflow__int_rand_multiply_52_bad:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_bad/CWE190_Integer_Overflow__int_rand_multiply_52_bad:call17 call=CWE190_Integer_Overflow__int_rand_multiply_52_bad:call17
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_multiply_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147450880 signed=2147450880
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=4294901760 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_multiply_52_bad
  Path: Block: CWE190_Integer_Overflow__int_rand_multiply_52_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_multiply_52a.c:31
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_rand_multiply_52c_badSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_multiply_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`194.12 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

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
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_multiply_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147450880 signed=2147450880
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=4294901760 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_multiply_52_bad
  Path: Block: CWE190_Integer_Overflow__int_rand_multiply_52_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_multiply_52a.c:31
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=4

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_multiply_52__bad__up4.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_rand_multiply_52c_badSink --param-name data --up 4 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_multiply_52c_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`198.26 ms`；实际：`boundary-triggerable`；判定：`TP`。
- 风险：SignedOverflow=boundary-triggerable；原因：无。

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
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_multiply_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
  Operand operand0 i32 unsigned=2147450880 signed=2147450880
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=4294901760 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_multiply_52_bad
  Path: Block: CWE190_Integer_Overflow__int_rand_multiply_52_bad:%entry
  Path: Call: rand at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_multiply_52a.c:31
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_multiply_52__good.ll --function CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`33.78 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

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
  depth boundary: CWE190_Integer_Overflow__int_rand_multiply_52b_goodB2GSink:call CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink arg0 context=root:CWE190_Integer_Overflow__int_rand_multiply_52b_goodB2GSink call=CWE190_Integer_Overflow__int_rand_multiply_52b_goodB2GSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
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

## good / up=2

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up2.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_multiply_52__good.ll --function CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink --param-name data --up 2 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`237.12 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_good/CWE190_Integer_Overflow__int_rand_multiply_52_good:call1/goodB2G:call4 call=goodB2G:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_good/CWE190_Integer_Overflow__int_rand_multiply_52_good:call1/goodB2G:call6 call=goodB2G:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_good/CWE190_Integer_Overflow__int_rand_multiply_52_good:call1/goodB2G:call9 call=goodB2G:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_good/CWE190_Integer_Overflow__int_rand_multiply_52_good:call1/goodB2G:call12 call=goodB2G:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_good/CWE190_Integer_Overflow__int_rand_multiply_52_good:call1/goodB2G:call14 call=goodB2G:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_multiply_52_good/CWE190_Integer_Overflow__int_rand_multiply_52_good:call1/goodB2G:call17 call=goodB2G:call17
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
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

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_multiply_52__good.ll --function CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`234.80 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

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
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
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

## good / up=4

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_multiply_52__good__up4.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_multiply_52__good.ll --function CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink --param-name data --up 4 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_multiply_52c_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`224.50 ms`；实际：`scoped-unreachable`；判定：`TN`。
- 风险：SignedOverflow=scoped-unreachable；原因：无。

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
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 2: source function has no parameters and its caller passes no data
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

