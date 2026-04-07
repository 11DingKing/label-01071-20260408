# Qt 框选截图工具

一个使用 Qt 6 和 C++17 开发的跨平台框选截图工具，支持 Windows、macOS 和 Linux。

## 一键运行

### macOS / Linux

```bash
chmod +x run.sh
./run.sh
```

### Windows

**方式 A：双击运行**

```
双击 run.bat 文件
```

**方式 B：PowerShell**

```powershell
右键 run.ps1 → 使用 PowerShell 运行
```

---

**脚本功能：**

- ✅ 检测操作系统
- ✅ 检查依赖状态 (Qt6, CMake)
- ✅ **缺失依赖时询问用户**（不会静默安装）
- ✅ 提供手动安装说明
- ✅ 编译项目
- ✅ 启动截图工具

**依赖安装策略：**

脚本不会自动安装依赖。当检测到缺失依赖时，会提示用户选择：

```
检测到缺失依赖: CMake, Qt6

请选择操作:
  [1] 自动安装依赖 (需要管理员权限)
  [2] 显示手动安装说明
  [3] 退出
```

这确保了：
- 🔒 不会意外修改系统环境
- 📋 无 sudo 权限时可查看手动安装说明
- 🎯 用户完全掌控安装过程

---

## macOS 首次使用（重要！）

macOS 需要授予「屏幕录制」权限才能截图。首次运行 `./run.sh` 时会自动打开相关窗口。

### 详细步骤

#### 步骤 1：解锁设置

- 在自动打开的「系统设置 → 隐私与安全性 → 屏幕录制」页面
- 点击左下角的 🔒 **锁图标**，输入密码解锁

#### 步骤 2：添加应用

- 点击应用列表下方的 **[ + ]** 按钮
- 在弹出的文件选择器中，按 `Cmd + Shift + G`
- 输入路径（根据你的实际位置）：
  ```
  ~/Documents/web-ai/1071/screenshot-tool/build/
  ```
- 选择 `ScreenshotTool.app`，点击「打开」

**或者直接拖拽：**

- 脚本会自动打开 Finder 并选中 `ScreenshotTool.app`
- 直接将它拖到系统设置的应用列表中

#### 步骤 3：启用权限

- 在列表中找到 `ScreenshotTool`
- **勾选** 它旁边的复选框

#### 步骤 4：重新运行

```bash
./run.sh
```

### 如果还是无法截图

1. **完全退出程序**：

   ```bash
   pkill -f ScreenshotTool
   ```

2. **检查权限是否已勾选**：
   - 系统设置 → 隐私与安全性 → 屏幕录制
   - 确保 `ScreenshotTool` 已勾选

3. **重新运行**：
   ```bash
   ./run.sh
   ```

### 终端也需要权限（可选）

如果通过终端运行，终端应用本身也可能需要「屏幕录制」权限：

- 将 `Terminal.app` 或你使用的终端（如 iTerm）也添加到屏幕录制列表中

---

## 功能特性

| 功能         | 说明                                           |
| ------------ | ---------------------------------------------- |
| 全屏截图捕获 | 支持多屏幕、高 DPI                             |
| 鼠标框选     | 拖拽选择、调整大小、移动选区                   |
| 一键复制     | 截图自动复制到剪贴板，支持 Cmd+V / Ctrl+V 粘贴 |
| 保存文件     | 支持 PNG 格式保存到本地                        |
| 工具栏       | 取消 / 保存 / 确认（带文字标签）               |
| 快捷键       | ESC 取消、Enter 确认、Ctrl+S 保存              |
| Toast 提示   | 操作完成后显示浮动提示                         |

---

## 使用方法

1. 运行 `./run.sh`（macOS/Linux）或 `run.bat`（Windows）
2. 点击 **开始截图** 按钮
3. 拖动鼠标框选需要截取的区域
4. 使用工具栏按钮完成操作：

| 按钮    | 快捷键 | 功能            |
| ------- | ------ | --------------- |
| 🔴 取消 | ESC    | 取消本次截图    |
| 🔵 保存 | Ctrl+S | 保存为 PNG 文件 |
| 🟢 确认 | Enter  | 复制到剪贴板    |

### 剪贴板使用

点击 **确认** 后，截图会自动复制到系统剪贴板，可以直接粘贴到：

- 微信、钉钉等聊天软件
- Word、Pages 等文档软件
- Photoshop、Figma 等设计软件
- 任意支持图片粘贴的应用

**粘贴快捷键：**

- macOS: `Cmd + V`
- Windows/Linux: `Ctrl + V`

---

## 手动编译

### 前置依赖

- Qt 6.2+
- CMake 3.16+
- C++17 编译器

### macOS

```bash
brew install qt cmake
cd screenshot-tool && mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt)
make -j$(sysctl -n hw.ncpu)
./ScreenshotTool.app/Contents/MacOS/ScreenshotTool
```

### Ubuntu/Debian

```bash
sudo apt install qt6-base-dev cmake build-essential
cd screenshot-tool && mkdir build && cd build
cmake .. && make -j$(nproc)
./ScreenshotTool
```

### Windows

```powershell
cd screenshot-tool
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.x\msvc2019_64"
cmake --build . --config Release
.\Release\ScreenshotTool.exe
```

---

## 项目结构

```
.
├── run.sh                # macOS/Linux 一键脚本
├── run.bat               # Windows 批处理脚本
├── run.ps1               # Windows PowerShell 脚本
├── README.md
└── screenshot-tool/      # 源码目录
    ├── CMakeLists.txt
    └── src/
        ├── main.cpp           # 程序入口（精简）
        ├── MainWindow.cpp/h   # 主窗口逻辑
        ├── StartWindow.cpp/h  # 启动界面 UI
        ├── ScreenCapture.cpp/h
        ├── SelectionOverlay.cpp/h
        ├── ToolBar.cpp/h
        ├── StyleManager.cpp/h # 统一样式管理
        └── Logger.cpp/h
```

---

## 技术栈

| 类别 | 技术 |
|------|------|
| 语言 | C++17 |
| 框架 | Qt 6 (Widgets, Gui) |
| 构建 | CMake 3.16+ |
| 剪贴板 | QMimeData + PNG 格式 |

---

## 代码架构

| 模块 | 职责 |
|------|------|
| `main.cpp` | 程序入口，仅负责初始化和信号连接 |
| `StartWindow` | 启动界面 UI 组件（独立模块） |
| `MainWindow` | 截图流程控制、系统托盘管理 |
| `SelectionOverlay` | 选区覆盖层、鼠标交互 |
| `ToolBar` | 工具栏 UI（使用 StyleManager） |
| `ScreenCapture` | 屏幕捕获、多屏/高DPI 支持 |
| `StyleManager` | **统一样式管理**（颜色、字体、样式表） |
| `Logger` | 日志系统（分类、彩色输出、文件轮转） |

**设计原则：**
- 所有 UI 样式通过 `StyleManager` 单例统一管理，避免硬编码
- 每个组件职责单一，便于维护和测试
- `main.cpp` 仅作为程序入口，不包含类定义

---

## License

MIT License
