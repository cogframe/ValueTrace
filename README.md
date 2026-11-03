# ValueTrace

ValueTrace 用 C++ 实现面向选定参数的程序分析。最终目标是：选择一个或一组参数，指定向上和向下跨越的函数调用层数，检查范围内的数据传播与安全条件。详细目标见 [IMPLEMENTATION_GOALS.md](IMPLEMENTATION_GOALS.md)，实施顺序见 [ROADMAP.md](ROADMAP.md)。

## 当前 C++ 版本支持

**LLVM 版本门槛：**当前仅支持 LLVM **23.x**（最低为 23.0，仓库使用 23.1.4）；CMake 对 22.x 及更低版本、以及尚未适配的 24.x 均在配置阶段报错。因此低版本不会进入本工具的 IR 解析和分析流程，新标志不会被本工具静默忽略。若独立拿旧版 LLVM 工具读取含新标志的 `.ll/.bc`，解析结果由那个 LLVM 版本决定，不能当作 ValueTrace 的受支持输入；本项目没有“忽略不认识的标志后继续分析”的降级模式。LLVM 23 自身无法解析的 IR 会作为输入错误报告，不会悄悄删掉标志。

- 使用 LLVM 23 官方接口读取、校验 `.ll` 和 `.bc` 文件，自动建立整数数据依赖图。
- 支持多个 `.ll/.bc` 文件或目录输入，用 LLVM 官方链接接口连接跨文件声明与定义，再执行参数定位、追踪、目标值求解及自动风险验证。
- 提取整数参数、常量、运算、类型转换、返回值和直接函数调用，并保留操作数顺序。
- 优先通过函数名和源码参数名定位起点，重复 `--param-name` 可选择同一函数的参数组；同时保留 LLVM 参数编号入口。
- 列出重载函数及其签名、文件和定义行，由用户选择；参数列表展示名称、类型、隐式标记、可追踪状态及映射依据。
- 定位失败时报告调试信息缺失、位置记录缺失、变换表达式、分片、歧义或不支持的类型；有实际内联记录时报告内联依据，不把未知原因直接说成优化或内联。
- 连接调用实参与形参、返回值与调用结果；不同调用位置具有独立的节点上下文。
- 分别设置上下追踪深度。函数内部依赖不消耗深度，每次进入或返回跨函数边界均消耗一层。
- 返回追踪到的节点及其最小调用深度，并记录达到限制的边界。
- 保留位宽、指令、比较方式、`nsw/nuw/exact/nneg/samesign/disjoint` 标志、完整原始 IR 和可获得的源码位置。
- 在选定传播范围内输出潜在算术风险告警，统一分类并保留具体原因。
- 算术报告支持 5 档累加展示及按状态过滤，始终保留各状态总数、显示数和隐藏数；默认第 2 档。
- 使用原生 Z3 查询单个函数入口整数参数的目标值，结合向上调用范围内的分支、`switch`、`phi`、`select` 和调用返回关系。
- 求解结果区分 `SAT`、`UNSAT`、`UNKNOWN`，可行结果包含边界入参、调用/分支路径及约束。
- 使用 `verify` 自动验证选定参数组上下游范围内的整数风险，生成溢出、回绕、下溢、除零及有符号除法溢出条件；报告路径验证状态、输入、操作数及结果位模式。
- 自动检查整数缩窄的信息丢失、左移/右移的非法位移量，以及合法位移量下的左移溢出和回绕。
- 识别调用上下文、节点预算和导入展开边界；对未知内存来源、间接调用及外部定义给出说明。

内存传播的固定预期见 [内存传播验收契约](examples/memory_acceptance/ACCEPTANCE.md)。6a 已支持选中指针参数、单目标读写和跨直接调用的强更新；`memory_acceptance` 运行 6a 的单指针与全局地址逃逸用例，后续 6b/6c 用例在 `memory_acceptance_later` 中禁用。6a 的 Juliet 与回归结果见 [第六阶段 6a 评估](eval/juliet_run6a/summary.md)。地址经未建模调用或存入其他内存后，后续读取取完整位宽自由值并标记“地址已逃逸（待 6b）”；依赖该值的 SAT 保持 `unknown`。每条确定风险报告列出其读取的对象、偏移、大小、最后写入位置及更新方式。

`trace` 输出值依赖和未验证候选；`solve` 查询一个函数入口参数的手写目标条件；`verify` 自动验证一个或一组参数传播范围内的算术候选。三者均不输出整个项目的安全结论。运行预处理脚本后，普通标量局部变量可被提升为值依赖；仍保留的内存读写、浮点、向量及指针传播未建模，可能显示为内存边界或求解未确定。

## 算术风险告警

对外统一使用 `Warning: ArithmeticRisk`，内部保留 `reason`：`SignedOverflow`、`UnsignedWrap`、`UnsignedUnderflow`、`DivideByZero`、`SignedDivisionOverflow`、`SignedTruncationLoss`、`UnsignedTruncationLoss` 和 `InvalidShiftAmount`。在选定传播范围内检查整数加减乘、除法、取余、缩窄和位移；常量按实际位宽检查，明显无风险的加零、乘一、合法常量位移及不丢失信息的常量缩窄不产生对应候选。结果按现有纯文本逐行格式输出，例如：

```text
Warning: ArithmeticRisk reason=UnsignedUnderflow feasibility=unverified status=unverified assumption=unsigned node=#42 function=B context=... instruction=%x = sub i32 %a, %b
```

`source-type=signed/unsigned` 表示已从直接对应的源码调试类型取得依据；`attribute-type` 表示依据来自参数扩展属性；`assumption=signed/unsigned` 则仍只是该数值解释下的候选风险。它们适用于加减乘、左移及缩窄；不会为 `i32/i64` 赋予全局符号性。`feasibility=unverified` 表示尚未结合分支条件证明此输入可走到该指令。违反 `nsw/nuw` 的情况仍是对应标志的语义风险，不能把产生的 poison 当成普通回绕。下游是否接受运算结果，需要有明确的下游约束才能判断；当前不会仅凭运算结果大小编造这一类告警。

保留这套自查告警行为。`verify` 给警告补充验证依据，不把风险变成执行错误；`solve` 仍只回答指定目标值，不修改算术警告的验证状态。

边界自由输入按完整位宽建模，不能当成上游实际会产生的范围。`verify` 对完整建模或仅含过近似依赖的 UNSAT 标为 `scoped-unreachable`；含丢弃依赖时标为 `unknown`；SAT 只有在依赖已完整建模，或未建模依赖仅来自已声明的外部输入源时，才标为 `boundary-triggerable`。声明外部输入源的反例会注明“来源范围未验证”。当前没有来源闭合证明，因此不会产生 `source-confirmed`，即使最高函数没有调用者。同一位置的各原因分别验证、分别展示和计数，不混合调用上下文。

## 自动风险验证

```powershell
# 不需要 --op/--value；检查 RiskB.count 上下各一层传播范围内的算术候选
.\build-msvc\valuetrace.exe verify build-msvc/fixtures/risk_unsafe.ll --function RiskB --param-name count --up 1 --down 1 --report-level 5
# 无检查：UnsignedWrap 为 boundary-triggerable，包含输入、路径和关键值

.\build-msvc\valuetrace.exe verify build-msvc/fixtures/risk_safe.bc --function RiskB --param-name count --up 1 --down 1 --report-level 5
# RiskA 限制 count <= 1000：相同 count + 8 的风险为 scoped-unreachable
```

- 起点沿用名称、编号和候选选择，支持同一函数的参数组。候选只取上下游切片的并集；同一个上下文中的同一条原因只检查一次。
- 分支、switch、phi/select 和普通直接调用返回沿实际路径求解。`i32/i64` 只提供位宽；加减乘仍保留有符号/无符号解释假设，不凭类型名猜测。
- 检查目标运算自身的溢出时，不预先假设它的 `nsw/nuw` 成立；到达目标之前的已建模执行条件继续有效。违反标志时，报告模运算位模式并标明真实 IR 结果为 poison，不能把位模式冒充有效结果。除零和有符号最小值除以/取余 `-1` 不提供有效结果。
- 检查在运算之前才能排除到达该运算的风险路径；事后检查不能消除已经发生的运算风险。
- SAT 只需一条完整建模的触发路径；UNSAT 要求范围内相关路径和依赖完整。未知内存、未知调用、循环回边及预算耗尽保留 `unknown` 和具体原因。数据切片外的控制辅助调用目前也保留未确定；需要超出向上边界的调用者后续状态时不猜测其入口状态。
- 默认 `--timeout-ms 5000` 是整批预算（包括导入耗时，导入本身不被中断）；`--max-states 2000`、`--max-steps 50000` 是每个上下文/运算/原因的执行预算。未完成条目保留为未知，不删除。所有候选先验证，再应用展示过滤，隐藏警告不会减少验证工作。

验收源文件为 `examples/risk_chain.cpp`，真实 C++ 三函数链包含有检查/无检查版本和 `.ll/.bc` 输入。测试将求得的无符号回绕输入传回编译后的 A → B → C，核对运算结果及 C 实际收到的值。

### 缩窄和位移检查

| 检查原因 | 条件和解释 |
|---|---|
| `SignedTruncationLoss` | 截取低位后，符号扩展回原位宽不能恢复原值。例如按有符号解释，32 位的 32768 缩为 16 位后变成 -32768。 |
| `UnsignedTruncationLoss` | 截取低位后，零扩展回原位宽不能恢复原值。例如 65536 缩为 16 位后变成 0。 |
| `InvalidShiftAmount` | `shl/lshr/ashr` 的位移量按无符号位模式比较，大于等于操作数位宽；这也覆盖负数位移量。32 位允许 0..31，64 位允许 0..63。 |
| `SignedOverflow` / `UnsignedWrap`（左移） | 位移量合法，但左移的数学结果不能由结果位宽保留；两种解释分别检查，不把非法位移量重复算作数值溢出。 |

缩窄的两种解释分别保留，不能简单认为“高位是 1 就一定丢失有符号值”：例如 -1 缩窄后再符号扩展仍是 -1。信息丢失可能是程序有意行为，警告不自动等同于漏洞。检查针对实际 LLVM `trunc`；没有缩窄的同位宽符号转换或被优化成其他指令的源码转换不据此生成缩窄告警。

缩窄报告包含原操作数及位宽、结果位模式及目标位宽。`trunc` 和 `shl` 自身的 `nsw/nuw` 不提前作为假设；违规时标明 poison。非法位移量同样产生 poison，报告不提供虚构的结果位模式。此前操作产生的 poison 仍按原语义传播。

位移检查按 IR 的实际位宽进行，已经反映编译器做过的整数提升；不据此恢复不同 C/C++ 标准版本的全部源码位移规则。当前右移 `exact` 标志继续参与执行有效性建模，其违规尚无独立风险条目。

```powershell
# 类型缩窄：上下游均为 0，观察边界输入下的信息丢失
.\build-msvc\valuetrace.exe verify build-msvc/fixtures/integer_checks.ll --function Narrow64 --param-name value --up 0 --down 0 --report-level 5

# 位移量：纳入一层上游 GuardShift32 的 amount < 32 检查
.\build-msvc\valuetrace.exe verify build-msvc/fixtures/integer_checks.bc --function Shift32 --param-name amount --up 1 --down 0 --report-level 5
```

`examples/integer_checks.cpp` 用于生成带调试信息的 `.ll/.bc` 并回放缩窄及合法位移量的无符号回绕反例；非法位移量的反例只核对条件，不在原生 C++ 中执行未定义操作。

## 未建模依赖清单

`verify` 为每条风险结论输出未建模依赖的数量、具体来源及分类。**过近似**是把未建模值替换为独立的任意位向量，相关分支仍约束这些值。**丢弃**表示有未探索的路径或无法完整分析的指令，包括循环回边、再次进入同一基本块、步骤或状态预算耗尽、不支持的指令；这些情况阻止 `scoped-unreachable`。报告分别列出 `Over-approximation dependency`、`Discarded dependency` 和 `Unexplored path`。只有过近似依赖时，UNSAT 意味着对这些值的任意取值都无法触发当前风险，因此可以报告 `scoped-unreachable`，并写明“对 X 的任意值成立”。

依赖清单只跟随当前风险的操作数与实际路径条件传播：无关的读取或写入不会仅因位于同一函数中而污染结论。超出所选 `--down` 范围的调用返回值在被使用时记入清单；调用可能写入的内存，在后续读取可能受其影响时记入清单。SAT 若依赖其他未建模值，仍为 `unknown`，不输出反例；若仅依赖声明的外部输入源，则为 `boundary-triggerable`，并分别说明单次返回值范围与来源序列可达性是否已验证。默认仅声明 `rand`；可重复使用 `verify --external-input-source NAME` 追加函数名。若上游切片在无参来源函数或常量调用处闭合，则停止扩展无关的调用者，并在报告中写明闭合深度。

第四轮的 `verify` 还会列出每条已判定风险的自由值、上游无 UB/无 poison 前提，以及调试类型是否只用于筛选风险类别。调试类型不会收窄求解器中的位向量取值域。`rand` 在 Windows MSVC 目标三元组下按 `[0,32767]`、Linux GNU 下按 `[0,2147483647]` 约束；其他目标三元组不自动假定范围。本机 UCRT `stdlib.h` 将 `RAND_MAX` 定义为 `0x7fff`，与 Windows MSVC 配置一致。`--rand-max N` 可显式覆盖，`--rand-max none` 用于关闭范围做回归对比。反例列出本路径中每次 `rand` 调用的取值。范围约束只验证单次返回值在指定区间内，不证明一组返回值可由同一个随机种子产生。Juliet 的 `RAND32()` 在本项目的 Windows MSVC、`-O0` IR 中使用无 `nsw` 的 `shl`，故这些位移溢出按无符号位向量回绕，不因 `nsw` 产生 poison；每次 `rand` 的范围约束会限制移位操作数，但不能直接等同于 `RAND32()` 的结果范围。

`llvm.abs`（包含 `INT_MIN` 的 poison 标志）、`llvm.smin/smax/umin/umax` 已按整数语义建模；浮点转换和 `ctpop/bswap` 尚未纳入。`verify --return-of NAME` 可用整数函数的返回值作为切片起点，不能同时指定参数。边界可触发且上游尚未闭合时，报告只列出下一层调用点及其实参摘要，不把这些调用点加入当前求解。

`verify --semantics ub-free|wrap` 控制 `nsw/nuw` 对上游路径的影响。默认 `ub-free` 保留 LLVM 的 poison 约束；`wrap` 是诊断用的假设语义，把带 `nsw/nuw` 的算术或缩窄溢出按固定位宽回绕，不把该上游路径排除。除 `nsw/nuw` 外的 poison、立即 UB 和未建模依赖规则继续生效。`scoped-unreachable` 必须经 `wrap` 模式确认为 UNSAT：默认模式先按 `ub-free` 求解，若 UNSAT 再按 `wrap` 复核；仅 `ub-free` 无解而 `wrap` 有解时，降为 `unknown`，报告“仅在无 UB 假设下不可触发”。报告的 `Semantics` 与 `Used assumptions` 行标明结论所用模式。

## 五档展示与过滤

这是展示范围，不是错误严重程度，也不改变分析、验证状态或程序退出码。档位逐档累加：

| 档位 | 增加展示的状态 | 状态名称 |
|---|---|---|
| 1 | 已核实可触发 | `source-confirmed` |
| 2（默认） | 边界条件下可触发 | `boundary-triggerable` |
| 3 | 未确定 | `unknown` |
| 4 | 尚未验证的候选 | `unverified` |
| 5 | 范围内不可触发 | `scoped-unreachable` |

`trace` 候选仍是未验证，默认第 2 档通常只显示统计；`verify` 默认展示边界可触发警告，未确定从第 3 档开始展示，范围内不可触发从第 5 档开始展示。无明细不代表已经安全。查看原有自查警告可使用第 4/5 档或选择 `unverified`：

```powershell
# 查看现有未验证警告，保留各条 reason 和解释假设
.\build-msvc\valuetrace.exe trace cpp/tests/fixtures/semantics.ll --function risk_cases --param 0 --up 0 --down 0 --report-level 4

# 也可以单独选择状态，名称区分大小写；可重复的状态会去重
.\build-msvc\valuetrace.exe trace cpp/tests/fixtures/semantics.ll --function risk_cases --param 0 --show-status "unverified,unknown"

# all 显示全部状态；none 隐藏全部风险明细，但仍显示统计和边界说明
.\build-msvc\valuetrace.exe trace cpp/tests/fixtures/semantics.ll --function risk_cases --param 0 --show-status none
```

`--report-level` 与 `--show-status` 不能同时指定。空列表、未知状态、越界档位或重复提供同一个选项都会报配置错误，避免静默忽略。每次报告都展示五类状态的总数、显示数、隐藏数；同一个图节点同时出现在上下追踪结果时只统计一次，每种风险原因分别计数。

过滤针对 `trace/verify` 的算术风险明细，不隐藏传播图、模型缺失、调用边界或真正的执行错误。`solve` 的 `SAT/UNSAT/UNKNOWN` 是独立的目标值求解结果；向 `solve` 传入风险过滤选项会明确报错。展示档位不改变边界分析策略。

## 目标值与路径求解

```powershell
# 两份 IR 来自同一个 A → B → C 样例，只相差 A 中的参数检查
.\build-msvc\valuetrace.exe solve build-msvc/fixtures/path_guarded.ll --function C --param-name length --op ugt --value 4096 --up 2
# UNSAT：A 限制 input <= 1024，C.length = input * 4，无法大于 4096

.\build-msvc\valuetrace.exe solve build-msvc/fixtures/path_unguarded.ll --function C --param-name length --op ugt --value 4096 --up 2
# SAT：输出一组边界输入、目标值、调用路径和分支约束
```

验收源文件是 `examples/path_chain.cpp`。构建生成带检查的 `.ll/.bc` 和无检查的 `.ll`；CTest 还把求出的输入送入实际编译的样例程序，核对 C 真正收到的值。C 中的记录语句用于保持可观察调用，避免编译器消除原样返回入参的空操作调用；求解目标是 C 的入口参数，不涉及分析记录语句的内存行为。

- `--op` 支持 `eq/ne/ugt/uge/ult/ule/sgt/sge/slt/sle`。`u` 和 `s` 明确选择无符号或有符号比较，不能仅凭 `i32/i64` 推断。
- `--value` 接受十进制或 `0x` 十六进制；负数用于有符号比较或相等比较。超出目标位宽的查询值会被拒绝，不静默截断。
- `--up` 为 0..64，从目标函数沿实际直接调用者向上确定范围。到达边界的函数入参是自由符号输入；范围之外的调用者条件没有纳入，SAT 是边界条件下的候选，不是实际崩溃历史或完整外部输入。
- `solve` 当前是函数入口目标查询，要求 `--down 0`（默认值）。双向依赖图继续使用 `trace` 的独立 `--up/--down`；本轮不查询函数内部任意时刻的变量值。
- `SAT` 表示找到支持模型中的定义明确路径；`UNSAT` 表示该条件在所选调用范围和模型下不可达，不等于程序安全。仅含过近似任意值的 UNSAT 可以给出当前范围的不可触发结论；未探索路径、步骤预算耗尽、循环回边或不支持的指令会阻止该结论并给出 `UNKNOWN`。SAT 若依赖未声明的外部来源或其他未建模值也为 `UNKNOWN`。某条已完整建模的可行路径仍可提供 SAT，即使其他路径尚未完成。
- 默认总求解时间 5000 ms、路径状态预算 2000、指令步骤预算 50000，可用 `--timeout-ms`、`--max-states`、`--max-steps` 调整。调用者上下文预算沿用 4000。求解层按这些预算运行，不使用图导入器的节点预算。

已支持标量整数的加减乘、位运算、除法/取余、位移、比较及整数宽度转换；按路径处理分支、`switch`、`phi`、`select`、普通直接调用返回和基本 `llvm.assume`。常见 min/max 整数内建函数也可建模。尚未做循环展开；需要再次进入同一基本块时保留未确定状态，不把第一次展开的结果当成完整证明。内存、间接调用、外部实现及未支持的属性或内建函数会保留具体边界。

求解器区分产生 poison 的运算与执行即触发未定义行为的除法等操作：未被采用的 `select` 分支不会错误约束目标值，除零则不能因结果未使用而被当成定义明确执行。语义依据见 [LLVM LangRef](https://llvm.org/docs/LangRef.html#poison-values)。现有算术风险自查警告仍独立存在。

## 整数类型依据

风险报告分别显示 `source`（直接对应的源码类型）、`resolved`（当前采用的整数解释）和 `basis`（依据）。优先级为：`llvm.dbg.value` / `#dbg_value` 对应的源码类型及 DWARF 编码 > 参数 `signext/zeroext` 属性 > 指令自身的有符号/无符号语义 > 未知。`i32/i64` 本身始终没有全局 signedness。不同来源矛盾时输出 `Type conflict`，保留相关候选风险，不静默选择一种解释。无 `nsw` 的 IR 运算虽产生模位模式，也不能据此把已识别的源码有符号溢出当作安全执行。

源码类型沿 `typedef`、`const`、`volatile`、`restrict` 包装追到 `DIBasicType`；支持 `DW_ATE_signed`、`DW_ATE_unsigned`、`DW_ATE_signed_char`、`DW_ATE_unsigned_char` 和 `DW_ATE_boolean`。只接受直接指向单个 LLVM 整数值的空调试表达式。运算结果没有直接调试记录时，可在同位宽、所有非常量操作数都有一致类型记录的情况下推得解释；结果类型与操作数类型不一致时报告冲突并保留候选。枚举的底层类型、位域、片段和复杂调试表达式暂标为 `source=unknown`，并写明原因。没有调试信息时仍保留原来的双解释候选检查，报告标明源码类型未知；若指令自身规定有符号语义，只将它作为较低优先级的操作依据。已知源码类型只用于选择适用的风险解释，**不**使结果升级为 `source-confirmed`；该等级仍需来源闭合证明。

**整数提升与缩窄：**风险条件总按实际运算指令的位宽计算。`uint16_t`、`uint8_t`、`char`、`bool` 在源码表达式中可能先提升为 `int`；例如 `zext i16` 后的 `add i32` 不是 16 位回绕。对 `zext/sext` 操作数，工具用扩展前的位宽证明明显不可能的加法溢出；中间值没有可靠的调试类型对应时仍标 `source=unknown`，不会继承任一原变量的类型。赋给较窄类型时，信息丢失归于后续的 `trunc` 指令。

## 构建和运行

需要 CMake 3.24+、Ninja、LLVM 23 开发包和兼容的 C++20 工具链。本机 LLVM 已放在 `third_party/llvm-23.1.4`，Visual Studio C++ 工具和 SDK 已安装。PowerShell 下运行：

```powershell
.\scripts\build-msvc.cmd
.\build-msvc\valuetrace.exe trace build-msvc/fixtures/parameter_chain.ll --function B --param-name count --up 1 --down 1
.\build-msvc\valuetrace.exe trace build-msvc/fixtures/parameter_chain.bc --function B --param-name count --up 0 --down 0
```

构建脚本自动选择 Visual Studio x64 环境，使用 LLVM 包内的 `clang-cl` 和 MSVC ABI，并在 `build-msvc` 中构建与测试。首次配置会下载固定版本且校验 SHA256 的 Zlib/Zstandard 源码及 Z3 5.3.3 Windows 原生开发包，全部保存在构建目录内。Z3 DLL 自动复制到程序目录；不依赖 Python 或其虚拟环境。原来的 MinGW `build` 目录不用于 LLVM 接入版本。

验收样例是 `examples/parameter_chain.cpp` 中的 `A → B → C`。构建时会生成对应 `.ll`、`.bc` 和无调试信息的 `.ll`；选择 `B` 的第 0 个参数，向上找到 `A`，向下找到 `C`，将两侧深度改为 0 可观察停止边界。

`--function` 接受精确 LLVM 符号、完整反修饰签名或函数名。重载名称不唯一时，命令行列出候选并等待选择；函数唯一则直接使用。未指定参数时，列出该函数的参数并等待选择，支持逗号分隔的多选。输入结束或选择无效时明确失败，不默认选择第一项。

对自己的源文件，使用同一开发包的 Clang 生成 IR，例如：

```powershell
.\scripts\prepare-ir.ps1 -Source examples/parameter_chain.cpp -Output build-msvc/my.ll
.\build-msvc\valuetrace.exe trace build-msvc/my.ll --function B --param-name count --param-name extra --up 1 --down 1
```

预处理脚本按文件扩展名选择语言：`.c` 使用 `-x c -std=c17`，`.cc/.cpp/.cxx/.C` 使用 `-x c++ -std=c++20`。随后以 `-O0 -g -Xclang -disable-O0-optnone -fno-inline` 生成 LLVM IR，再运行 `opt -passes=mem2reg,sroa`。普通标量局部变量的 `alloca/load/store` 因此转为可追踪的值关系，同时保留参数名所需的调试信息。输出名可用 `.ll` 或 `.bc`。本流程不使用 `-O1` 或更高优化，也不执行内联。

多文件可分别执行脚本生成 `.ll/.bc`，然后把这些文件一起交给 ValueTrace；也可显式链接为单份 IR：

```powershell
.\scripts\prepare-ir.ps1 -Source examples/multifile/a.cpp,examples/multifile/b.cpp,examples/multifile/c.cpp -Output build-msvc/linked.ll -Link
```

`-Link` 使用 `llvm-link`；不同文件需要相同的编译配置，必要时通过 `-Define` 传入宏定义。该预处理只提升可以安全提升的局部存储，不代替结构体字段或任意指针的内存传播分析。

优化后的 IR 可能把源码截断改写成掩码等等价操作；报告展示实际读取的 IR。

其他优化也可能直接消除调用，即使函数禁止内联。必须确认关注链在 IR 中保留；没有直接调用者时，求解器会说明该函数按自由入参边界处理，不猜测调用是否因优化消失。

## 跨文件分析

`inspect`、`trace`、`solve`、`verify` 共用输入入口，支持混合 `.ll/.bc` 文件，也可重复使用 `--input PATH`：

```powershell
# 明确列出不同编译单元
.\build-msvc\valuetrace.exe trace build-msvc/fixtures/multifile/safe/a.ll build-msvc/fixtures/multifile/safe/b.bc build-msvc/fixtures/multifile/safe/c.ll --function CrossB --param-name value --up 1 --down 1

# 专用目录中的全部中间代码：原有风险检查和五档过滤直接复用
.\build-msvc\valuetrace.exe verify build-msvc/fixtures/multifile/safe --function CrossB --param-name value --up 1 --down 1 --report-level 5

# 也可显式添加输入
.\build-msvc\valuetrace.exe solve --input build-msvc/fixtures/multifile/unsafe --function CrossC --param-name length --op ugt --value 4096 --up 2

# 不同文件中的同名 static 函数，列出候选后由用户选择
.\build-msvc\valuetrace.exe inspect build-msvc/fixtures/multifile/safe --function localStep
.\build-msvc\valuetrace.exe trace build-msvc/fixtures/multifile/safe --function localStep --function-choice 2 --param-name value --up 1 --down 0
```

目录只读取**当前这一层**的 `.ll/.bc` 普通文件，不递归进入子目录；显式输入与目录可以组合。文件规范化后按路径排序，并列出实际输入清单；同一路径或目录重叠导致文件重复时直接报错，不悄悄去重。一个编译单元请提供一种产物，不同时放入其 `.ll` 和 `.bc`。空目录、无效路径、解析失败都报输入错误。

多个输入应来自**同一目标程序和同一构建配置**。加载器检查目标平台与数据布局、同名外部符号类型、调用约定及主要 ABI 属性，并由 LLVM 校验模块标志和链接结果；不一致时报告冲突符号及涉及文件。它不能证明宏、头文件版本和所有编译选项一致，使用者仍需提供配套产物。双方均缺失目标信息时不能据此认定平台已核实。

外部符号匹配使用 LLVM 符号及类型，不通过相似的源码函数名猜测连接。**第一版对重复的外部定义一律报错，包括 weak/linkonce/COMDAT 的重复函数定义**，不会自动选择某份模板或内联实现；LLVM 的 appending 列表（例如 `llvm.used`）按官方规则合并。此策略优先保证冲突明确可见，尚不覆盖任意真实 C++ 构建的 ODR 合并策略。

不同文件中的本地函数保持独立；多文件加载时分配包含编译单元序号的内部符号，候选继续展示原函数名、源码位置及输入文件。不被其他函数调用、但仍存在于输入 IR 的本地函数也保留为可选起点。候选序号和内部符号只对当前输入集合有效；增加文件后应重新查看候选。

跨文件不会额外消耗追踪深度，仍按实际函数调用边界计数。缺少实现时保留原来的外部定义边界：如果风险依赖其未知返回值或执行效果，报告未确定；在调用之前已能完整验证的风险不受无关缺失定义影响。链接成功不等于输入项目完整，也不提升 `source-confirmed` 等级。

验收样例位于 `examples/multifile`：`a.cpp → b.cpp → c.cpp`，含有检查和无检查版本，混合 `.ll/.bc`，并与等价单文件样例对照。测试回放真实 C++ 调用链的求解输入，也检查同名 static 隔离、未引用本地函数保留、冲突诊断及深度边界。

本版分析已有中间代码；尚不自动读取项目编译配置或编译整个项目。原有 `-g` 名称映射及内联边界继续适用。

## 按名字和候选列表选择

```powershell
# 只查看候选及参数，不开始追踪，也不会等待输入
.\build-msvc\valuetrace.exe inspect build-msvc/fixtures/named_parameters.ll --function overloaded
.\build-msvc\valuetrace.exe inspect build-msvc/fixtures/named_parameters.ll --function Worker::process

# 交互选择重载，然后选择参数
.\build-msvc\valuetrace.exe trace build-msvc/fixtures/named_parameters.ll --function overloaded

# 名称唯一时直接追踪，无需了解 this 的位置
.\build-msvc\valuetrace.exe trace build-msvc/fixtures/named_parameters.ll --function Worker::process --param-name count

# 自动化运行可明确指定当前候选列表中的第几项
.\build-msvc\valuetrace.exe trace build-msvc/fixtures/named_parameters.ll --function overloaded --function-choice 2 --param-choice 1
```

`--function-choice` 和 `--param-choice` 使用列表中从 **1** 开始的编号；候选编号只对应当前输入文件和当前函数查询。原来的 `--param` 使用从 **0** 开始的 LLVM 实际参数编号，是低层备用入口；隐式参数也占该编号。长期保存的查询优先使用准确函数符号或完整签名加参数名。

名称区分大小写，只匹配函数定义作用域内的形参，不匹配同名局部变量，也不把 LLVM SSA 名字当成源码名字。源码类型用于展示和支持范围校验，不用于给 LLVM 值指定全局 signedness。

参数名选择要求保留 `-g` 信息，并且形参调试位置记录直接、无表达式地指向一个 LLVM 入参。工具不使用“源码序号减一”映射；当前对象 `this` 和隐藏返回存储参数由候选列表标明，指针参数显示为不可追踪。结构体、分片或复杂表达式映射会说明具体不支持的原因。无源码名称时仍可在明确标注的 LLVM 参数行中选择，或使用 `--param`，但不能通过 `--param-name` 猜测名称。

失败报告区分缺失调试信息、找不到指定名称、缺少位置记录、计算值/常量位置、表达式变换、内存地址、分片、多重映射和类型不支持。发现真实 `inlinedAt` 记录时明确报告内联证据；只有优化标记或映射缺失时，不据此断言具体消失原因。成功时输出“源码参数名 → LLVM 入参”的实际对应关系。

## 内联相关的使用边界

当前支持范围是分析用 IR 中保留了相关函数及其调用边界的参数传播。工具不还原函数内联前的源码调用链，也不重建已被消除的形参。

- N 层深度按输入 LLVM IR 实际保留的函数边界计算，不代表内联前源码中的 N 层调用。
- 如果所关注参数及其传播链涉及已发生的函数内联，不保证源码级追溯结果的正确性、完整性和可靠性；不能据此判断源码范围内已经排查完整或安全。
- 建议为排查单独生成保留调用边界的 IR，并检查关注函数和调用是否仍存在。示例使用 `-fno-inline`；编译选项不能代替对实际 IR 的确认。
- 当前不会自动证明输入 IR 未发生内联，也不会自动识别所有内联情况。函数查找失败只表示输入中无法定位该函数，不能直接判断其是否被内联。

这个边界针对涉及内联的分析链，不要求整个项目的所有函数均未发生内联。

## 代码结构

```text
cpp/include/valuetrace/DependencyGraph.h  图节点与边
cpp/include/valuetrace/Slice.h            参数组、双向深度和结果
cpp/src/DependencyGraph.cpp              图的存储与校验
cpp/src/Slice.cpp                        有限深度追踪
cpp/include/valuetrace/ReportFilter.h     五类风险状态、档位过滤及计数接口
cpp/src/ReportFilter.cpp                 累加档位和精确状态过滤
cpp/src/ArithmeticReport.cpp             不改变分析结果的算术报告展示
cpp/include/valuetrace/LlvmReader.h       LLVM 导入查询与结果接口
cpp/src/LlvmReader.cpp                   官方 IR 读取、整数元数据及调用上下文建图
cpp/include/valuetrace/ParameterSelection.h 函数及参数候选接口
cpp/src/ParameterSelection.cpp           调试位置映射、候选发现和失败原因
cpp/include/valuetrace/ProgramInput.h     多文件及目录输入解析
cpp/src/ProgramInput.cpp                 文件展开、排序与重复输入检查
cpp/src/ModuleLoader.cpp                 配置/符号预检、LLVM 链接及本地函数保留
cpp/include/valuetrace/PathSolver.h       目标参数查询、可行输入与路径结果
cpp/src/PathSolver.cpp                   分支路径及有限调用范围执行
cpp/src/IntegerExpressions.cpp           LLVM 整数与定义条件到 Z3 的转换
cpp/include/valuetrace/RiskVerifier.h     参数范围内自动风险验证接口
cpp/src/RiskVerifier.cpp                 候选、调用身份和验证状态关联
cpp/src/RiskConditions.cpp               不假定目标自身有效的违规条件
cpp/app/main.cpp                         命令行和演示
cpp/app/SelectionMenu.cpp                用户候选选择
cpp/tests/SliceTests.cpp                 核心规则测试
cpp/tests/LlvmReaderTests.cpp            真实 C++、整数语义、调用隔离和边界测试
cpp/tests/ParameterSelectionTests.cpp    名称、重载、this/sret 和失效原因测试
cpp/tests/PathSolverTests.cpp            路径合流、整数语义和未确定边界测试
cpp/tests/ReportFilterTests.cpp          五档、显隐统计和候选状态不变测试
cpp/tests/RiskVerifierTests.cpp          自动条件、标志、范围、未知依赖和真实反例回放
cpp/tests/IntegerCheckTests.cpp          缩窄、位移量、左移回绕、路径检查及 C++ 回放
cpp/tests/MultiFileTests.cpp             跨文件对照、static 隔离、配置冲突和 C++ 回放
```

导入器从选定函数的调用者上下文出发，展开有限调用树，并为每个调用上下文建立独立节点。上下深度分别最多为 64；默认导入预算为 4000 个上下文、100000 个节点，超出预算直接报错。递归按展开限制保留边界，不无限展开。

LLVM 的 `i32/i64` 及其他整数节点只存位宽，不存全局 signedness。操作语义按具体指令解释；例如同一个 `i64` 可分别用于有符号和无符号比较。源码调试信息用于位置、名称映射及源码类型展示，不据此猜测表达式语义或覆盖指令语义。

若只构建不依赖 LLVM 的图核心和演示，可使用 `-DVALUETRACE_WITH_LLVM=OFF`；该模式不提供 `trace`、`inspect`、`solve` 或 `verify` 命令。

之前的 Python 原型已归档到 [legacy/python](legacy/python/README.md)，供设计和简单算术结果参考。C++ 工程不依赖 Python。
