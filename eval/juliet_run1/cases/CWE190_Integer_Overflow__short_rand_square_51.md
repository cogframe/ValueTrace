# CWE190_Integer_Overflow__short_rand_square_51

- 源文件：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51a.c` 等，共 2 个。
- 源码差异：bad 将 short 的平方存回 short；goodB2G 使用 abs、sqrt 构成上界检查。
- 目标：各自最后一个跨文件 sink 的参数 `data`；bad 与 good 的完整函数名见每次命令。
- 期望：bad 应报，good 不应报；good 路径在 CWE190 选 goodB2G，在 CWE197 选 goodG2B。
- 说明：以下只给目标 sink 的风险分级；`up=3` 日志里的上游其他运算不计入本用例目标结论。

## bad / up=0

- 原始日志：[查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE190_Integer_Overflow__short_rand_square_51__bad.ll --function CWE190_Integer_Overflow__short_rand_square_51b_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`1163.77 ms`。
- 目标风险：SignedOverflow=scoped-unreachable, UnsignedWrap=boundary-triggerable, SignedTruncationLoss=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__short_rand_square_51_bad(void):call ?CWE190_Integer_Overflow__short_rand_square_51b_badSink@@YAXF@Z arg0 context=root:?CWE190_Integer_Overflow__short_rand_square_51_bad@@YAXXZ call=?CWE190_Integer_Overflow__short_rand_square_51_bad@@YAXXZ:call24
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51b_badSink(short); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `UnsignedWrap`；参数值：`  Boundary input CWE190_Integer_Overflow__short_rand_square_51b_badSink(short).data i16 unsigned=62844 signed=-2692`。
  路径摘要：`  Path: Boundary entry: CWE190_Integer_Overflow__short_rand_square_51b_badSink(short) |   Path: Block: CWE190_Integer_Overflow__short_rand_square_51b_badSink(short):%entry |   Path: Risk operation:   %mul = mul i32 %conv, %conv1, !dbg !309 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:28`。
- 风险类别 `SignedTruncationLoss`；参数值：`  Boundary input CWE190_Integer_Overflow__short_rand_square_51b_badSink(short).data i16 unsigned=62047 signed=-3489`。
  路径摘要：`  Path: Boundary entry: CWE190_Integer_Overflow__short_rand_square_51b_badSink(short) |   Path: Block: CWE190_Integer_Overflow__short_rand_square_51b_badSink(short):%entry |   Path: Risk operation:   %conv2 = trunc i32 %mul to i16, !dbg !309 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:28`。

**人工核对：**局部检出成立的是 `SignedTruncationLoss`：short 平方在提升后的 i32 中可表示，但存回 i16 可丢失信息。`UnsignedWrap` 另是未获源码符号性证实的候选，不能算该源码的无符号回绕证明。

## bad / up=3

- 原始日志：[查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE190_Integer_Overflow__short_rand_square_51__bad.ll --function CWE190_Integer_Overflow__short_rand_square_51b_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`161.75 ms`。
- 目标风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；综合等级：`unknown`；判定：`UNK`。
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
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51_bad(void); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 无可采信反例值。

**人工核对：**局部风险已出现，但上游 rand 返回值未建模；未确定理由合理，不能把它记成未检出或已证实可达。

**最可能的解法：**建模外部输入源及其可用约束；必须保留来源未闭合的标记。

## good / up=0

- 原始日志：[查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE190_Integer_Overflow__short_rand_square_51__good.ll --function CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`40.31 ms`。
- 目标风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；综合等级：`unknown`；判定：`UNK`。
- 失败原因分类：间接或外部调用；不支持的指令。

**未建模依赖清单原文（同文去重）**

```text
  Unmodeled dependency: risk depends on unmodeled control or operands before the target operation
  Unmodeled dependency: unmodeled return value from external call before target: llvm.abs.i32
  Unmodeled dependency: unsupported value dependency:   %conv2 = fptosi double %call1 to i32, !dbg !316 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
```

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: goodB2G(void):call ?CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink@@YAXF@Z arg0 context=root:?goodB2G@@YAXXZ call=?goodB2G@@YAXXZ:call24
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink(short); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 无可采信反例值。

**人工核对：**本地 guard 使用 sqrt、llvm.abs 和 fptosi；当前模型不足，未确定理由合理。

**最可能的解法：**补 `fptosi` 等浮点到整数依赖，并处理相关外部函数语义。

## good / up=3

- 原始日志：[查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE190_Integer_Overflow__short_rand_square_51__good.ll --function CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`74.06 ms`。
- 目标风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；综合等级：`unknown`；判定：`UNK`。
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
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51_good(void); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 无可采信反例值。

**人工核对：**目标在 up=0 可被本地 guard 排除，扩大范围后因调用上下文未能展开变成 unknown；没有形成错误的安全证明，但这是需要修的稳定性退化。

**最可能的解法：**修上下文展开。
