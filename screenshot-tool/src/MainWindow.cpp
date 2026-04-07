/**
 * @file MainWindow.cpp
 * @brief 主窗口类实现
 */

#include "MainWindow.h"
#include "ScreenCapture.h"
#include "SelectionOverlay.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDateTime>
#include <QScreen>
#include <QStyle>
#include <QProcess>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QGuiApplication>

// 定义日志分类
Q_LOGGING_CATEGORY(logMainWindow, "screenshot.mainwindow")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_trayIcon(nullptr)
    , m_trayMenu(nullptr)
    , m_captureAction(nullptr)
    , m_aboutAction(nullptr)
    , m_quitAction(nullptr)
    , m_screenCapture(new ScreenCapture(this))
    , m_selectionOverlay(nullptr)
    , m_globalShortcut(nullptr)
{
    qCInfo(logMainWindow) << "初始化主窗口";
    
    // 设置窗口属性
    setWindowTitle("框选截图工具");
    setMinimumSize(300, 200);
    resize(400, 300);
    
    // 创建托盘图标和菜单
    createTrayMenu();
    createTrayIcon();
    
    // 注册快捷键
    registerGlobalShortcuts();
    
    qCInfo(logMainWindow) << "主窗口初始化完成";
}

MainWindow::~MainWindow()
{
    qCInfo(logMainWindow) << "销毁主窗口";
    
    if (m_selectionOverlay) {
        m_selectionOverlay->close();
        delete m_selectionOverlay;
        m_selectionOverlay = nullptr;
    }
}

void MainWindow::createTrayMenu()
{
    qCDebug(logMainWindow) << "创建托盘菜单";
    
    m_trayMenu = new QMenu(this);
    
    // 截图动作
    m_captureAction = new QAction("截图 (&C)", this);
    m_captureAction->setShortcut(QKeySequence("Ctrl+Alt+A"));
    m_captureAction->setToolTip("开始框选截图");
    connect(m_captureAction, &QAction::triggered, this, &MainWindow::startCapture);
    m_trayMenu->addAction(m_captureAction);
    
    m_trayMenu->addSeparator();
    
    // 关于动作
    m_aboutAction = new QAction("关于 (&A)", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    m_trayMenu->addAction(m_aboutAction);
    
    m_trayMenu->addSeparator();
    
    // 退出动作
    m_quitAction = new QAction("退出 (&Q)", this);
    connect(m_quitAction, &QAction::triggered, this, &MainWindow::quitApplication);
    m_trayMenu->addAction(m_quitAction);
}

void MainWindow::createTrayIcon()
{
    qCDebug(logMainWindow) << "创建系统托盘图标";
    
    m_trayIcon = new QSystemTrayIcon(this);
    
    // 使用系统图标作为托盘图标
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_DesktopIcon);
    
    // 如果获取不到系统图标，创建一个简单的图标
    if (icon.isNull()) {
        QPixmap pixmap(32, 32);
        pixmap.fill(QColor("#409EFF"));
        icon = QIcon(pixmap);
    }
    
    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip("框选截图工具\n点击开始截图\n右键查看菜单");
    m_trayIcon->setContextMenu(m_trayMenu);
    
    // 连接托盘图标激活信号
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayIconActivated);
    
    // 显示托盘图标
    m_trayIcon->show();
    
    // 显示启动通知
    showNotification("截图工具已启动", 
                    "点击托盘图标或按 Ctrl+Alt+A 开始截图");
}

void MainWindow::registerGlobalShortcuts()
{
    qCDebug(logMainWindow) << "注册全局快捷键";
    
    // 注意：Qt 的 QShortcut 只在窗口有焦点时有效
    // 真正的全局快捷键需要平台特定的实现
    // 这里提供一个基本的实现，当窗口可见时可用
    
    m_globalShortcut = new QShortcut(QKeySequence("Ctrl+Alt+A"), this);
    m_globalShortcut->setContext(Qt::ApplicationShortcut);
    connect(m_globalShortcut, &QShortcut::activated, this, &MainWindow::startCapture);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    qCDebug(logMainWindow) << "托盘图标被激活，原因:" << reason;
    
    switch (reason) {
    case QSystemTrayIcon::Trigger:       // 单击
    case QSystemTrayIcon::DoubleClick:   // 双击
        startCapture();
        break;
    case QSystemTrayIcon::MiddleClick:   // 中键点击
        showAbout();
        break;
    default:
        break;
    }
}

void MainWindow::startCapture()
{
    qCInfo(logMainWindow) << "开始截图流程";
    
    // 如果已有选区窗口，先关闭
    if (m_selectionOverlay) {
        m_selectionOverlay->close();
        delete m_selectionOverlay;
        m_selectionOverlay = nullptr;
    }
    
    // 捕获全屏
    qCInfo(logMainWindow) << "正在捕获屏幕...";
    QPixmap screenshot = m_screenCapture->captureAllScreens();
    qCInfo(logMainWindow) << "捕获结果: isNull=" << screenshot.isNull() 
                          << "size=" << screenshot.width() << "x" << screenshot.height();
    
    if (screenshot.isNull() || screenshot.width() == 0 || screenshot.height() == 0) {
        qCWarning(logMainWindow) << "截图失败：无法捕获屏幕";
        
        // 显示错误对话框
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("截图失败");
        msgBox.setText("无法捕获屏幕图像");
        
#ifdef Q_OS_MAC
        msgBox.setInformativeText(
            "macOS 需要授予「屏幕录制」权限：\n\n"
            "1. 打开 系统设置 -> 隐私与安全性\n"
            "2. 点击「屏幕录制」\n"
            "3. 添加并勾选 ScreenshotTool\n"
            "4. 重新运行程序\n\n"
            "注意：通过终端运行时，终端也需要权限"
        );
        msgBox.addButton("打开设置", QMessageBox::ActionRole);
        msgBox.addButton("退出", QMessageBox::RejectRole);
        
        int result = msgBox.exec();
        
        if (result == 0) {  // ActionRole button was clicked
            QProcess::startDetached("open", 
                QStringList() << "x-apple.systempreferences:com.apple.preference.security?Privacy_ScreenCapture");
        }
#else
        msgBox.setInformativeText("请检查系统权限设置");
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
#endif
        
        // 发出截图完成信号
        emit captureCompleted();
        return;
    }
    
    qCInfo(logMainWindow) << "屏幕捕获成功，尺寸:" 
                          << screenshot.width() << "x" << screenshot.height();
    
    // 创建选区覆盖层
    m_selectionOverlay = new SelectionOverlay();
    m_selectionOverlay->setScreenshot(screenshot);
    
    // 连接信号
    connect(m_selectionOverlay, &SelectionOverlay::captureConfirmed,
            this, &MainWindow::onCaptureConfirmed);
    connect(m_selectionOverlay, &SelectionOverlay::saveRequested,
            this, &MainWindow::onSaveToFile);
    connect(m_selectionOverlay, &SelectionOverlay::captureCancelled,
            this, &MainWindow::onCaptureCancelled);
    
    // 显示覆盖层
    m_selectionOverlay->showFullScreen();
    
    qCInfo(logMainWindow) << "选区覆盖层已显示，等待用户框选";
}

void MainWindow::onCaptureConfirmed(const QPixmap &pixmap)
{
    qCInfo(logMainWindow) << "截图已确认，尺寸:" 
                          << pixmap.width() << "x" << pixmap.height();
    
    // 复制到剪贴板
    copyToClipboard(pixmap);
    
    // 关闭选区窗口
    if (m_selectionOverlay) {
        m_selectionOverlay->close();
        m_selectionOverlay->deleteLater();
        m_selectionOverlay = nullptr;
    }
    
    // 显示浮动提示
    showToast(QString("✓ 已复制到剪贴板 (%1×%2)").arg(pixmap.width()).arg(pixmap.height()));
    
    // 发出截图完成信号
    emit captureCompleted();
    
    // 如果是直接截图模式（quitOnLastWindowClosed），延迟退出
    if (QApplication::quitOnLastWindowClosed()) {
        QTimer::singleShot(1500, qApp, &QApplication::quit);
    }
}

void MainWindow::onSaveToFile(const QPixmap &pixmap)
{
    qCInfo(logMainWindow) << "请求保存截图到文件";
    
    // 生成默认文件名
    QString defaultFileName = QString("screenshot_%1.png")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    
    // 获取默认保存路径
    QString defaultPath = QStandardPaths::writableLocation(
        QStandardPaths::PicturesLocation);
    
    if (defaultPath.isEmpty()) {
        defaultPath = QStandardPaths::writableLocation(
            QStandardPaths::HomeLocation);
    }
    
    QString fullPath = defaultPath + "/" + defaultFileName;
    
    // 先隐藏选区窗口
    if (m_selectionOverlay) {
        m_selectionOverlay->hide();
    }
    
    // 打开保存对话框
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        "保存截图",
        fullPath,
        "PNG 图像 (*.png);;JPEG 图像 (*.jpg *.jpeg);;所有文件 (*.*)"
    );
    
    if (fileName.isEmpty()) {
        qCInfo(logMainWindow) << "用户取消保存";
        // 重新显示选区窗口
        if (m_selectionOverlay) {
            m_selectionOverlay->showFullScreen();
        }
        return;
    }
    
    // 保存图片
    bool success = false;
    if (fileName.endsWith(".jpg", Qt::CaseInsensitive) ||
        fileName.endsWith(".jpeg", Qt::CaseInsensitive)) {
        success = pixmap.save(fileName, "JPEG", 95);
    } else {
        // 默认保存为 PNG
        if (!fileName.endsWith(".png", Qt::CaseInsensitive)) {
            fileName += ".png";
        }
        success = pixmap.save(fileName, "PNG");
    }
    
    if (success) {
        qCInfo(logMainWindow) << "截图已保存到:" << fileName;
        showNotification("保存成功", QString("截图已保存到:\n%1").arg(fileName));
    } else {
        qCWarning(logMainWindow) << "保存失败:" << fileName;
        showNotification("保存失败", "无法保存截图文件", 
                        QSystemTrayIcon::Warning);
    }
    
    // 关闭选区窗口
    if (m_selectionOverlay) {
        m_selectionOverlay->close();
        m_selectionOverlay->deleteLater();
        m_selectionOverlay = nullptr;
    }
    
    // 发出截图完成信号
    emit captureCompleted();
}

void MainWindow::onCaptureCancelled()
{
    qCInfo(logMainWindow) << "截图已取消";
    
    if (m_selectionOverlay) {
        m_selectionOverlay->close();
        m_selectionOverlay->deleteLater();
        m_selectionOverlay = nullptr;
    }
    
    // 发出截图完成信号
    emit captureCompleted();
    
    // 如果是直接截图模式，取消时也退出
    if (QApplication::quitOnLastWindowClosed()) {
        QTimer::singleShot(100, qApp, &QApplication::quit);
    }
}

void MainWindow::copyToClipboard(const QPixmap &pixmap)
{
    QClipboard *clipboard = QApplication::clipboard();
    
    // 使用 QMimeData 确保兼容性
    QMimeData *mimeData = new QMimeData();
    
    // 同时设置 PNG 和原始图像格式，提高兼容性
    QImage image = pixmap.toImage();
    mimeData->setImageData(image);
    
    // 添加 PNG 格式数据
    QByteArray pngData;
    QBuffer buffer(&pngData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    mimeData->setData("image/png", pngData);
    
    clipboard->setMimeData(mimeData);
    
    // macOS 需要处理剪贴板事件确保数据同步
    QApplication::processEvents();
    
    qCDebug(logMainWindow) << "图像已复制到剪贴板，格式: PNG, 尺寸:" 
                           << image.width() << "x" << image.height();
}

void MainWindow::showNotification(const QString &title, 
                                  const QString &message,
                                  QSystemTrayIcon::MessageIcon icon)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, message, icon, 3000);
    }
}

void MainWindow::showAbout()
{
    qCDebug(logMainWindow) << "显示关于对话框";
    
    QMessageBox::about(nullptr, 
        "关于 - 框选截图工具",
        "<h3>框选截图工具</h3>"
        "<p>版本: 1.0.0</p>"
        "<p>一个简洁的跨平台截图工具</p>"
        "<hr>"
        "<p><b>使用方法:</b></p>"
        "<ul>"
        "<li>点击托盘图标或按 <b>Ctrl+Alt+A</b> 开始截图</li>"
        "<li>拖拽鼠标框选截图区域</li>"
        "<li>点击工具栏按钮或使用快捷键完成操作</li>"
        "</ul>"
        "<p><b>快捷键:</b></p>"
        "<ul>"
        "<li><b>Enter</b> - 确认并复制到剪贴板</li>"
        "<li><b>Ctrl+S</b> - 保存到文件</li>"
        "<li><b>ESC</b> - 取消截图</li>"
        "</ul>"
        "<hr>"
        "<p>基于 Qt 6 开发</p>"
    );
}

void MainWindow::quitApplication()
{
    qCInfo(logMainWindow) << "用户请求退出应用程序";
    
    // 隐藏托盘图标
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
    
    // 关闭选区窗口
    if (m_selectionOverlay) {
        m_selectionOverlay->close();
    }
    
    // 退出应用
    QApplication::quit();
}

void MainWindow::showToast(const QString &message)
{
    // 创建浮动提示窗口
    QWidget *toast = new QWidget(nullptr);
    toast->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    toast->setAttribute(Qt::WA_TranslucentBackground);
    toast->setAttribute(Qt::WA_DeleteOnClose);
    
    // 创建标签
    QLabel *label = new QLabel(message, toast);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(R"(
        QLabel {
            background-color: rgba(50, 50, 50, 220);
            color: white;
            padding: 12px 24px;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 500;
        }
    )");
    label->adjustSize();
    
    // 设置窗口大小
    toast->setFixedSize(label->size());
    
    // 计算位置（屏幕底部中央）
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - toast->width()) / 2;
        int y = screenGeometry.height() - toast->height() - 100;
        toast->move(x, y);
    }
    
    // 添加透明度动画
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(toast);
    toast->setGraphicsEffect(effect);
    
    // 显示动画
    QPropertyAnimation *fadeIn = new QPropertyAnimation(effect, "opacity");
    fadeIn->setDuration(200);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    
    // 隐藏动画
    QPropertyAnimation *fadeOut = new QPropertyAnimation(effect, "opacity");
    fadeOut->setDuration(300);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    
    // 1秒后开始淡出
    QTimer::singleShot(1000, [fadeOut]() {
        fadeOut->start();
    });
    
    // 淡出完成后关闭
    QObject::connect(fadeOut, &QPropertyAnimation::finished, toast, &QWidget::close);
    
    toast->show();
    fadeIn->start();
}
