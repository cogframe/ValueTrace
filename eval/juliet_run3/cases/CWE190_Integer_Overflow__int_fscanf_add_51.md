# CWE190_Integer_Overflow__int_fscanf_add_51

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c` 等 2 个文件。
- 差异：fscanf 将值写入局部变量；bad 在 sink 加 1，goodB2G 在加法前检查上界。
- 最小深度：`up=1`；新增深度：`up=3`。
- 分类：常规评估。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_fscanf_add_51__bad.ll --function CWE190_Integer_Overflow__int_fscanf_add_51b_badSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_fscanf_add_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`23.38 ms`；实际：`boundary-triggerable`；判定：`TP`。
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
  depth boundary: CWE190_Integer_Overflow__int_fscanf_add_51_bad:call CWE190_Integer_Overflow__int_fscanf_add_51b_badSink arg0 context=root:CWE190_Integer_Overflow__int_fscanf_add_51_bad call=CWE190_Integer_Overflow__int_fscanf_add_51_bad:call5
  Model note: Verification root: CWE190_Integer_Overflow__int_fscanf_add_51b_badSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例与关键值原文**

```text
  Boundary input CWE190_Integer_Overflow__int_fscanf_add_51b_badSink.data i32 unsigned=2147483647 signed=2147483647
  Operand operand0 i32 unsigned=2147483647 signed=2147483647
  Operand operand1 i32 unsigned=1 signed=1
  Result: modular-bit-pattern=2147483648 i32 ir-value=defined
  Path: Boundary entry: CWE190_Integer_Overflow__int_fscanf_add_51b_badSink
  Path: Block: CWE190_Integer_Overflow__int_fscanf_add_51b_badSink:%entry
  Path: Risk operation:   %add = add i32 %data, 1, !dbg !39 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51b.c:26
```

人工核对：与源码预期一致，但边界反例未证明真实外部输入范围。

## bad / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_fscanf_add_51__bad.ll --function CWE190_Integer_Overflow__int_fscanf_add_51b_badSink --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_fscanf_add_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`24.13 ms`；实际：`unknown`；判定：`UNK`。
- 风险：SignedOverflow=unknown；原因：内存未建模、不支持的指令、间接或外部调用。

**依赖清单原文**

```text
Unmodeled dependency: memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:29
Unmodeled dependency: unmodeled pointer/global write by call dependency outside selected data slice: fscanf
Unmodeled dependency: unmodeled pointer/global write by external call before target: __acrt_iob_func
Unmodeled dependency: unsupported value dependency:   %0 = load i32, ptr %data, align 4, !dbg !40 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:32
```

**过近似清单原文**

```text
Over-approximation dependency: memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:29
Over-approximation dependency: unmodeled pointer/global write by call dependency outside selected data slice: fscanf
Over-approximation dependency: unmodeled pointer/global write by external call before target: __acrt_iob_func
```

**丢弃清单原文**

```text
Discarded dependency: unsupported value dependency:   %0 = load i32, ptr %data, align 4, !dbg !40 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:32
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_fscanf_add_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
（无）
```

人工核对：未确定；内存未建模、不支持的指令、间接或外部调用。最可能的后续工作是补齐对应模型或指令。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_fscanf_add_51__bad.ll --function CWE190_Integer_Overflow__int_fscanf_add_51b_badSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_fscanf_add_51b_badSink`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`25.60 ms`；实际：`unknown`；判定：`UNK`。
- 风险：SignedOverflow=unknown；原因：内存未建模、不支持的指令、间接或外部调用。

**依赖清单原文**

```text
Unmodeled dependency: memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:29
Unmodeled dependency: unmodeled pointer/global write by call dependency outside selected data slice: fscanf
Unmodeled dependency: unmodeled pointer/global write by external call before target: __acrt_iob_func
Unmodeled dependency: unsupported value dependency:   %0 = load i32, ptr %data, align 4, !dbg !40 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:32
```

**过近似清单原文**

```text
Over-approximation dependency: memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:29
Over-approximation dependency: unmodeled pointer/global write by call dependency outside selected data slice: fscanf
Over-approximation dependency: unmodeled pointer/global write by external call before target: __acrt_iob_func
```

**丢弃清单原文**

```text
Discarded dependency: unsupported value dependency:   %0 = load i32, ptr %data, align 4, !dbg !40 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:32
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
  Model note: Verification root: CWE190_Integer_Overflow__int_fscanf_add_51_bad; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
（无）
```

人工核对：未确定；内存未建模、不支持的指令、间接或外部调用。最可能的后续工作是补齐对应模型或指令。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_fscanf_add_51__good.ll --function CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`27.82 ms`；实际：`scoped-unreachable`；判定：`TN`。
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
  depth boundary: goodB2G:call CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink arg0 context=root:goodB2G call=goodB2G:call5
  Model note: Verification root: CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
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

## good / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_fscanf_add_51__good.ll --function CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`26.84 ms`；实际：`unknown`；判定：`UNK`。
- 风险：SignedOverflow=unknown；原因：内存未建模、不支持的指令、间接或外部调用。

**依赖清单原文**

```text
Unmodeled dependency: memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:59
Unmodeled dependency: unmodeled pointer/global write by call dependency outside selected data slice: fscanf
Unmodeled dependency: unmodeled pointer/global write by external call before target: __acrt_iob_func
Unmodeled dependency: unsupported value dependency:   %0 = load i32, ptr %data, align 4, !dbg !47 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:62
```

**过近似清单原文**

```text
Over-approximation dependency: memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:59
Over-approximation dependency: unmodeled pointer/global write by call dependency outside selected data slice: fscanf
Over-approximation dependency: unmodeled pointer/global write by external call before target: __acrt_iob_func
```

**丢弃清单原文**

```text
Discarded dependency: unsupported value dependency:   %0 = load i32, ptr %data, align 4, !dbg !47 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:62
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
（无）
```

人工核对：未确定；内存未建模、不支持的指令、间接或外部调用。最可能的后续工作是补齐对应模型或指令。

## good / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_fscanf_add_51__good.ll --function CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_fscanf_add_51b_goodB2GSink`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`26.16 ms`；实际：`unknown`；判定：`UNK`。
- 风险：SignedOverflow=unknown；原因：内存未建模、不支持的指令、间接或外部调用。

**依赖清单原文**

```text
Unmodeled dependency: memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:59
Unmodeled dependency: unmodeled pointer/global write by call dependency outside selected data slice: fscanf
Unmodeled dependency: unmodeled pointer/global write by external call before target: __acrt_iob_func
Unmodeled dependency: unsupported value dependency:   %0 = load i32, ptr %data, align 4, !dbg !47 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:62
```

**过近似清单原文**

```text
Over-approximation dependency: memory side effect before target not modeled at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:59
Over-approximation dependency: unmodeled pointer/global write by call dependency outside selected data slice: fscanf
Over-approximation dependency: unmodeled pointer/global write by external call before target: __acrt_iob_func
```

**丢弃清单原文**

```text
Discarded dependency: unsupported value dependency:   %0 = load i32, ptr %data, align 4, !dbg !47 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s03\CWE190_Integer_Overflow__int_fscanf_add_51a.c:62
```

**未探索路径原文**

```text
（无）
```

**停止边界原文**

```text
  Model note: Verification root: goodB2G; integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
  Model note: Data slice closed at upstream depth 1: source function has no parameters and its caller passes no data
```

**反例与关键值原文**

```text
（无）
```

人工核对：未确定；内存未建模、不支持的指令、间接或外部调用。最可能的后续工作是补齐对应模型或指令。

