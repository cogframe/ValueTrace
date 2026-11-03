# CWE197_Numeric_Truncation_Error__int_rand_to_short_52

- 源文件：`E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52a.c` 等，共 3 个。
- 源码差异：bad 将 rand 来源的 int 转 short；goodG2B 从 CHAR_MAX-5 常量传入同类 sink。
- 目标：最后一级跨文件 sink 的 `data` 参数；CWE190 rand 用 goodB2G，CWE197 与常量组用 goodG2B。
- 最小充分深度：`up=2`（按预先固定的 51/52/54 调用结构）。
- 以下风险等级只取目标 sink；上游运算候选可在原始日志查阅。

## bad / up=0

- 日志：[原始 stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`19.43 ms`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedTruncationLoss=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_badSink:call CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink arg0 context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_badSink call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_badSink:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedTruncationLoss`。
```text
  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink.data i32 unsigned=32768 signed=32768
  Operand operand0 i32 unsigned=32768 signed=32768
  Result: modular-bit-pattern=32768 i16 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink:%entry
  Path: Risk operation:   %conv = trunc i32 %data to i16, !dbg !29 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52c.c:27
```


**人工核对：**目标风险在选定边界下可触发；若来源仍在边界外，此结论仍非完整来源证明。

## bad / up=2

- 日志：[原始 stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up2.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink --param-name data --up 2 --down 3 --report-level 5`
- 退出码：`0`；耗时：`156.91 ms`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedTruncationLoss=unknown；实际等级：`unknown`；判定：`UNK`。
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
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call4 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call4
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call6 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call6
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call9 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call9
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call12 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call12
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call14 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call14
  depth boundary: rand:unresolved return context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad/CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call17 call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad:call17
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**目标 sink 的局部风险存在，但 rand 返回值未建模；未确定有依据，不能算已证实的来源反例。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## bad / up=3

- 日志：[原始 stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`157.21 ms`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedTruncationLoss=unknown；实际等级：`unknown`；判定：`UNK`。
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
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**目标 sink 的局部风险存在，但 rand 返回值未建模；未确定有依据，不能算已证实的来源反例。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## good / up=0

- 日志：[原始 stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.64 ms`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink`；参数：`data`；期望：不应报。
- 目标风险：SignedTruncationLoss=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`OUT_OF_SCOPE`。
- 失败原因分类：其他（安全来源在深度范围外）。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_goodG2BSink:call CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink arg0 context=root:CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_goodG2BSink call=CWE197_Numeric_Truncation_Error__int_rand_to_short_52b_goodG2BSink:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedTruncationLoss`。
```text
  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink.data i32 unsigned=32768 signed=32768
  Operand operand0 i32 unsigned=32768 signed=32768
  Result: modular-bit-pattern=32768 i16 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink
  Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink:%entry
  Path: Risk operation:   %conv = trunc i32 %data to i16, !dbg !27 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_short_52c.c:41
```


**人工核对：**该 SAT 只证明自由边界可触发，good 常量来源尚未纳入；按第二轮规则记 OUT_OF_SCOPE，而非实际 good 误报。

## good / up=2

- 日志：[原始 stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up2.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink --param-name data --up 2 --down 3 --report-level 5`
- 退出码：`0`；耗时：`19.26 ms`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink`；参数：`data`；期望：不应报。
- 目标风险：SignedTruncationLoss=scoped-unreachable；实际等级：`scoped-unreachable`；判定：`TN`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  Model note: Verification root: goodG2B; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**安全常量来源已进入选定范围，目标缩窄风险被排除；与 goodG2B 源码一致。

## good / up=3

- 日志：[原始 stdout/stderr](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE197_Numeric_Truncation_Error__int_rand_to_short_52__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`16.97 ms`。
- sink：`CWE197_Numeric_Truncation_Error__int_rand_to_short_52c_goodG2BSink`；参数：`data`；期望：不应报。
- 目标风险：SignedTruncationLoss=unknown；实际等级：`unknown`；判定：`UNK`。
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
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_short_52_good; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**未建模来源或调用上下文阻止安全证明；需要核对原文，不能当作真实可触发。

**最可能的解法：**修上下文展开及路径切片。

