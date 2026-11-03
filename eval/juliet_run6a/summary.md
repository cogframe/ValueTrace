# 第五轮 6a：指针参数与解引用

- 固定预期先写入 `examples/memory_acceptance/ACCEPTANCE.md`；仅运行 6a 用例。
- CTest：35/35 个已启用测试通过；`memory_acceptance_later` 禁用。
- 第三、四轮各 113 条固定命令，分别在 `ub-free` 与 `wrap` 下重跑；每轮 226 份日志。
- 结果口径为目标风险的总体等级；内存证据逐条见 [memory_evidence.md](memory_evidence.md)。

## 阶段验收表

| 用例 | up | 模式 | 预期 | 实际 | 通过 | 耗时 ms | 日志 |
|---|---:|---|---|---|---|---:|---|
| bad | 0 | `ub-free` | `boundary-triggerable` | `boundary-triggerable` | 是 | 21.34 | [日志](stage6a/63_bad_up0_ub-free.log) |
| bad | 0 | `wrap` | `boundary-triggerable` | `boundary-triggerable` | 是 | 20.88 | [日志](stage6a/63_bad_up0_wrap.log) |
| bad | 1 | `ub-free` | `boundary-triggerable` | `boundary-triggerable` | 是 | 25.83 | [日志](stage6a/63_bad_up1_ub-free.log) |
| bad | 1 | `wrap` | `boundary-triggerable` | `boundary-triggerable` | 是 | 26.83 | [日志](stage6a/63_bad_up1_wrap.log) |
| bad | 3 | `ub-free` | `boundary-triggerable` | `boundary-triggerable` | 是 | 26.17 | [日志](stage6a/63_bad_up3_ub-free.log) |
| bad | 3 | `wrap` | `boundary-triggerable` | `boundary-triggerable` | 是 | 26.47 | [日志](stage6a/63_bad_up3_wrap.log) |
| good | 0 | `ub-free` | `scoped-unreachable` | `scoped-unreachable` | 是 | 34.46 | [日志](stage6a/63_good_up0_ub-free.log) |
| good | 0 | `wrap` | `scoped-unreachable` | `scoped-unreachable` | 是 | 25.29 | [日志](stage6a/63_good_up0_wrap.log) |
| good | 1 | `ub-free` | `scoped-unreachable` | `scoped-unreachable` | 是 | 75.75 | [日志](stage6a/63_good_up1_ub-free.log) |
| good | 1 | `wrap` | `scoped-unreachable` | `scoped-unreachable` | 是 | 41.84 | [日志](stage6a/63_good_up1_wrap.log) |
| good | 3 | `ub-free` | `scoped-unreachable` | `scoped-unreachable` | 是 | 73.04 | [日志](stage6a/63_good_up3_ub-free.log) |
| good | 3 | `wrap` | `scoped-unreachable` | `scoped-unreachable` | 是 | 42.60 | [日志](stage6a/63_good_up3_wrap.log) |
| 01_pointer_output | 0 | `ub-free` | `boundary-triggerable` | `boundary-triggerable` | 是 | 26.81 | [日志](stage6a/01_pointer_output_ub-free.log) |
| 01_pointer_output | 0 | `wrap` | `boundary-triggerable` | `boundary-triggerable` | 是 | 28.29 | [日志](stage6a/01_pointer_output_wrap.log) |
| 07_escaped_global | 0 | `ub-free` | `unknown` | `unknown` | 是 | 24.80 | [日志](stage6a/07_escaped_global_ub-free.log) |
| 07_escaped_global | 0 | `wrap` | `unknown` | `unknown` | 是 | 27.11 | [日志](stage6a/07_escaped_global_wrap.log) |

## fscanf 阶段 6a 保护回归

| bad/good | up | 第四轮 | ub-free | wrap | 日志 |
|---|---:|---|---|---|---|
| bad | 0 | `boundary-triggerable` | `boundary-triggerable` | `boundary-triggerable` | [默认](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up0__ub-free.log) / [wrap](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up0__wrap.log) |
| bad | 1 | `unknown` | `unknown` | `unknown` | [默认](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up1__ub-free.log) / [wrap](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up1__wrap.log) |
| bad | 3 | `unknown` | `unknown` | `unknown` | [默认](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up3__ub-free.log) / [wrap](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__bad__up3__wrap.log) |
| good | 0 | `scoped-unreachable` | `scoped-unreachable` | `scoped-unreachable` | [默认](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up0__ub-free.log) / [wrap](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up0__wrap.log) |
| good | 1 | `unknown` | `unknown` | `unknown` | [默认](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up1__ub-free.log) / [wrap](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up1__wrap.log) |
| good | 3 | `unknown` | `unknown` | `unknown` | [默认](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up3__ub-free.log) / [wrap](round4/logs/CWE190_Integer_Overflow__int_fscanf_add_51__good__up3__wrap.log) |

## 固定用例回归汇总

| 来源 | 模式 | boundary-triggerable | scoped-unreachable | unknown | 其他 |
|---|---|---:|---:|---:|---:|
| 第3轮 | `ub-free` | 62 | 38 | 7 | 6 |
| 第3轮 | `wrap` | 62 | 38 | 7 | 6 |
| 第4轮 | `ub-free` | 65 | 41 | 7 | 0 |
| 第4轮 | `wrap` | 65 | 41 | 7 | 0 |

- 既有 TP/TN 退化：0 条。
- `scoped-unreachable` 随 up 增大变 `unknown`：4 条，其中无新增丢弃依赖：0 条。
- rand 有界/无界 UNSAT 复核：170/170 通过。
- 确定结论的内存证据记录：2180 条。

### 深度单调性变化

- 第3轮 `CWE190_Integer_Overflow__int_fscanf_add_51` good `ub-free` up=0→1：有新增丢弃依赖。
- 第3轮 `CWE190_Integer_Overflow__int_fscanf_add_51` good `wrap` up=0→1：有新增丢弃依赖。
- 第4轮 `CWE190_Integer_Overflow__int_fscanf_add_51` good `ub-free` up=0→1：有新增丢弃依赖。
- 第4轮 `CWE190_Integer_Overflow__int_fscanf_add_51` good `wrap` up=0→1：有新增丢弃依赖。

## 判定

- 6a 新用例未通过：0；fscanf 原 unknown 变确定：0。
