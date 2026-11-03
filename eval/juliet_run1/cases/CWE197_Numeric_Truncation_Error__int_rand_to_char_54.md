# CWE197_Numeric_Truncation_Error__int_rand_to_char_54

- 源文件：`E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_54a.c` 等，共 5 个。
- 源码差异：bad 将跨五文件的 int 转成 char；goodG2B 用 CHAR_MAX-5 作来源，sink 仍有相同转换。
- 目标：各自最后一个跨文件 sink 的参数 `data`；bad 与 good 的完整函数名见每次命令。
- 期望：bad 应报，good 不应报；good 路径在 CWE190 选 goodB2G，在 CWE197 选 goodG2B。
- 说明：以下只给目标 sink 的风险分级；`up=3` 日志里的上游其他运算不计入本用例目标结论。

## bad / up=0

- 原始日志：[查看](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`17.94 ms`。
- 目标风险：SignedTruncationLoss=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_char_54d_badSink(int):call ?CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink@@YAXH@Z arg0 context=root:?CWE197_Numeric_Truncation_Error__int_rand_to_char_54d_badSink@@YAXH@Z call=?CWE197_Numeric_Truncation_Error__int_rand_to_char_54d_badSink@@YAXH@Z:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink(int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `SignedTruncationLoss`；参数值：`  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink(int).data i32 unsigned=128 signed=128`。
  路径摘要：`  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink(int) |   Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink(int):%entry |   Path: Risk operation:   %conv = trunc i32 %data to i8, !dbg !323 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_54e.c:27`。

**人工核对：**目标 sink 的风险在当前自由边界下可触发；只证明边界条件，不证明 rand 或更上游真实来源可生成反例。

## bad / up=3

- 原始日志：[查看](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_54__bad.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`18.85 ms`。
- 目标风险：SignedTruncationLoss=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad(void):call ?CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_badSink@@YAXH@Z arg0 context=root:?CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad@@YAXXZ call=?CWE197_Numeric_Truncation_Error__int_rand_to_char_54_bad@@YAXXZ:call23
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_badSink(int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `SignedTruncationLoss`；参数值：`  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_badSink(int).data i32 unsigned=128 signed=128`。
  路径摘要：`  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_badSink(int) |   Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_badSink(int):%entry |   Path: Call: CWE197_Numeric_Truncation_Error__int_rand_to_char_54c_badSink(int) at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_54b.c:28 |   Path: Risk operation:   %conv = trunc i32 %data to i8, !dbg !323 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_54e.c:27`。

**人工核对：**目标 sink 的风险在当前自由边界下可触发；只证明边界条件，不证明 rand 或更上游真实来源可生成反例。

## good / up=0

- 原始日志：[查看](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.54 ms`。
- 目标风险：SignedTruncationLoss=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`FP`。
- 失败原因分类：其他（安全来源在 --up 边界外）。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE197_Numeric_Truncation_Error__int_rand_to_char_54d_goodG2BSink(int):call ?CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink@@YAXH@Z arg0 context=root:?CWE197_Numeric_Truncation_Error__int_rand_to_char_54d_goodG2BSink@@YAXH@Z call=?CWE197_Numeric_Truncation_Error__int_rand_to_char_54d_goodG2BSink@@YAXH@Z:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink(int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `SignedTruncationLoss`；参数值：`  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink(int).data i32 unsigned=128 signed=128`。
  路径摘要：`  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink(int) |   Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink(int):%entry |   Path: Risk operation:   %conv = trunc i32 %data to i8, !dbg !322 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_54e.c:41`。

**人工核对：**按指定评分为 FP。当前自由边界未包含 goodG2B 的安全常量来源；反例是边界输入，不证明 Juliet 的 good 执行可触发。

## good / up=3

- 原始日志：[查看](../logs/CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE197_Numeric_Truncation_Error__int_rand_to_char_54__good.ll --function CWE197_Numeric_Truncation_Error__int_rand_to_char_54e_goodG2BSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.00 ms`。
- 目标风险：SignedTruncationLoss=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`FP`。
- 失败原因分类：其他（安全来源在 --up 边界外）。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: goodG2B(void):call ?CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_goodG2BSink@@YAXH@Z arg0 context=root:?goodG2B@@YAXXZ call=?goodG2B@@YAXXZ:call0
  Model note: Verification root: CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_goodG2BSink(int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `SignedTruncationLoss`；参数值：`  Boundary input CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_goodG2BSink(int).data i32 unsigned=128 signed=128`。
  路径摘要：`  Path: Boundary entry: CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_goodG2BSink(int) |   Path: Block: CWE197_Numeric_Truncation_Error__int_rand_to_char_54b_goodG2BSink(int):%entry |   Path: Call: CWE197_Numeric_Truncation_Error__int_rand_to_char_54c_goodG2BSink(int) at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_54b.c:41 |   Path: Risk operation:   %conv = trunc i32 %data to i8, !dbg !322 at E:\Juliet\C\testcases\CWE197_Numeric_Truncation_Error\s01\CWE197_Numeric_Truncation_Error__int_rand_to_char_54e.c:41`。

**人工核对：**按指定评分为 FP。反例来自 54b 的自由边界，安全常量在 54a，与最终 sink 相隔四次调用，未纳入 up=3；不能据此断言 good 源实际会触发。
