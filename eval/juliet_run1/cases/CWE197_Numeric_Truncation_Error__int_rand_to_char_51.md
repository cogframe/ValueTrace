# CWE197_Numeric_Truncation_Error__int_rand_to_char_51

- 源文件：`E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_51a.c` 等，共 2 个。
- 源码差异：bad 将 rand 来源的 int 转成 char；goodG2B 用 CHAR_MAX-5 作来源，sink 仍有相同转换。
- 目标：各自最后一个跨文件 sink 的参数 `data`；bad 与 good 的完整函数名见每次命令。
- 期望：bad 应报，good 不应报；good 路径在 CWE190 选 goodB2G，在 CWE197 选 goodG2B。
- 说明：以下只给目标 sink 的风险分级；`up=3` 日志里的上游其他运算不计入本用例目标结论。

## bad / up=0

- 原始日志：[查看](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`21.31 ms`。
- 目标风险：SignedTruncationLoss=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad(void):call ?CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink@@YAXH@Z arg0 context=root:?CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad@@YAXXZ call=?CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad@@YAXXZ:call23
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink(int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `SignedTruncationLoss`；参数值：`  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink(int).data i32 unsigned=128 signed=128`。
  路径摘要：`  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink(int) |   Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink(int):%entry |   Path: Risk operation:   %conv = trunc i32 %data to i8, !dbg !308 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_51b.c:27`。

**人工核对：**目标 sink 的风险在当前自由边界下可触发；只证明边界条件，不证明 rand 或更上游真实来源可生成反例。

## bad / up=3

- 原始日志：[查看](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`127.64 ms`。
- 目标风险：SignedTruncationLoss=unknown；综合等级：`unknown`；判定：`UNK`。
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
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_bad(void); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 无可采信反例值。

**人工核对：**局部风险已出现，但上游 rand 返回值未建模；未确定理由合理，不能把它记成未检出或已证实可达。

**最可能的解法：**建模外部输入源及其可用约束；必须保留来源未闭合的标记。

## good / up=0

- 原始日志：[查看](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`19.08 ms`。
- 目标风险：SignedTruncationLoss=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`FP`。
- 失败原因分类：其他（安全来源在 --up 边界外）。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: goodG2B(void):call ?CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink@@YAXH@Z arg0 context=root:?goodG2B@@YAXXZ call=?goodG2B@@YAXXZ:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink(int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `SignedTruncationLoss`；参数值：`  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink(int).data i32 unsigned=128 signed=128`。
  路径摘要：`  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink(int) |   Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink(int):%entry |   Path: Risk operation:   %conv = trunc i32 %data to i8, !dbg !307 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_51b.c:41`。

**人工核对：**按指定评分为 FP。当前自由边界未包含 goodG2B 的安全常量来源；反例是边界输入，不证明 Juliet 的 good 执行可触发。

## good / up=3

- 原始日志：[查看](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_51__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_51b_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`15.50 ms`。
- 目标风险：SignedTruncationLoss=unknown；综合等级：`unknown`；判定：`UNK`。
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
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_51_good(void); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 无可采信反例值。

**人工核对：**上游调用上下文未能展开，安全常量无法用于排除截断；未确定理由与日志一致。

**最可能的解法：**修上下文展开。
