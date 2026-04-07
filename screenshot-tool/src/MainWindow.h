/**
 * @file MainWindow.h
 * @brief 主窗口类 - 管理系统托盘和截图流程
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QShortcut>
#include <QLoggingCategory>

// 前向声明
class ScreenCapture;
class SelectionOverlay;

// 日志分类声明
Q_DECLARE_LOGGING_CATEGORY(logMainWindow)

/**
 * @class MainWindow
 * @brief 应用程序主窗口，负责管理系统托盘图标和协调截图流程
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~MainWindow() override;

public slots:
    /**
     * @brief 开始截图流程
     * 
     * 捕获全屏图像并显示选区覆盖层
     */
    void startCapture();
    
    /**
     * @brief 处理截图完成
     * @param pixmap 截取的图像
     */
    void onCaptureConfirmed(const QPixmap &pixmap);
    
    /**
     * @brief 处理保存到文件请求
     * @param pixmap 要保存的图像
     */
    void onSaveToFile(const QPixmap &pixmap);
    
    /**
     * @brief 处理截图取消
     */
    void onCaptureCancelled();

signals:
    /**
     * @brief 截图完成信号（无论成功、保存还是取消）
     */
    void captureCompleted();

private slots:
    /**
     * @brief 处理托盘图标激活事件
     * @param reason 激活原因
     */
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    
    /**
     * @brief 显示关于对话框
     */
    void showAbout();
    
    /**
     * @brief 退出应用程序
     */
    void quitApplication();

private:
    /**
     * @brief 创建系统托盘图标
     */
    void createTrayIcon();
    
    /**
     * @brief 创建托盘菜单
     */
    void createTrayMenu();
    
    /**
     * @brief 注册全局快捷键
     */
    void registerGlobalShortcuts();
    
    /**
     * @brief 复制图像到剪贴板
     * @param pixmap 要复制的图像
     */
    void copyToClipboard(const QPixmap &pixmap);
    
    /**
     * @brief 显示托盘通知
     */
    void showNotification(const QString &title, 
                         const QString &message,
                         QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information);
    
    /**
     * @brief 显示浮动提示
     */
    void showToast(const QString &message);

private:
    // 系统托盘相关
    QSystemTrayIcon *m_trayIcon;     ///< 系统托盘图标
    QMenu *m_trayMenu;               ///< 托盘右键菜单
    
    // 托盘菜单动作
    QAction *m_captureAction;        ///< 截图动作
    QAction *m_aboutAction;          ///< 关于动作
    QAction *m_quitAction;           ///< 退出动作
    
    // 截图核心组件
    ScreenCapture *m_screenCapture;       ///< 屏幕捕获器
    SelectionOverlay *m_selectionOverlay; ///< 选区覆盖层
    
    // 快捷键
    QShortcut *m_globalShortcut;     ///< 全局截图快捷键
};

#endif // MAINWINDOW_H
