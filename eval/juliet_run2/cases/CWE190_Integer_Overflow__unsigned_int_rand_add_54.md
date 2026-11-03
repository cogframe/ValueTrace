# CWE190_Integer_Overflow__unsigned_int_rand_add_54

- 源文件：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__unsigned_int_rand_add_54a.c` 等，共 5 个。
- 源码差异：bad 用 rand 来源做 unsigned int + 1；goodB2G 在加法前检查 data < UINT_MAX。
- 目标：最后一级跨文件 sink 的 `data` 参数；CWE190 rand 用 goodB2G，CWE197 与常量组用 goodG2B。
- 最小充分深度：`up=4`（按预先固定的 51/52/54 调用结构）。
- 以下风险等级只取目标 sink；上游运算候选可在原始日志查阅。

## bad / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.21 ms`。
- sink：`CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink`；参数：`data`；期望：应报。
- 目标风险：UnsignedWrap=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__unsigned_int_rand_add_54d_badSink:call CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink arg0 context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54d_badSink call=CWE190_Integer_Overflow__unsigned_int_rand_add_54d_badSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`UnsignedWrap`。
```text
  Boundary input CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink.data i32 unsigned=4294967295 signed=-1
  Operand operand0 i32 unsigned=4294967295 signed=-1
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=0 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink
  Path: Block: CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink:%entry
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !31 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__unsigned_int_rand_add_54e.c:26
```


**人工核对：**目标风险在选定边界下可触发；若来源仍在边界外，此结论仍非完整来源证明。

## bad / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`20.67 ms`。
- sink：`CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink`；参数：`data`；期望：应报。
- 目标风险：UnsignedWrap=boundary-triggerable；实际等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink arg0 context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad call=CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call23
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 类别：`UnsignedWrap`。
```text
  Boundary input CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink.data i32 unsigned=4294967295 signed=-1
  Operand operand0 i32 unsigned=4294967295 signed=-1
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=0 i32 ir-value=defined
```

  路径摘要：
```text
  Path: Boundary entry: CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink
  Path: Block: CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink:%entry
  Path: Call: CWE190_Integer_Overflow__unsigned_int_rand_add_54c_badSink at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__unsigned_int_rand_add_54b.c:27
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !31 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__unsigned_int_rand_add_54e.c:26
```


**人工核对：**目标风险在选定边界下可触发；若来源仍在边界外，此结论仍非完整来源证明。

## bad / up=4

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up4.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink --param-name data --up 4 --down 3 --report-level 5`
- 退出码：`0`；耗时：`147.85 ms`。
- sink：`CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink`；参数：`data`；期望：应报。
- 目标风险：UnsignedWrap=unknown；实际等级：`unknown`；判定：`UNK`。
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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad/CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call4 call=CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad/CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call6 call=CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad/CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call9 call=CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad/CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call12 call=CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad/CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call14 call=CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad/CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call17 call=CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad:call17
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**目标 sink 的局部风险存在，但 rand 返回值未建模；未确定有依据，不能算已证实的来源反例。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

## good / up=0

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__good.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`23.33 ms`。
- sink：`CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：UnsignedWrap=scoped-unreachable；实际等级：`scoped-unreachable`；判定：`TN`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__unsigned_int_rand_add_54d_goodB2GSink:call CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink arg0 context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54d_goodB2GSink call=CWE190_Integer_Overflow__unsigned_int_rand_add_54d_goodB2GSink:call0
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**本地 guard 排除风险路径；与 goodB2G 源码一致。

## good / up=3

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__good.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`23.11 ms`。
- sink：`CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：UnsignedWrap=scoped-unreachable；实际等级：`scoped-unreachable`；判定：`TN`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: goodB2G:call CWE190_Integer_Overflow__unsigned_int_rand_add_54b_goodB2GSink arg0 context=root:goodB2G call=goodB2G:call23
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54b_goodB2GSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**本地 guard 排除风险路径；与 goodB2G 源码一致。

## good / up=4

- 日志：[原始 stdout/stderr](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up4.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run2\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__good.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink --param-name data --up 4 --down 3 --report-level 5`
- 退出码：`0`；耗时：`295.51 ms`。
- sink：`CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink`；参数：`data`；期望：不应报。
- 目标风险：UnsignedWrap=unknown；实际等级：`unknown`；判定：`UNK`。
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
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_good/CWE190_Integer_Overflow__unsigned_int_rand_add_54_good:call1/goodB2G:call4 call=goodB2G:call4
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_good/CWE190_Integer_Overflow__unsigned_int_rand_add_54_good:call1/goodB2G:call6 call=goodB2G:call6
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_good/CWE190_Integer_Overflow__unsigned_int_rand_add_54_good:call1/goodB2G:call9 call=goodB2G:call9
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_good/CWE190_Integer_Overflow__unsigned_int_rand_add_54_good:call1/goodB2G:call12 call=goodB2G:call12
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_good/CWE190_Integer_Overflow__unsigned_int_rand_add_54_good:call1/goodB2G:call14 call=goodB2G:call14
  depth boundary: rand:unresolved return context=root:CWE190_Integer_Overflow__unsigned_int_rand_add_54_good/CWE190_Integer_Overflow__unsigned_int_rand_add_54_good:call1/goodB2G:call17 call=goodB2G:call17
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例值、风险类别与路径摘要**

- 无可采信反例值。

**人工核对：**未建模来源或调用上下文阻止安全证明；需要核对原文，不能当作真实可触发。

**最可能的解法：**建模 rand/RAND32 外部输入及其可证明范围。

