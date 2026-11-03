# CWE190_Integer_Overflow__int_rand_add_51

- 源文件：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_51a.c` 等，共 2 个。
- 源码差异：bad 用 rand 来源做 int + 1；goodB2G 在加法前检查 data < INT_MAX。
- 目标：最后一级跨文件 sink 的 `data` 参数；CWE190 rand 用 goodB2G，CWE197 与常量组用 goodG2B。
- 最小充分深度：`up=1`（按预先固定的 51/52/54 调用结构）。
- 以下风险等级只取目标 sink；上游运算候选可在原始日志查阅。

## bad / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_rand_add_51__bad.ll --function CWE190_Integer_Overflow__int_rand_add_51b_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`23.75 ms`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__int_rand_add_51_bad:call CWE190_Integer_Overflow__int_rand_add_51b_badSink arg0 context=root:CWE190_Integer_Overflow__int_rand_add_51_bad call=CWE190_Integer_Overflow__int_rand_add_51_bad:call23
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Boundary input CWE190_Integer_Overflow__int_rand_add_51b_badSink.data i32 unsigned=2147483647 signed=2147483647
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_rand_add_51b_badSink
  Path: Block: CWE190_Integer_Overflow__int_rand_add_51b_badSink:%entry
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !25 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_51b.c:26
```


**人工核对：**目标风险在选定边界下可触发；若来源仍在边界外，此结论仍非完整来源证明。

## bad / up=1

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up1.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_rand_add_51__bad.ll --function CWE190_Integer_Overflow__int_rand_add_51b_badSink --param-name data --up 1 --down 3 --report-level 5`
- 退出码：`0`；耗时：`154.19 ms`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=unknown；实际等级：`unknown`；判定：`UNK`。
- 失败原因分类：外部输入源未建模。

**未建模依赖清单原文（同文去重）**

```text
  Unmodeled dependency: risk depends on unmodeled control or operands before the target operation
  Unmodeled dependency: unmodeled return value from external call before target: rand
```

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call4 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call6 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call9 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call12 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call14 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_bad/CWE190_Integer_Overflow__int_rand_add_51_bad:call17 call=CWE190_Integer_Overflow__int_rand_add_51_bad:call17
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**目标 sink 的局部风险存在，但 rand 返回值未建模；未确定有依据，不能算已证实的来源反例。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## bad / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_rand_add_51__bad.ll --function CWE190_Integer_Overflow__int_rand_add_51b_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`153.75 ms`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=unknown；实际等级：`unknown`；判定：`UNK`。
- 失败原因分类：外部输入源未建模。

**未建模依赖清单原文（同文去重）**

```text
  Unmodeled dependency: risk depends on unmodeled control or operands before the target operation
  Unmodeled dependency: unmodeled return value from external call before target: rand
```

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**目标 sink 的局部风险存在，但 rand 返回值未建模；未确定有依据，不能算已证实的来源反例。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## good / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_rand_add_51__good.ll --function CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`26.45 ms`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=scoped-unreachable；实际等级：`scoped-unreachable`；判定：`TN`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: goodB2G:call CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink arg0 context=root:goodB2G call=goodB2G:call23
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**本地 guard 排除风险路径；与 goodB2G 源码一致。

## good / up=1

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_rand_add_51__good.ll --function CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink --param-name data --up 1 --down 3 --report-level 5`
- 退出码：`0`；耗时：`312.40 ms`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=unknown；实际等级：`unknown`；判定：`UNK`。
- 失败原因分类：外部输入源未建模。

**未建模依赖清单原文（同文去重）**

```text
  Unmodeled dependency: risk depends on unmodeled control or operands before the target operation
  Unmodeled dependency: unmodeled return value from external call before target: rand
```

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call4 call=goodB2G:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call6 call=goodB2G:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call9 call=goodB2G:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call12 call=goodB2G:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call14 call=goodB2G:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call17 call=goodB2G:call17
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**扩大到第一跳后把 rand 未建模依赖纳入；本地 guard 原可排除风险，现退化为 unknown。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## good / up=2

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_rand_add_51__good.ll --function CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink --param-name data --up 2 --down 3 --report-level 5`
- 退出码：`0`；耗时：`79.71 ms`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=unknown；实际等级：`unknown`；判定：`UNK`。
- 失败原因分类：上游上下文无法展开。

**未建模依赖清单原文（同文去重）**

```text
  Unmodeled dependency: unexplored path: target call context cannot be expanded within the selected data slice
```

**未探索路径原文（同文去重）**

```text
  Unexplored path: target call context cannot be expanded within the selected data slice
```

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51_good; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**扩大到第二跳后调用上下文无法展开；安全结论进一步受到未探索路径限制。

**最可能的解法：**修上下文展开及路径切片。

## good / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_rand_add_51__good.ll --function CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`78.13 ms`。
- sink：`CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=unknown；实际等级：`unknown`；判定：`UNK`。
- 失败原因分类：上游上下文无法展开。

**未建模依赖清单原文（同文去重）**

```text
  Unmodeled dependency: unexplored path: target call context cannot be expanded within the selected data slice
```

**未探索路径原文（同文去重）**

```text
  Unexplored path: target call context cannot be expanded within the selected data slice
```

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_rand_add_51_good; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**扩大到第二跳后调用上下文无法展开；安全结论进一步受到未探索路径限制。

**最可能的解法：**修上下文展开及路径切片。

## good 深度突变：报告原文

目标第一次在 `up=1` 从 scoped-unreachable 变为 unknown；`up=2` 才第一次出现调用上下文未探索。以下为目标风险的原始行与对应依赖行：

**up=0** — [日志](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up0.log)

```text
Warning: ArithmeticRisk reason=SignedOverflow feasibility=scoped-unreachable status=scoped-unreachable source-type=signed node=#33 function=CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink context=root:goodB2G/goodB2G:call23 source=E:/Juliet/C/testcases/CWE190_Integer_Overflow/s04/CWE190_Integer_Overflow__int_rand_add_51b.c:51 instruction=  %add = add i32 %data, 1, !dbg !33
```

**up=1** — [日志](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up1.log)

```text
Warning: ArithmeticRisk reason=SignedOverflow feasibility=unknown status=unknown source-type=signed node=#40 function=CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call23 source=E:/Juliet/C/testcases/CWE190_Integer_Overflow/s04/CWE190_Integer_Overflow__int_rand_add_51b.c:51 instruction=  %add = add i32 %data, 1, !dbg !33
  Unmodeled dependency: risk depends on unmodeled control or operands before the target operation
  Unmodeled dependency: unmodeled return value from external call before target: rand
```

**up=2** — [日志](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up2.log)

```text
Warning: ArithmeticRisk reason=SignedOverflow feasibility=unknown status=unknown source-type=signed node=#40 function=CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call23 source=E:/Juliet/C/testcases/CWE190_Integer_Overflow/s04/CWE190_Integer_Overflow__int_rand_add_51b.c:51 instruction=  %add = add i32 %data, 1, !dbg !33
  Unmodeled dependency: unexplored path: target call context cannot be expanded within the selected data slice
  Unexplored path: target call context cannot be expanded within the selected data slice
```

**up=3** — [日志](../logs/CWE190_Integer_Overflow__int_rand_add_51__good__up3.log)

```text
Warning: ArithmeticRisk reason=SignedOverflow feasibility=unknown status=unknown source-type=signed node=#40 function=CWE190_Integer_Overflow__int_rand_add_51b_goodB2GSink context=root:CWE190_Integer_Overflow__int_rand_add_51_good/CWE190_Integer_Overflow__int_rand_add_51_good:call1/goodB2G:call23 source=E:/Juliet/C/testcases/CWE190_Integer_Overflow/s04/CWE190_Integer_Overflow__int_rand_add_51b.c:51 instruction=  %add = add i32 %data, 1, !dbg !33
  Unmodeled dependency: unexplored path: target call context cannot be expanded within the selected data slice
  Unexplored path: target call context cannot be expanded within the selected data slice
```

