# CWE190_Integer_Overflow__short_rand_square_51

- 源文件：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51a.c` 等，共 2 个。
- 源码差异：bad 将 short 的平方存回 short；goodB2G 用 abs/sqrt 检查上界。
- 目标：最后一级跨文件 sink 的 `data` 参数；CWE190 rand 用 goodB2G，CWE197 与常量组用 goodG2B。
- 最小充分深度：`up=1`（按预先固定的 51/52/54 调用结构）。
- 以下风险等级只取目标 sink；上游运算候选可在原始日志查阅。

## bad / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__short_rand_square_51__bad.ll --function CWE190_Integer_Overflow__short_rand_square_51b_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`1172.19 ms`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=scoped-unreachable, UnsignedWrap=boundary-triggerable, SignedTruncationLoss=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__short_rand_square_51_bad:call CWE190_Integer_Overflow__short_rand_square_51b_badSink arg0 context=root:CWE190_Integer_Overflow__short_rand_square_51_bad call=CWE190_Integer_Overflow__short_rand_square_51_bad:call24
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`UnsignedWrap`。
```text
  Boundary input CWE190_Integer_Overflow__short_rand_square_51b_badSink.data i16 unsigned=62844 signed=-2692
  Operand operand0 i32 unsigned=4294964604 signed=-2692
  Operand operand1 i32 unsigned=4294964604 signed=-2692
  Result: modular-bit-pattern=7246864 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__short_rand_square_51b_badSink
  Path: Block: CWE190_Integer_Overflow__short_rand_square_51b_badSink:%entry
  Path: Risk operation:   %mul = mul i32 %conv, %conv1, !dbg !26 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:28
```

- 类别：`SignedTruncationLoss`。
```text
  Boundary input CWE190_Integer_Overflow__short_rand_square_51b_badSink.data i16 unsigned=62047 signed=-3489
  Operand operand0 i32 unsigned=12173121 signed=12173121
  Result: modular-bit-pattern=48961 i16 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__short_rand_square_51b_badSink
  Path: Block: CWE190_Integer_Overflow__short_rand_square_51b_badSink:%entry
  Path: Risk operation:   %conv2 = trunc i32 %mul to i16, !dbg !26 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:28
```


**人工核对：**有效的源码风险是 short 平方后的 SignedTruncationLoss；无符号回绕只是来源符号性未证实的候选。

## bad / up=1

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up1.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__short_rand_square_51__bad.ll --function CWE190_Integer_Overflow__short_rand_square_51b_badSink --param-name data --up 1 --down 3 --report-level 5`
- 退出码：`0`；耗时：`180.56 ms`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；实际等级：`unknown`；判定：`UNK`。
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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call4 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call6 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call9 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call12 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call14 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_bad/CWE190_Integer_Overflow__short_rand_square_51_bad:call17 call=CWE190_Integer_Overflow__short_rand_square_51_bad:call17
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**目标 sink 的局部风险存在，但 rand 返回值未建模；未确定有依据，不能算已证实的来源反例。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## bad / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__short_rand_square_51__bad.ll --function CWE190_Integer_Overflow__short_rand_square_51b_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`184.00 ms`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_badSink`；参数：`data`；期望：应报。
- 目标风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；实际等级：`unknown`；判定：`UNK`。
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
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**目标 sink 的局部风险存在，但 rand 返回值未建模；未确定有依据，不能算已证实的来源反例。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## good / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__short_rand_square_51__good.ll --function CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`44.93 ms`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；实际等级：`unknown`；判定：`UNK`。
- 失败原因分类：间接或外部调用；不支持的指令。

**未建模依赖清单原文（同文去重）**

```text
  Unmodeled dependency: risk depends on unmodeled control or operands before the target operation
  Unmodeled dependency: unmodeled return value from external call before target: llvm.abs.i32
  Unmodeled dependency: unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
```

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: goodB2G:call CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink arg0 context=root:goodB2G call=goodB2G:call24
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**guard 使用 abs、sqrt 与浮点转整数；未建模依赖阻止安全证明。

**最可能的解法：**补浮点到整数指令以及 guard 使用的外部函数语义。

## good / up=1

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__good__up1.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__short_rand_square_51__good.ll --function CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink --param-name data --up 1 --down 3 --report-level 5`
- 退出码：`0`；耗时：`242.91 ms`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；实际等级：`unknown`；判定：`UNK`。
- 失败原因分类：外部输入源未建模。

**未建模依赖清单原文（同文去重）**

```text
  Unmodeled dependency: risk depends on unmodeled control or operands before the target operation
  Unmodeled dependency: unmodeled return value from external call before target: llvm.abs.i32
  Unmodeled dependency: unmodeled return value from external call before target: rand
  Unmodeled dependency: unsupported value dependency:   %conv1 = fptosi double %call to i32, !dbg !35 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__short_rand_square_51b.c:51
```

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call4 call=goodB2G:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call6 call=goodB2G:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call9 call=goodB2G:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call12 call=goodB2G:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call14 call=goodB2G:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__short_rand_square_51_good/CWE190_Integer_Overflow__short_rand_square_51_good:call1/goodB2G:call17 call=goodB2G:call17
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**未建模来源或调用上下文阻止安全证明；需要核对原文，不能当作真实可触发。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## good / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__short_rand_square_51__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__short_rand_square_51__good.ll --function CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`90.09 ms`。
- sink：`CWE190_Integer_Overflow__short_rand_square_51b_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：SignedOverflow=unknown, UnsignedWrap=unknown, SignedTruncationLoss=unknown；实际等级：`unknown`；判定：`UNK`。
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
  Model note: Verification root: CWE190_Integer_Overflow__short_rand_square_51_good; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**未建模来源或调用上下文阻止安全证明；需要核对原文，不能当作真实可触发。

**最可能的解法：**修上下文展开及路径切片。

## bad / up=0 五次重复测时

| 次数 | 耗时 ms | 等级 | 原始日志 |
|---:|---:|---|---|
| 1 | 1176.85 | `boundary-triggerable` | [查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat1.log) |
| 2 | 1179.93 | `boundary-triggerable` | [查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat2.log) |
| 3 | 1170.67 | `boundary-triggerable` | [查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat3.log) |
| 4 | 1176.60 | `boundary-triggerable` | [查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat4.log) |
| 5 | 1171.65 | `boundary-triggerable` | [查看](../logs/CWE190_Integer_Overflow__short_rand_square_51__bad__up0__repeat5.log) |

最小 1170.67 ms；中位数 1176.60 ms；平均 1175.14 ms；最大 1179.93 ms；样本标准差 3.88 ms。

