# ValueTrace

> 这是归档的 Python 原型。当前主实现和构建方式见 [项目 README](../../README.md)。以下内容描述归档版本，执行安装命令时应位于本目录。

项目实现目标与验收标准见 [IMPLEMENTATION_GOALS.md](../../IMPLEMENTATION_GOALS.md)，逐步实施顺序见 [ROADMAP.md](../../ROADMAP.md)。

ValueTrace 是一个面向 C/C++ 整数数据传播的最小分析原型。给它一份 LLVM IR、入口函数和目标函数的整数参数，它会沿直接函数调用追踪参数的计算过程，并用 Z3 寻找满足目标条件的入口参数。

## 第一版支持

- 一个 LLVM IR 文件中的整数参数、直接函数调用及返回值。
- 整数加、减、乘、位移，以及 `trunc`、`zext`、`sext` 类型转换。
- 从入口函数到目标调用的有限深度传播；`--max-depth` 表示最多跨越的函数调用次数。
- 对目标参数施加等于、大于、小于条件，并给出一组入口参数和传播路径。
- 遇到分支、内存读写、间接调用或其他未建模的指令时报告“不支持”，不输出安全结论。

当前版本分析的是**选定调用链上的数值关系**，不自动判定漏洞。目标条件由使用者提供。

## 安装

需要 Python 3.10+ 和 Clang。Windows PowerShell 下，在项目目录执行：

```powershell
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install -e .
```

## 运行示例

```powershell
clang++ -S -emit-llvm -O1 -fno-inline -fno-discard-value-names examples/chain.cpp -o examples/chain.ll
.\.venv\Scripts\valuetrace.exe analyze examples/chain.ll --entry source --target consume:0 --op ugt --value 4096 --max-depth 2
```

`consume:0` 指 `consume` 的第 0 个参数；`ugt` 指按无符号整数比较“大于”。示例会显示一组使目标参数大于 4096 的 `source` 入参。也可以使用 `eq`、`ult`、`uge`、`ule`。C++ 示例用 `extern "C"` 保留简单函数名；普通 C++ 函数需要传入 LLVM IR 里的符号名。

## 结果含义

`SAT`：在当前模型与指定调用深度内找到满足条件的一组输入。`UNSAT`：当前已建模的调用链没有满足条件的输入。`UNKNOWN`：超时或分析所需的指令尚未支持。三者都不是对整个项目的安全认证。
