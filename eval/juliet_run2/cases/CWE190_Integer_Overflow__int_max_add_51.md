# CWE190_Integer_Overflow__int_max_add_51

- 源文件：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_add_51a.c` 等，共 2 个。
- 源码差异：bad 从 INT_MAX 常量传入，sink 做 +1；goodG2B 从常量 2 传入相同算术 sink。
- 目标：最后一级跨文件 sink 的 `data` 参数；CWE190 rand 用 goodB2G，CWE197 与常量组用 goodG2B。
- 最小充分深度：`up=1`（按预先固定的 51/52/54 调用结构）。
- 以下风险等级只取目标 sink；上游运算候选可在原始日志查阅。

## bad / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_add_51__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_add_51__bad.ll --function CWE190_Integer_Overflow__int_max_add_51b_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.31 ms`。
- sink：`CWE190_Integer_Overflow__int_max_add_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__int_max_add_51_bad:call CWE190_Integer_Overflow__int_max_add_51b_badSink arg0 context=root:CWE190_Integer_Overflow__int_max_add_51_bad call=CWE190_Integer_Overflow__int_max_add_51_bad:call0
  Model note: Verification root: CWE190_Integer_Overflow__int_max_add_51b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Boundary input CWE190_Integer_Overflow__int_max_add_51b_badSink.data i32 unsigned=2147483647 signed=2147483647
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_add_51b_badSink
  Path: Block: CWE190_Integer_Overflow__int_max_add_51b_badSink:%entry
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !23 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_add_51b.c:26
```


**人工核对：**目标风险在选定边界下可触发；若来源仍在边界外，此结论仍非完整来源证明。

## bad / up=1

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_add_51__bad__up1.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_add_51__bad.ll --function CWE190_Integer_Overflow__int_max_add_51b_badSink --param-name data --up 1 --down 3 --report-level 5`
- 退出码：`0`；耗时：`18.04 ms`。
- sink：`CWE190_Integer_Overflow__int_max_add_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_max_add_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_add_51_bad
  Path: Block: CWE190_Integer_Overflow__int_max_add_51_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_max_add_51b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_add_51a.c:32
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !23 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_add_51b.c:26
```


**人工核对：**源码常量已跨调用传至目标；日志的操作数和路径给出具体触发位模式。工具仍标 source-closed=false，不能改称 source-confirmed。

## bad / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_add_51__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_add_51__bad.ll --function CWE190_Integer_Overflow__int_max_add_51b_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`18.46 ms`。
- sink：`CWE190_Integer_Overflow__int_max_add_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_max_add_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_add_51_bad
  Path: Block: CWE190_Integer_Overflow__int_max_add_51_bad:%entry
  Path: Call: CWE190_Integer_Overflow__int_max_add_51b_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_add_51a.c:32
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !23 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_add_51b.c:26
```


**人工核对：**源码常量已跨调用传至目标；日志的操作数和路径给出具体触发位模式。工具仍标 source-closed=false，不能改称 source-confirmed。

## good / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_add_51__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_add_51__good.ll --function CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`24.07 ms`。
- sink：`CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`OUT_OF_SCOPE`。
- 失败原因分类：其他（安全来源在深度范围外）。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: goodG2B:call CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink arg0 context=root:goodG2B call=goodG2B:call0
  Model note: Verification root: CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`SignedOverflow`。
```text
  Boundary input CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink.data i32 unsigned=2147483647 signed=2147483647
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink
  Path: Block: CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink:%entry
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !30 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_max_add_51b.c:40
```


**人工核对：**该 SAT 只证明自由边界可触发，good 常量来源尚未纳入；按第二轮规则记 OUT_OF_SCOPE，而非实际 good 误报。

## good / up=1

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_add_51__good__up1.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_add_51__good.ll --function CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink --param-name data --up 1 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.27 ms`。
- sink：`CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink`；参数：`data`；期望：不应报。
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

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__int_max_add_51__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__int_max_add_51__good.ll --function CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`17.13 ms`。
- sink：`CWE190_Integer_Overflow__int_max_add_51b_goodG2BSink`；参数：`data`；期望：不应报。
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
  Model note: Verification root: CWE190_Integer_Overflow__int_max_add_51_good; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**未建模来源或调用上下文阻止安全证明；需要核对原文，不能当作真实可触发。

**最可能的解法：**修上下文展开及路径切片。

