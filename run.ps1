# ============================================================
# Qt 框选截图工具 - Windows PowerShell 一键运行脚本
# 右键选择"使用 PowerShell 运行"
# ============================================================

$Host.UI.RawUI.WindowTitle = "Qt 框选截图工具"

Write-Host ""
Write-Host "╔═══════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║     📸 Qt 框选截图工具 v1.0.0         ║" -ForegroundColor Cyan
Write-Host "╚═══════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ScriptDir "screenshot-tool\build"
$ExePath1 = Join-Path $BuildDir "Release\ScreenshotTool.exe"
$ExePath2 = Join-Path $BuildDir "ScreenshotTool.exe"

# 检查是否已编译
if ((Test-Path $ExePath1) -or (Test-Path $ExePath2)) {
    Write-Host "[*] 启动截图工具..." -ForegroundColor Blue
    Write-Host ""
    
    if (Test-Path $ExePath1) {
        Start-Process $ExePath1
    } else {
        Start-Process $ExePath2
    }
    exit 0
}

# 显示手动安装说明
function Show-ManualInstallGuide {
    Write-Host ""
    Write-Host "┌─────────────────────────────────────────────────────────────┐" -ForegroundColor Cyan
    Write-Host "│                    手动安装依赖说明                          │" -ForegroundColor Cyan
    Write-Host "├─────────────────────────────────────────────────────────────┤" -ForegroundColor Cyan
    Write-Host "│                                                             │" -ForegroundColor Cyan
    Write-Host "│  CMake:                                                     │" -ForegroundColor Cyan
    Write-Host "│    方式1: winget install Kitware.CMake                      │" -ForegroundColor Cyan
    Write-Host "│    方式2: https://cmake.org/download/                       │" -ForegroundColor Cyan
    Write-Host "│                                                             │" -ForegroundColor Cyan
    Write-Host "│  Qt6:                                                       │" -ForegroundColor Cyan
    Write-Host "│    下载: https://www.qt.io/download-qt-installer            │" -ForegroundColor Cyan
    Write-Host "│    安装时选择:                                              │" -ForegroundColor Cyan
    Write-Host "│      - Qt 6.x (最新版本)                                    │" -ForegroundColor Cyan
    Write-Host "│      - MSVC 2019/2022 64-bit 编译器                         │" -ForegroundColor Cyan
    Write-Host "│      - Qt 5 Compatibility Module (可选)                     │" -ForegroundColor Cyan
    Write-Host "│                                                             │" -ForegroundColor Cyan
    Write-Host "│  Visual Studio:                                             │" -ForegroundColor Cyan
    Write-Host "│    需要安装 Visual Studio 2019/2022                         │" -ForegroundColor Cyan
    Write-Host '│    选择 "C++ 桌面开发" 工作负载                             │' -ForegroundColor Cyan
    Write-Host "│                                                             │" -ForegroundColor Cyan
    Write-Host "│  安装完成后，重新运行 run.ps1                               │" -ForegroundColor Cyan
    Write-Host "└─────────────────────────────────────────────────────────────┘" -ForegroundColor Cyan
    Write-Host ""
}

# 未编译，检查依赖
Write-Host "[*] 检查依赖..." -ForegroundColor Blue
Write-Host ""

$MissingDeps = @()

# 检查 CMake
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Host "[!] CMake 未安装" -ForegroundColor Yellow
    $MissingDeps += "CMake"
} else {
    Write-Host "[√] CMake 已安装" -ForegroundColor Green
}

# 检查 Qt6
$QtPath = $null
$QtDirs = @("C:\Qt", "$env:LOCALAPPDATA\Qt")
foreach ($dir in $QtDirs) {
    if (Test-Path $dir) {
        $qt6Dir = Get-ChildItem $dir -Directory | Where-Object { $_.Name -match "^6\." } | Select-Object -First 1
        if ($qt6Dir) {
            $msvcDir = Join-Path $qt6Dir.FullName "msvc2019_64"
            if (Test-Path $msvcDir) {
                $QtPath = $msvcDir
                break
            }
            # 也检查 msvc2022_64
            $msvcDir = Join-Path $qt6Dir.FullName "msvc2022_64"
            if (Test-Path $msvcDir) {
                $QtPath = $msvcDir
                break
            }
        }
    }
}

if (-not $QtPath) {
    Write-Host "[!] Qt6 未安装" -ForegroundColor Yellow
    $MissingDeps += "Qt6"
} else {
    Write-Host "[√] Qt6 已安装: $QtPath" -ForegroundColor Green
}

# 如果有缺失依赖，询问用户
if ($MissingDeps.Count -gt 0) {
    $depsStr = $MissingDeps -join ", "
    Write-Host ""
    Write-Host "┌─────────────────────────────────────────────────────────────┐" -ForegroundColor Yellow
    Write-Host "│  检测到缺失依赖: $depsStr" -ForegroundColor Yellow
    Write-Host "└─────────────────────────────────────────────────────────────┘" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "请选择操作:" -ForegroundColor Cyan
    Write-Host "  [1] 自动安装依赖 (CMake 通过 winget, Qt6 需手动)" -ForegroundColor Green
    Write-Host "  [2] 显示手动安装说明" -ForegroundColor Blue
    Write-Host "  [3] 退出" -ForegroundColor Red
    Write-Host ""
    $choice = Read-Host "请输入选项 [1/2/3]"
    
    switch ($choice) {
        "1" {
            Write-Host ""
            # 安装 CMake
            if ($MissingDeps -contains "CMake") {
                Write-Host "[*] 尝试通过 winget 安装 CMake..." -ForegroundColor Blue
                try {
                    winget install Kitware.CMake --silent
                    Write-Host "[√] CMake 安装成功" -ForegroundColor Green
                } catch {
                    Write-Host "[X] CMake 安装失败，请手动安装" -ForegroundColor Red
                }
            }
            
            # Qt6 需要手动安装
            if ($MissingDeps -contains "Qt6") {
                Write-Host ""
                Write-Host "[!] Qt6 需要手动安装:" -ForegroundColor Yellow
                Write-Host "    https://www.qt.io/download-qt-installer" -ForegroundColor Cyan
                Write-Host "    安装时选择 Qt 6.x 和 MSVC 编译器" -ForegroundColor Cyan
                Write-Host ""
                Write-Host "安装完成后，请重新运行此脚本。" -ForegroundColor Yellow
                Read-Host "按回车键退出"
                exit 1
            }
        }
        "2" {
            Show-ManualInstallGuide
            Read-Host "按回车键退出"
            exit 0
        }
        default {
            Write-Host ""
            Write-Host "[!] 已取消。请先安装依赖后再运行。" -ForegroundColor Yellow
            Show-ManualInstallGuide
            Read-Host "按回车键退出"
            exit 1
        }
    }
}

# 编译项目
Write-Host ""
Write-Host "[*] 编译项目..." -ForegroundColor Blue

$SourceDir = Join-Path $ScriptDir "screenshot-tool"
$BuildDir = Join-Path $SourceDir "build"

if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

Set-Location $BuildDir

# CMake 配置
$cmakeResult = cmake .. -DCMAKE_PREFIX_PATH="$QtPath" -DCMAKE_BUILD_TYPE=Release 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "[X] CMake 配置失败" -ForegroundColor Red
    Write-Host $cmakeResult
    Read-Host "按回车键退出"
    exit 1
}

# 编译
$buildResult = cmake --build . --config Release 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "[X] 编译失败" -ForegroundColor Red
    Write-Host $buildResult
    Read-Host "按回车键退出"
    exit 1
}

Write-Host "[√] 编译完成" -ForegroundColor Green

# 运行
Set-Location $ScriptDir
Write-Host ""
Write-Host "[*] 启动截图工具..." -ForegroundColor Blue
Write-Host ""

$ExePath = Join-Path $BuildDir "Release\ScreenshotTool.exe"
if (-not (Test-Path $ExePath)) {
    $ExePath = Join-Path $BuildDir "ScreenshotTool.exe"
}

if (Test-Path $ExePath) {
    Start-Process $ExePath
} else {
    Write-Host "[X] 找不到可执行文件" -ForegroundColor Red
    Read-Host "按回车键退出"
    exit 1
}
