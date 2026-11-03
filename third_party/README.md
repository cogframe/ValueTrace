# LLVM 开发依赖

本项目选用 LLVM 23.1.4 Windows x64 MSVC 完整开发包。

## 下载

文件：`clang+llvm-23.1.4-x86_64-pc-windows-msvc.tar.xz`，约 860 MB。

- [直接下载](https://github.com/llvm/llvm-project/releases/download/llvmorg-23.1.4/clang%2Bllvm-23.1.4-x86_64-pc-windows-msvc.tar.xz)
- [官方发布页](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.4)

官方下载说明将 `clang+llvm-` 压缩包用于开发依赖 LLVM 库的软件；本项目需要其中的开发头文件、库和 CMake 配置。

压缩包保存目录：`E:\ValueTrace\third_party\downloads`。

官方列出的 SHA256：

```text
854bffcfc84528e0ee92490ff5c178920abd358a79c3e98f053e41e3e8e4c6d2
```

## 解压

将包内整个安装根目录的内容放入 `E:\ValueTrace\third_party\llvm-23.1.4`。使用 7-Zip 时，`.tar.xz` 可能需要先解出 `.tar`，再解开 `.tar`；如果产生一层长名称目录，将该目录的全部内容放到上述目录，避免多嵌套一层。

最终至少应有：

```text
E:\ValueTrace\third_party\llvm-23.1.4\bin\clang-cl.exe
E:\ValueTrace\third_party\llvm-23.1.4\include\llvm\IR\Module.h
E:\ValueTrace\third_party\llvm-23.1.4\lib\cmake\llvm\LLVMConfig.cmake
```

后续 CMake 的 `LLVM_DIR` 指向：

```text
E:/ValueTrace/third_party/llvm-23.1.4/lib/cmake/llvm
```

## 接入方式

该包使用 MSVC ABI。后续用本机 Visual Studio x64 开发环境和 MSVC 或包内的 `clang-cl` 编译 ValueTrace，并使用新的 `build-msvc` 目录；原 `build` 使用 LLVM-MinGW，不能直接复用其中的配置来链接此包。

当前已通过 `find_package(LLVM CONFIG REQUIRED)` 接入 Core、IRReader、Support 和 Demangle 组件。运行 `scripts/build-msvc.cmd`，脚本使用包内的 `clang-cl`、Visual Studio C++ 工具和 Windows SDK，在 `build-msvc` 中构建和测试。

该官方包还引用 Zlib 和 Zstandard 静态库。构建配置自动从 [Zlib 官方源码](https://github.com/madler/zlib/releases/tag/v1.3.1) 和 [Zstandard 官方源码](https://github.com/facebook/zstd/releases/tag/v1.5.7) 下载固定版本并校验 SHA256，使用同一编译器构建；不使用本机 Strawberry/MinGW 目录中的库。下载、源码和产物均在 `build-msvc/_deps` 中，无需另外安装。

本机已检测到 Visual Studio C++ 工具及 Windows SDK，无需为此重复下载完整 IDE。

## 原生 Z3

路径求解使用 [Z3 官方 Windows x64 开发包 5.3.3](https://github.com/Z3Prover/z3/releases/tag/z3-5.3.3)。CMake 自动下载并校验 SHA256，解压到 `build-msvc/_deps/vt_z3-src`，链接其中的 C++ 头文件与导入库，并复制 DLL 到程序目录。无需另装 Python Z3 包。
