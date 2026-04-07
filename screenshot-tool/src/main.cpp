/**
 * @file main.cpp
 * @brief Qt框选截图工具 - 程序入口
 * 
 * 应用程序主入口点，负责：
 * - 初始化 Qt 应用程序
 * - 创建并显示启动窗口
 * - 连接启动窗口与主窗口的信号
 */

#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QGuiApplication>

#include "MainWindow.h"
#include "StartWindow.h"
#include "StyleManager.h"

/**
 * @brief 程序入口函数
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出码
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("ScreenshotTool");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ScreenshotTool");
    
    // 应用全局样式
    app.setStyleSheet(StyleManager::instance()->globalStyleSheet());
    
    // 创建主窗口（后台管理截图逻辑）
    MainWindow mainWindow;
    
    // 创建启动窗口（用户交互界面）
    StartWindow startWindow;
    
    // 连接信号：点击开始截图
    QObject::connect(&startWindow, &StartWindow::startCaptureRequested, [&]() {
        startWindow.hide();
        // 延迟启动截图，确保窗口完全隐藏
        QTimer::singleShot(200, &mainWindow, &MainWindow::startCapture);
    });
    
    // 连接信号：截图完成后重新显示启动窗口
    QObject::connect(&mainWindow, &MainWindow::captureCompleted, [&startWindow]() {
        startWindow.show();
        startWindow.raise();
        startWindow.activateWindow();
    });
    
    // 显示启动窗口并居中
    startWindow.show();
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        startWindow.move(
            (screen->geometry().width() - startWindow.width()) / 2,
            (screen->geometry().height() - startWindow.height()) / 2
        );
    }
    
    return app.exec();
}
