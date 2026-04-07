#!/bin/bash

# ============================================================
# Qt 框选截图工具 - 一键运行脚本
# 
# 使用方法：chmod +x run.sh && ./run.sh
# 支持系统：macOS、Ubuntu/Debian、Fedora/CentOS、Arch Linux
# ============================================================

set -e

# 获取脚本所在目录的绝对路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# ===== 颜色定义 =====
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# ===== 辅助函数 =====
print_banner() {
    echo ""
    echo -e "${CYAN}╔═══════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║     📸 Qt 框选截图工具 v1.0.0         ║${NC}"
    echo -e "${CYAN}╚═══════════════════════════════════════╝${NC}"
    echo ""
}

print_step() {
    echo -e "${BLUE}▶ $1${NC}"
}

print_success() {
    echo -e "${GREEN}✔ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✖ $1${NC}"
}

# ===== 检测操作系统 =====
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ -f /etc/debian_version ]]; then
        OS="debian"
    elif [[ -f /etc/redhat-release ]]; then
        OS="redhat"
    elif [[ -f /etc/arch-release ]]; then
        OS="arch"
    else
        OS="unknown"
    fi
}

# ===== 显示手动安装说明 =====
show_manual_install_guide() {
    echo ""
    echo -e "${CYAN}┌─────────────────────────────────────────────────────────────┐${NC}"
    echo -e "${CYAN}│                    手动安装依赖说明                          │${NC}"
    echo -e "${CYAN}├─────────────────────────────────────────────────────────────┤${NC}"
    
    case "$OS" in
        macos)
            echo -e "${CYAN}│  macOS:                                                     │${NC}"
            echo -e "${CYAN}│    brew install qt cmake                                    │${NC}"
            echo -e "${CYAN}│                                                             │${NC}"
            echo -e "${CYAN}│  如果没有 Homebrew，先安装:                                  │${NC}"
            echo -e "${CYAN}│    /bin/bash -c \"\$(curl -fsSL https://raw.githubusercon   │${NC}"
            echo -e "${CYAN}│    tent.com/Homebrew/install/HEAD/install.sh)\"             │${NC}"
            ;;
        debian)
            echo -e "${CYAN}│  Ubuntu/Debian:                                             │${NC}"
            echo -e "${CYAN}│    sudo apt-get update                                      │${NC}"
            echo -e "${CYAN}│    sudo apt-get install build-essential cmake qt6-base-dev │${NC}"
            echo -e "${CYAN}│                         libgl1-mesa-dev libxkbcommon-dev   │${NC}"
            ;;
        redhat)
            echo -e "${CYAN}│  Fedora/CentOS/RHEL:                                        │${NC}"
            echo -e "${CYAN}│    sudo dnf install gcc-c++ cmake qt6-qtbase-devel         │${NC}"
            echo -e "${CYAN}│                      mesa-libGL-devel                       │${NC}"
            ;;
        arch)
            echo -e "${CYAN}│  Arch Linux:                                                │${NC}"
            echo -e "${CYAN}│    sudo pacman -Syu base-devel cmake qt6-base               │${NC}"
            ;;
        *)
            echo -e "${CYAN}│  请安装以下依赖:                                            │${NC}"
            echo -e "${CYAN}│    - Qt 6.2+                                                │${NC}"
            echo -e "${CYAN}│    - CMake 3.16+                                            │${NC}"
            echo -e "${CYAN}│    - C++17 编译器 (GCC 8+ / Clang 7+ / MSVC 2019+)         │${NC}"
            ;;
    esac
    
    echo -e "${CYAN}│                                                             │${NC}"
    echo -e "${CYAN}│  安装完成后，重新运行 ./run.sh                              │${NC}"
    echo -e "${CYAN}└─────────────────────────────────────────────────────────────┘${NC}"
    echo ""
}

# ===== 检测并安装依赖 =====
check_and_install_deps() {
    print_step "检查依赖..."
    
    MISSING_DEPS=""
    
    # 检查 CMake
    if ! command -v cmake &> /dev/null; then
        print_warning "CMake 未安装"
        MISSING_DEPS="${MISSING_DEPS}CMake "
    else
        print_success "CMake 已安装: $(cmake --version | head -1)"
    fi
    
    # 检查 Qt6
    QT_FOUND=false
    if command -v qmake6 &> /dev/null; then
        QT_FOUND=true
    elif command -v qmake &> /dev/null && qmake --version 2>/dev/null | grep -q "Qt version 6"; then
        QT_FOUND=true
    elif [[ "$OS" == "macos" ]] && brew list qt@6 &>/dev/null 2>&1; then
        QT_FOUND=true
    elif [[ "$OS" == "macos" ]] && brew list qt &>/dev/null 2>&1; then
        QT_FOUND=true
    fi
    
    if [[ "$QT_FOUND" == "true" ]]; then
        print_success "Qt6 已安装"
    else
        print_warning "Qt6 未安装"
        MISSING_DEPS="${MISSING_DEPS}Qt6 "
    fi
    
    # 如果所有依赖都已安装，直接返回
    if [[ -z "$MISSING_DEPS" ]]; then
        return 0
    fi
    
    # 显示缺失的依赖
    echo ""
    echo -e "${YELLOW}┌─────────────────────────────────────────────────────────────┐${NC}"
    echo -e "${YELLOW}│  检测到缺失依赖: ${RED}${MISSING_DEPS}${YELLOW}                               ${NC}"
    echo -e "${YELLOW}└─────────────────────────────────────────────────────────────┘${NC}"
    echo ""
    
    # 询问用户是否自动安装
    echo -e "${CYAN}请选择操作:${NC}"
    echo -e "  ${GREEN}[1]${NC} 自动安装依赖 (需要管理员权限)"
    echo -e "  ${BLUE}[2]${NC} 显示手动安装说明"
    echo -e "  ${RED}[3]${NC} 退出"
    echo ""
    echo -n -e "${CYAN}请输入选项 [1/2/3]: ${NC}"
    read -r choice
    
    case "$choice" in
        1)
            echo ""
            install_deps
            ;;
        2)
            show_manual_install_guide
            exit 0
            ;;
        *)
            echo ""
            print_warning "已取消。请先安装依赖后再运行。"
            show_manual_install_guide
            exit 1
            ;;
    esac
}

# ===== 安装依赖 =====
install_deps() {
    print_step "安装依赖..."
    
    case "$OS" in
        macos)
            if ! command -v brew &> /dev/null; then
                print_step "安装 Homebrew..."
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
                if [[ -f /opt/homebrew/bin/brew ]]; then
                    eval "$(/opt/homebrew/bin/brew shellenv)"
                fi
            fi
            print_step "安装 Qt6 和 CMake..."
            brew install qt cmake
            ;;
        debian)
            print_step "安装依赖 (需要 sudo)..."
            sudo apt-get update
            sudo apt-get install -y build-essential cmake qt6-base-dev \
                libgl1-mesa-dev libxkbcommon-dev
            ;;
        redhat)
            print_step "安装依赖 (需要 sudo)..."
            if command -v dnf &> /dev/null; then
                sudo dnf install -y gcc-c++ cmake qt6-qtbase-devel mesa-libGL-devel
            else
                sudo yum install -y gcc-c++ cmake qt6-qtbase-devel mesa-libGL-devel
            fi
            ;;
        arch)
            print_step "安装依赖 (需要 sudo)..."
            sudo pacman -Syu --noconfirm base-devel cmake qt6-base
            ;;
        *)
            print_error "不支持自动安装"
            show_manual_install_guide
            exit 1
            ;;
    esac
    
    print_success "依赖安装完成"
}

# ===== 编译项目 =====
build_project() {
    print_step "编译项目..."
    
    cd "$SCRIPT_DIR/screenshot-tool"
    
    # 创建构建目录
    mkdir -p build
    cd build
    
    # CMake 配置
    CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
    
    if [[ "$OS" == "macos" ]]; then
        QT_PATH=$(brew --prefix qt 2>/dev/null || brew --prefix qt@6 2>/dev/null || echo "")
        if [[ -n "$QT_PATH" ]]; then
            CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_PREFIX_PATH=$QT_PATH"
        fi
    fi
    
    cmake .. $CMAKE_ARGS
    
    # 编译
    if [[ "$OS" == "macos" ]]; then
        CORES=$(sysctl -n hw.ncpu)
    else
        CORES=$(nproc)
    fi
    
    make -j$CORES
    
    print_success "编译完成"
    cd ../..
}

# ===== 运行程序 =====
run_app() {
    print_step "启动截图工具..."
    
    cd "$SCRIPT_DIR/screenshot-tool"
    
    # 查找可执行文件
    if [[ -f "build/ScreenshotTool.app/Contents/MacOS/ScreenshotTool" ]]; then
        EXECUTABLE="build/ScreenshotTool.app/Contents/MacOS/ScreenshotTool"
    elif [[ -f "build/ScreenshotTool" ]]; then
        EXECUTABLE="build/ScreenshotTool"
    else
        print_error "找不到可执行文件"
        return 1
    fi
    
    # macOS 屏幕录制权限提示
    if [[ "$OS" == "macos" ]]; then
        APP_PATH="$(pwd)/build/ScreenshotTool.app"
        FIRST_RUN_FLAG="$(pwd)/build/.permission_shown"
        
        if [[ ! -f "$FIRST_RUN_FLAG" ]]; then
            echo ""
            echo -e "${YELLOW}┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓${NC}"
            echo -e "${YELLOW}┃              ${RED}⚠️  macOS 权限设置提示${YELLOW}                        ┃${NC}"
            echo -e "${YELLOW}┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫${NC}"
            echo -e "${YELLOW}┃                                                           ┃${NC}"
            echo -e "${YELLOW}┃  macOS 需要授予「${CYAN}屏幕录制${YELLOW}」权限才能正常截图。             ┃${NC}"
            echo -e "${YELLOW}┃  首次运行会自动打开系统设置窗口。                         ┃${NC}"
            echo -e "${YELLOW}┃                                                           ┃${NC}"
            echo -e "${YELLOW}┃  ${GREEN}设置步骤:${YELLOW}                                                ┃${NC}"
            echo -e "${YELLOW}┃    1. 点击左下角 🔒 解锁（需要输入密码）                  ┃${NC}"
            echo -e "${YELLOW}┃    2. 点击 ${CYAN}[+]${YELLOW} 添加应用                                    ┃${NC}"
            echo -e "${YELLOW}┃    3. 选择 ${CYAN}ScreenshotTool.app${YELLOW}                              ┃${NC}"
            echo -e "${YELLOW}┃    4. 勾选启用权限                                        ┃${NC}"
            echo -e "${YELLOW}┃    5. 重新运行 ${CYAN}./run.sh${YELLOW}                                    ┃${NC}"
            echo -e "${YELLOW}┃                                                           ┃${NC}"
            echo -e "${YELLOW}┃  ${BLUE}应用位置:${YELLOW}                                                ┃${NC}"
            echo -e "${YELLOW}┃  ${CYAN}${APP_PATH}${NC}"
            echo -e "${YELLOW}┃                                                           ┃${NC}"
            echo -e "${YELLOW}┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛${NC}"
            echo ""
            
            # 打开系统设置的屏幕录制页面
            open "x-apple.systempreferences:com.apple.preference.security?Privacy_ScreenCapture"
            sleep 0.3
            
            # 打开 Finder 并选中 app
            if [[ -d "$APP_PATH" ]]; then
                open -R "$APP_PATH"
            fi
            
            # 创建标记文件
            touch "$FIRST_RUN_FLAG"
            
            echo -e "${CYAN}按回车键继续启动（如未设置权限，截图可能显示黑屏）...${NC}"
            read -r
            echo ""
        fi
    fi
    
    print_success "启动中..."
    echo ""
    
    ./$EXECUTABLE
}

# ===== 主程序 =====
main() {
    print_banner
    detect_os
    
    print_step "检测到系统: $OS"
    echo ""
    
    # 检查是否已编译
    cd "$SCRIPT_DIR"
    
    if [[ -f "screenshot-tool/build/ScreenshotTool.app/Contents/MacOS/ScreenshotTool" ]] || \
       [[ -f "screenshot-tool/build/ScreenshotTool" ]]; then
        # 已编译，直接运行
        run_app
    else
        # 未编译，先安装依赖并编译
        check_and_install_deps
        echo ""
        build_project
        echo ""
        run_app
    fi
}

main "$@"
