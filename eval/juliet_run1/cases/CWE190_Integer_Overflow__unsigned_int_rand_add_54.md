# CWE190_Integer_Overflow__unsigned_int_rand_add_54

- 源文件：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__unsigned_int_rand_add_54a.c` 等，共 5 个。
- 源码差异：bad 跨五文件传入后做 unsigned int + 1；goodB2G 检查 data < UINT_MAX。
- 目标：各自最后一个跨文件 sink 的参数 `data`；bad 与 good 的完整函数名见每次命令。
- 期望：bad 应报，good 不应报；good 路径在 CWE190 选 goodB2G，在 CWE197 选 goodG2B。
- 说明：以下只给目标 sink 的风险分级；`up=3` 日志里的上游其他运算不计入本用例目标结论。

## bad / up=0

- 原始日志：[查看](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`18.32 ms`。
- 目标风险：UnsignedWrap=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__unsigned_int_rand_add_54d_badSink(unsigned int):call ?CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink@@YAXI@Z arg0 context=root:?CWE190_Integer_Overflow__unsigned_int_rand_add_54d_badSink@@YAXI@Z call=?CWE190_Integer_Overflow__unsigned_int_rand_add_54d_badSink@@YAXI@Z:call0
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink(unsigned int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `UnsignedWrap`；参数值：`  Boundary input CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink(unsigned int).data i32 unsigned=4294967295 signed=-1`。
  路径摘要：`  Path: Boundary entry: CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink(unsigned int) |   Path: Block: CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink(unsigned int):%entry |   Path: Risk operation:   %add = add i32 %data, 1, !dbg !322 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__unsigned_int_rand_add_54e.c:26`。

**人工核对：**目标 sink 的风险在当前自由边界下可触发；只证明边界条件，不证明 rand 或更上游真实来源可生成反例。

## bad / up=3

- 原始日志：[查看](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__bad.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_badSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`19.05 ms`。
- 目标风险：UnsignedWrap=boundary-triggerable；综合等级：`boundary-triggerable`；判定：`TP`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad(void):call ?CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink@@YAXI@Z arg0 context=root:?CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad@@YAXXZ call=?CWE190_Integer_Overflow__unsigned_int_rand_add_54_bad@@YAXXZ:call23
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink(unsigned int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 风险类别 `UnsignedWrap`；参数值：`  Boundary input CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink(unsigned int).data i32 unsigned=4294967295 signed=-1`。
  路径摘要：`  Path: Boundary entry: CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink(unsigned int) |   Path: Block: CWE190_Integer_Overflow__unsigned_int_rand_add_54b_badSink(unsigned int):%entry |   Path: Call: CWE190_Integer_Overflow__unsigned_int_rand_add_54c_badSink(unsigned int) at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__unsigned_int_rand_add_54b.c:27 |   Path: Risk operation:   %add = add i32 %data, 1, !dbg !322 at E:\Juliet\C\testcases\CWE190_Integer_Overflow\s05\CWE190_Integer_Overflow__unsigned_int_rand_add_54e.c:26`。

**人工核对：**目标 sink 的风险在当前自由边界下可触发；只证明边界条件，不证明 rand 或更上游真实来源可生成反例。

## good / up=0

- 原始日志：[查看](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up0.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__good.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink --param-name data --up 0 --down 3 --report-level 5`
- 退出码：`0`；耗时：`23.43 ms`。
- 目标风险：UnsignedWrap=scoped-unreachable；综合等级：`scoped-unreachable`；判定：`TN`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: CWE190_Integer_Overflow__unsigned_int_rand_add_54d_goodB2GSink(unsigned int):call ?CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink@@YAXI@Z arg0 context=root:?CWE190_Integer_Overflow__unsigned_int_rand_add_54d_goodB2GSink@@YAXI@Z call=?CWE190_Integer_Overflow__unsigned_int_rand_add_54d_goodB2GSink@@YAXI@Z:call0
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink(unsigned int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 无可采信反例值。

**人工核对：**目标运算受本地 guard 约束，范围内不可触发；与所选 goodB2G 路径一致。

## good / up=3

- 原始日志：[查看](../logs/CWE190_Integer_Overflow__unsigned_int_rand_add_54__good__up3.log)。
- 完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run1\ir\CWE190_Integer_Overflow__unsigned_int_rand_add_54__good.ll --function CWE190_Integer_Overflow__unsigned_int_rand_add_54e_goodB2GSink --param-name data --up 3 --down 3 --report-level 5`
- 退出码：`0`；耗时：`22.31 ms`。
- 目标风险：UnsignedWrap=scoped-unreachable；综合等级：`scoped-unreachable`；判定：`TN`。
- 失败原因分类：无。

**未建模依赖清单原文（同文去重）**

无。

**未探索路径原文（同文去重）**

无。

**停止边界原文**

```text
  depth boundary: goodB2G(void):call ?CWE190_Integer_Overflow__unsigned_int_rand_add_54b_goodB2GSink@@YAXI@Z arg0 context=root:?goodB2G@@YAXXZ call=?goodB2G@@YAXXZ:call23
  Model note: Verification root: CWE190_Integer_Overflow__unsigned_int_rand_add_54b_goodB2GSink(unsigned int); integer entry inputs use their full bit-width domains. Constraints before this boundary are not assumed.
```

**反例及路径**

- 无可采信反例值。

**人工核对：**目标运算受本地 guard 约束，范围内不可触发；与所选 goodB2G 路径一致。
