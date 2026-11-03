@echo off
setlocal
set "VSLANG=1033"
set "VT_ROOT=%~dp0.."
set "VT_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VT_VSWHERE%" (
  echo Visual Studio Installer vswhere.exe not found.
  exit /b 1
)
for /f "usebackq tokens=*" %%i in (`"%VT_VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VT_VS=%%i"
if not defined VT_VS (
  echo Visual Studio C++ tools not found.
  exit /b 1
)
call "%VT_VS%\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b 1
set "VSLANG=1033"
cmake --fresh -S "%VT_ROOT%" -B "%VT_ROOT%\build-msvc" -G Ninja "-DCMAKE_C_COMPILER=%VT_ROOT%\third_party\llvm-23.1.4\bin\clang-cl.exe" "-DCMAKE_CXX_COMPILER=%VT_ROOT%\third_party\llvm-23.1.4\bin\clang-cl.exe" -DCMAKE_BUILD_TYPE=Release -DVALUETRACE_WITH_LLVM=ON -DBUILD_TESTING=ON
if errorlevel 1 exit /b 1
cmake --build "%VT_ROOT%\build-msvc"
if errorlevel 1 exit /b 1
ctest --test-dir "%VT_ROOT%\build-msvc" --output-on-failure
exit /b %errorlevel%
