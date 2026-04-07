@echo off
chcp 65001 >nul 2>&1
setlocal EnableDelayedExpansion

:: ============================================================
:: Qt 框选截图工具 - Windows 一键运行脚本
:: 双击运行即可
:: ============================================================

title Qt 框选截图工具

echo.
echo ╔═══════════════════════════════════════╗
echo ║     📸 Qt 框选截图工具 v1.0.0         ║
echo ╚═══════════════════════════════════════╝
echo.

:: 检查是否已编译
if exist "%~dp0screenshot-tool\build\Release\ScreenshotTool.exe" (
    goto :run_app
)
if exist "%~dp0screenshot-tool\build\ScreenshotTool.exe" (
    goto :run_app
)

:: 未编译，检查依赖
echo [*] 检查依赖...
echo.

set MISSING_DEPS=
set CMAKE_MISSING=0
set QT_MISSING=0

:: 检查 CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] CMake 未安装
    set CMAKE_MISSING=1
    set MISSING_DEPS=CMake
) else (
    echo [√] CMake 已安装
)

:: 检查 Qt6
set QT_FOUND=0
if exist "C:\Qt\6*" set QT_FOUND=1
if exist "%LOCALAPPDATA%\Qt\6*" set QT_FOUND=1

if %QT_FOUND%==0 (
    echo [!] Qt6 未安装
    set QT_MISSING=1
    if defined MISSING_DEPS (
        set MISSING_DEPS=%MISSING_DEPS%, Qt6
    ) else (
        set MISSING_DEPS=Qt6
    )
) else (
    echo [√] Qt6 已安装
)

:: 如果有缺失依赖，询问用户
if defined MISSING_DEPS (
    echo.
    echo ┌─────────────────────────────────────────────────────────┐
    echo │  检测到缺失依赖: %MISSING_DEPS%
    echo └─────────────────────────────────────────────────────────┘
    echo.
    echo 请选择操作:
    echo   [1] 自动安装依赖 (CMake 通过 winget, Qt6 需手动)
    echo   [2] 显示手动安装说明
    echo   [3] 退出
    echo.
    set /p CHOICE="请输入选项 [1/2/3]: "
    
    if "!CHOICE!"=="1" goto :install_deps
    if "!CHOICE!"=="2" goto :show_manual
    goto :exit_script
)
goto :build_project

:install_deps
echo.
:: 安装 CMake
if %CMAKE_MISSING%==1 (
    echo [*] 尝试通过 winget 安装 CMake...
    winget install Kitware.CMake --silent
    if %errorlevel% neq 0 (
        echo [X] CMake 安装失败，请手动安装
    ) else (
        echo [√] CMake 安装成功
    )
)

:: Qt6 需要手动安装
if %QT_MISSING%==1 (
    echo.
    echo [!] Qt6 需要手动安装:
    echo     https://www.qt.io/download-qt-installer
    echo     安装时选择 Qt 6.x 和 MSVC 编译器
    echo.
    echo 安装完成后，请重新运行此脚本。
    pause
    exit /b 1
)
goto :build_project

:show_manual
echo.
echo ┌─────────────────────────────────────────────────────────────┐
echo │                    手动安装依赖说明                          │
echo ├─────────────────────────────────────────────────────────────┤
echo │                                                             │
echo │  CMake:                                                     │
echo │    方式1: winget install Kitware.CMake                      │
echo │    方式2: https://cmake.org/download/                       │
echo │                                                             │
echo │  Qt6:                                                       │
echo │    下载: https://www.qt.io/download-qt-installer            │
echo │    安装时选择:                                              │
echo │      - Qt 6.x (最新版本)                                    │
echo │      - MSVC 2019/2022 64-bit 编译器                         │
echo │      - Qt 5 Compatibility Module (可选)                     │
echo │                                                             │
echo │  Visual Studio:                                             │
echo │    需要安装 Visual Studio 2019/2022                         │
echo │    选择 "C++ 桌面开发" 工作负载                             │
echo │                                                             │
echo │  安装完成后，重新运行 run.bat                               │
echo └─────────────────────────────────────────────────────────────┘
echo.
pause
exit /b 0

:exit_script
echo.
echo [!] 已取消。请先安装依赖后再运行。
pause
exit /b 1

:build_project

:: 编译项目
echo.
echo [*] 编译项目...

cd /d "%~dp0screenshot-tool"
if not exist build mkdir build
cd build

:: 查找 Qt 路径
set QT_PATH=
for /d %%i in ("C:\Qt\6*") do set QT_PATH=%%i\msvc2019_64
if not defined QT_PATH (
    for /d %%i in ("%LOCALAPPDATA%\Qt\6*") do set QT_PATH=%%i\msvc2019_64
)

cmake .. -DCMAKE_PREFIX_PATH="%QT_PATH%" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo [X] CMake 配置失败
    pause
    exit /b 1
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [X] 编译失败
    pause
    exit /b 1
)

echo [√] 编译完成
cd /d "%~dp0"

:run_app
echo.
echo [*] 启动截图工具...
echo.

:: 查找并运行可执行文件
if exist "%~dp0screenshot-tool\build\Release\ScreenshotTool.exe" (
    start "" "%~dp0screenshot-tool\build\Release\ScreenshotTool.exe"
) else if exist "%~dp0screenshot-tool\build\ScreenshotTool.exe" (
    start "" "%~dp0screenshot-tool\build\ScreenshotTool.exe"
) else (
    echo [X] 找不到可执行文件
    pause
    exit /b 1
)

exit /b 0
