# 内存传播验收契约

## 第五轮开工前固定预期（2026-11-03）

以下预期在 6a 实现前写定。风险等级均指目标 sink 的有符号溢出或无符号回绕检查；`up` 是向上跨函数调用层数。若日志显示模型未闭合，应按 `unknown` 记录，不用结果反向修改预期。6b、6c 项本轮不运行。

| 阶段 | 用例与起点 | 固定预期 |
|---|---|---|
| 6a | Juliet `CWE190_Integer_Overflow__int_rand_add_63`：bad/good 的 `dataPtr`，各 `up=0,1,3` | bad 三个深度均为 `boundary-triggerable`；good 三个深度均为 `scoped-unreachable`。bad 的读取来自该指针指向对象的入口值或已执行写入；good 的守卫对任意该值排除溢出。每条确定结论显示对象、偏移 0、大小 4、最后写入位置和更新类型。 |
| 6a | `01_pointer_output.cpp`：`A.value`，`down=2` | `B` 对同一对象 `len` 偏移 0 的写入进入 `C` 的读取与加法；目标风险为 `boundary-triggerable`，反例与最后写入相符，内存证据为强更新。 |
| 6b | `02_distinct_fields.cpp`：`FieldCheck.count`，`down=2` | 仅 `count` 被写，`len` 保持独立；不出现由 `count` 假造出的 `len` 风险边，`len` 读取证据指向其原始值。 |
| 6c | `03_conditional_alias.cpp`：`ConditionalRead.value`，`down=1` | 多目标别名保留新旧两种可能；目标风险为 `unknown`，证据说明别名选择和弱更新。 |
| 6c | `04_external_pointer.cpp`：`ExternalPointer.value`，`down=1` | 目标不明的指针写入保守污染可能位置，风险为 `unknown`，并给出指针身份观测建议。 |
| 6c | `05_dynamic_index.cpp`：`DynamicIndex.value`，`down=1` | 变量下标写入为整数组弱更新；之后读取保留新旧值，风险为 `unknown`。 |
| 6b | `06_memcpy_field.cpp`：`MemcpyField.value`，`down=1` | `memcpy` 将源对象的 `len` 四字节拷到目标 `len`，不覆盖 `count`；目标风险为 `boundary-triggerable`，列出源和目标位置。 |
| 6b | Juliet `CWE190_Integer_Overflow__int_fscanf_add_51`：bad/good，`up=0,1,3` | bad 各深度为 `boundary-triggerable`；good 各深度为 `scoped-unreachable`，特别是 `up=1` 的守卫对任意来源值都排除溢出；`fscanf` 的输出位置有 havoc/摘要证据。 |
| 6a | Juliet `CWE190_Integer_Overflow__int_fscanf_add_51`：bad/good，`up=0,1,3` 的阶段回归 | 地址传给未建模调用后，后续 load 取完整位宽自由值并记过近似依赖 `地址已逃逸（待 6b）`。相对第四轮不得增强：bad `up=0` 可降为 `unknown`，bad `up=1,3` 保持 `unknown`；good `up=0` 可保持 `scoped-unreachable` 或降为 `unknown`，good `up=1,3` 保持 `unknown`。尤其不得新增 TN，亦不得将原 `unknown` 变成确定结论。 |
| 6a | 对抗：地址先存入全局变量，另一函数取出后传给外部调用，再读取原对象 | 原对象地址按逃逸处理；读取产生完整位宽自由值，过近似依赖注明 `地址已逃逸（待 6b）`；由该自由值支持的 SAT 最高为 `unknown`。经 `bitcast`、`gep`、`phi`、`select` 转交时同理。 |
| 6b | 对抗：指针经两层函数传递后被修改，调用者读取 | 读取追到最深处的最后一次写入；若目标风险确定，则报告该对象、偏移、大小和强更新。 |
| 6b | 对抗：同一结构体两个字段，部分 `memcpy` 仅覆盖其中一个 | 被覆盖字段的读取指向拷贝，另一字段仍指向原写入；无字段串扰。 |
| 6c | 对抗：循环内变量下标写数组，随后按另一变量下标读 | 整数组弱更新并保留旧值；存在循环截断时报告 `unknown`，不得给 `scoped-unreachable`。 |
| 6b | 对抗：对象地址传入未知外部函数后读取 | 可能被写位置全部 havoc；读取来源为 havoc 自由值并记过近似依赖，不得给 `scoped-unreachable`（若守卫独立于该值的特殊情形，须单独审计）。 |
| 6c | Juliet `CWE190_Integer_Overflow__int_rand_add_62`（C++，引用输出参数） | bad 来源经引用写回后可触发；good 的安全来源不可触发；若来源 `rand` 的实际序列可达性未证明，保留边界来源说明。 |
| 6c | Juliet `CWE190_Integer_Overflow__int_rand_add_64`（`void*` 转 `int*`） | bad 的 sink 可触发，good 的守卫不可触发；`void*` 转换保持同一对象、偏移 0。 |
| 6c | Juliet `CWE190_Integer_Overflow__int_rand_add_66`（数组传递） | bad 目标可触发，good 守卫不可触发；常量下标定位对应元素，读写不串扰。 |
| 6c | Juliet `CWE190_Integer_Overflow__int_rand_add_67`（结构体传递） | bad 目标可触发，good 守卫不可触发；风险操作数来自结构体内相应字段。 |
| 6b | Juliet `CWE190_Integer_Overflow__int_rand_add_68`（跨文件全局变量） | bad 目标可触发，good 守卫不可触发；读取关联同一全局对象的最后写入。 |

对抗性用例的源码在对应阶段开始前落地，但以上结果预期已固定。若具体 Juliet 变体在所选深度以外才有安全来源，标 `OUT_OF_SCOPE` 并保留下一层实参摘要，不把它记为误报。

六个程序相互独立，均按定义明确的 C++ 语义执行。生成 LLVM IR 时使用与项目其他样例一致的 `-O0 -g -Xclang -disable-O0-optnone -fno-inline`，随后运行 `opt -passes=mem2reg,sroa`；第 6 例额外使用 `-fno-builtin-memcpy` 保留函数调用。测试以实际 IR 为准；若编译器消除了被检查的读写，应调整样例保持观察点，而不推断源码中已消失的调用。

内存位置键为 `(obj, offset, size)`：`obj` 是栈对象、全局对象或堆分配点，`offset` 为对象内字节偏移；只对大小已知的读写给出精确字段结论。数组常量下标拆成独立位置。变量下标统一记为该数组的抽象位置并弱更新。堆对象按分配点分组。嵌套字段展平偏移；重叠的 union 字段标未确定。

“疑似”映射到现有五档中的 **`unknown`（第 3 档）**，不新增第六档，也不借用 `boundary-triggerable`。后者只用于已完整建模、但上游来源尚未闭合的 SAT。多目标弱别名的分支可以给出条件性反例，但若结论依赖无法核实的别名选择，最终状态保持 `unknown`；同一结果不能报告 `source-confirmed` 或 `scoped-unreachable`。

| 用例 | 查询起点 | 必须检查的结果 |
|---|---|---|
| `01_pointer_output.cpp` | `A.value`, `up=0, down=2` | 写入 `ctx.len` 的最终定义来自 B 的指针出参写入；C 的读取经该定义连接。C 的 `+8` 有可复算输入和路径，状态为 `boundary-triggerable`。 |
| `02_distinct_fields.cpp` | `FieldCheck.count`, `up=0, down=2` | `ChangeCount` 只更新 `(ctx, count 偏移, 4)`；`UseLen` 读取 `(ctx, len 偏移, 4)` 仍是常量 100。此起点的下游不得包含 `UseLen` 的加法风险；若单独验证该位置，应为 `scoped-unreachable`。 |
| `03_conditional_alias.cpp` | `ConditionalRead.value`, `up=0, down=1` | 对两个候选对象分别弱更新；读取 `first.len` 时区分 `chooseFirst` 的两个分支，既保留旧值 100，也保留新值 `value`。相关可触发风险的最终状态为 `unknown`，报告包含别名选择条件和降级原因，不将两分支混成一个确定写入。 |
| `04_external_pointer.cpp` | `ExternalPointer.value`, `up=0, down=1` | 外部 `target` 无对象身份，写入及后续读取标 `unknown`。报告给出具体位置的观测点建议，说明运行时需要记录指向的对象身份；不修改源码或静默加入运行时日志。 |
| `05_dynamic_index.cpp` | `DynamicIndex.value`, `up=0, down=1` | 在 `index < 2` 路径内弱更新整个数组抽象位置；`ReadZero` 读取 `values[0]` 时同时保留旧值 100 与 `index == 0` 时的新值，涉及弱更新的结论为 `unknown`。无越界路径进入分析结果。 |
| `06_memcpy_field.cpp` | `MemcpyField.value`, `up=0, down=1` | `CopyLen` 的 `memcpy` 摘要精确复制 4 字节 `src.len` → `dst.len`；`dst.count` 保持 2。后续 `dst.len + 8` 的风险来自 `value`，状态为 `boundary-triggerable`。 |

每个用例还检查操作前后位宽、源码位置和函数边界。用例 1、2、6 的单目标写入应强更新；用例 3 的两个可能目标和用例 5 的数组抽象位置应弱更新，读取使用条件选择或分支展开。候选目标超过上限（默认建议 8）、递归、间接调用、超过 64 层及无法总结的外部写入都保留 `unknown`，不输出范围内不可触发结论。首批库摘要覆盖 `memcpy`、`memset`、`strlen`；各摘要要单独验证读写范围与返回值。普通正常内存语义为前提。

执行状态：6a 的单指针和全局地址逃逸用例已启用并通过；6b/6c 用例保持禁用，仍是待通过验收。预期表未根据运行结果修改。
