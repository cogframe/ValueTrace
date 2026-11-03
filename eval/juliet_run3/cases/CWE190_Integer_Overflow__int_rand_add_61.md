# CWE190_Integer_Overflow__int_rand_add_61

- 源码：`E:\Juliet\C\testcases\CWE190_Integer_Overflow\s04\CWE190_Integer_Overflow__int_rand_add_61a.c` 等 2 个文件。
- 差异：_61 通过跨文件函数返回值传递；所选 source 参数在函数内被覆盖，风险在调用者内。
- 最小深度：`up=1`；新增深度：`up=3`。
- 分类：内存模型基线，不计入失败。

## bad / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_add_61__bad.ll --function CWE190_Integer_Overflow__int_rand_add_61b_badSource --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_badSource`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`9.36 ms`；实际：`no-target-candidate`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（所选参数未连接返回值风险）。

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
  depth boundary: CWE190_Integer_Overflow__int_rand_add_61_bad:call CWE190_Integer_Overflow__int_rand_add_61b_badSource arg0 context=root:CWE190_Integer_Overflow__int_rand_add_61_bad call=CWE190_Integer_Overflow__int_rand_add_61_bad:call0
```

**反例与关键值原文**

```text
（无）
```

人工核对：所选 source 形参被覆盖，风险位于调用者且未进入该参数的传播切片；无候选不代表安全。

## bad / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_add_61__bad.ll --function CWE190_Integer_Overflow__int_rand_add_61b_badSource --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_badSource`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`9.43 ms`；实际：`no-target-candidate`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（所选参数未连接返回值风险）。

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
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：所选 source 形参被覆盖，风险位于调用者且未进入该参数的传播切片；无候选不代表安全。

## bad / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__bad__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_add_61__bad.ll --function CWE190_Integer_Overflow__int_rand_add_61b_badSource --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_badSource`，目标参数：`data`，期望：应报。
- 退出码：`0`；耗时：`9.89 ms`；实际：`no-target-candidate`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（所选参数未连接返回值风险）。

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
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：所选 source 形参被覆盖，风险位于调用者且未进入该参数的传播切片；无候选不代表安全。

## good / up=0

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__good__up0.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_add_61__good.ll --function CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource --param-name data --up 0 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`10.24 ms`；实际：`no-target-candidate`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（所选参数未连接返回值风险）。

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
  depth boundary: goodG2B:call CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource arg0 context=root:goodG2B call=goodG2B:call0
```

**反例与关键值原文**

```text
（无）
```

人工核对：所选 source 形参被覆盖，风险位于调用者且未进入该参数的传播切片；无候选不代表安全。

## good / up=1

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__good__up1.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_add_61__good.ll --function CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource --param-name data --up 1 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`11.15 ms`；实际：`no-target-candidate`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（所选参数未连接返回值风险）。

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
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：所选 source 形参被覆盖，风险位于调用者且未进入该参数的传播切片；无候选不代表安全。

## good / up=3

- 原始日志：[stdout/stderr](../logs/CWE190_Integer_Overflow__int_rand_add_61__good__up3.log)；完整命令：`E:\ValueTrace\build-msvc\valuetrace.exe verify E:\ValueTrace\eval\juliet_run3\ir\CWE190_Integer_Overflow__int_rand_add_61__good.ll --function CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource --param-name data --up 3 --down 3 --report-level 5`。
- sink：`CWE190_Integer_Overflow__int_rand_add_61b_goodG2BSource`，目标参数：`data`，期望：不应报。
- 退出码：`0`；耗时：`10.53 ms`；实际：`no-target-candidate`；判定：`BASELINE`。
- 风险：无目标候选；原因：其他（所选参数未连接返回值风险）。

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
（无）
```

**反例与关键值原文**

```text
（无）
```

人工核对：所选 source 形参被覆盖，风险位于调用者且未进入该参数的传播切片；无候选不代表安全。

