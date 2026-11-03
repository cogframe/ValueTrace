# CWE190_Integer_Overflow__int_max_multiply_52

- 源文件：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52a.c` 等，共 3 个。
- 源码差异：bad 从 INT_MAX 常量传入，sink 做 ×2；goodG2B 从常量 2 传入相同算术 sink。
- 目标：最后一级跨文件 sink 的 `data` 参数；CWE190 rand 用 goodB2G，CWE197 与常量组用 goodG2B。
- 最小充分深度：`up=2`（按预先固定的 51/52/54 调用结构）。
- 以下风险等级只取目标 sink；上游运算候选可在原始日志查阅。

## bad / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`23.04 ms`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__int_max_multiply_52b_badSink:call CWE190_Integer_Overflow__int_max_multiply_52c_badSink arg0 context=root:CWE190_Integer_Overflow__int_max_multiply_52b_badSink call=CWE190_Integer_Overflow__int_max_multiply_52b_badSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52c_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Boundary input CWE190_Integer_Overflow__int_max_multiply_52c_badSink.data i32 unsigned=1073741824 signed=1073741824
  Operand operand0 i32 unsigned=1073741824 signed=1073741824
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52c_badSink
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52c_badSink:%entry
  Path: Branch true at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52c.c:24
  Path: Risk operation:   %mul = mul i32 %data, 2, !dbg !26 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52c.c:27
```


**人工核对：**目标风险在选定边界下可触发；若来源仍在边界外，此结论仍非完整来源证明。

## bad / up=2

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__bad__up2.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_badSink --param-name data --up 2 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.00 ms`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=4294967294 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52_bad
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_max_multiply_52b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52a.c:32
  Path: Risk operation:   %mul = mul i32 %data, 2, !dbg !26 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52c.c:27
```


**人工核对：**源码常量已跨调用传至目标；日志的操作数和路径给出具体触发位模式。工具仍标 source-closed=false，不能改称 source-confirmed。

## bad / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_multiply_52__bad.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.04 ms`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=4294967294 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52_bad
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_max_multiply_52b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52a.c:32
  Path: Risk operation:   %mul = mul i32 %data, 2, !dbg !26 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52c.c:27
```


**人工核对：**源码常量已跨调用传至目标；日志的操作数和路径给出具体触发位模式。工具仍标 source-closed=false，不能改称 source-confirmed。

## good / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_multiply_52__good.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`23.08 ms`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`OUT_OF_SCOPE`。
- 失败原因分类：其他（安全来源在深度范围外）。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__int_max_multiply_52b_goodG2BSink:call CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink arg0 context=root:CWE190_Integer_Overflow__int_max_multiply_52b_goodG2BSink call=CWE190_Integer_Overflow__int_max_multiply_52b_goodG2BSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Boundary input CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink.data i32 unsigned=1073741824 signed=1073741824
  Operand operand0 i32 unsigned=1073741824 signed=1073741824
  Operand operand1 i32 unsigned=2 signed=2
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink
  Path: Block: CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink:%entry
  Path: Branch true at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52c.c:39
  Path: Risk operation:   %mul = mul i32 %data, 2, !dbg !33 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_multiply_52c.c:42
```


**人工核对：**该 SAT 只证明自由边界可触发，good 常量来源尚未纳入；按第二轮规则记 OUT_OF_SCOPE，而非实际 good 误报。

## good / up=2

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up2.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_multiply_52__good.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink --param-name data --up 2 --down 3 --report-level 5`
- 退出码：`0`；耗时：`21.52 ms`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=scoped-unreachable；实际等级：`scoped-unreachable`；判定：`TN`。
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

**人工核对：**安全常量 2 已进入选定范围，目标风险被排除；与 goodG2B 源码一致。

## good / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_multiply_52__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_multiply_52__good.ll --function CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`17.05 ms`。
- sink：`CWE190_Integer_Overflow__int_max_multiply_52c_goodG2BSink`；参数：`data`；期望：不应报。
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
  Model note: Verification root: CWE190_Integer_Overflow__int_max_multiply_52_good; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**未建模来源或调用上下文阻止安全证明；需要核对原文，不能当作真实可触发。

**最可能的解法：**修上下文展开及路径切片。

