/**
 * @file StyleManager.h
 * @brief 样式管理器 - 统一管理应用程序的视觉样式
 * 
 * 提供：
 * - 统一的颜色方案
 * - 组件样式表
 * - 动画效果
 * - 主题切换支持
 */

#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QFont>
#include <QLinearGradient>

/**
 * @class StyleManager
 * @brief 样式管理器单例类
 */
class StyleManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 主题枚举
     */
    enum class Theme {
        Light,  ///< 浅色主题
        Dark    ///< 深色主题
    };
    Q_ENUM(Theme)

    /**
     * @brief 获取单例实例
     */
    static StyleManager* instance();
    
    /**
     * @brief 设置当前主题
     * @param theme 主题类型
     */
    void setTheme(Theme theme);
    
    /**
     * @brief 获取当前主题
     */
    Theme currentTheme() const;

    // ===== 颜色定义 =====
    
    /**
     * @brief 获取主题色
     */
    QColor primaryColor() const;
    
    /**
     * @brief 获取主题色（悬浮状态）
     */
    QColor primaryHoverColor() const;
    
    /**
     * @brief 获取主题色（按下状态）
     */
    QColor primaryPressedColor() const;
    
    /**
     * @brief 获取成功色
     */
    QColor successColor() const;
    
    /**
     * @brief 获取警告色
     */
    QColor warningColor() const;
    
    /**
     * @brief 获取错误色
     */
    QColor dangerColor() const;
    
    /**
     * @brief 获取信息色
     */
    QColor infoColor() const;
    
    /**
     * @brief 获取背景色
     */
    QColor backgroundColor() const;
    
    /**
     * @brief 获取卡片背景色
     */
    QColor cardBackgroundColor() const;
    
    /**
     * @brief 获取遮罩色
     */
    QColor maskColor() const;
    
    /**
     * @brief 获取边框色
     */
    QColor borderColor() const;
    
    /**
     * @brief 获取主文字色
     */
    QColor textPrimaryColor() const;
    
    /**
     * @brief 获取次要文字色
     */
    QColor textSecondaryColor() const;
    
    /**
     * @brief 获取占位符文字色
     */
    QColor textPlaceholderColor() const;

    // ===== 尺寸定义 =====
    
    /**
     * @brief 获取边框圆角
     */
    int borderRadius() const;
    
    /**
     * @brief 获取边框宽度
     */
    int borderWidth() const;
    
    /**
     * @brief 获取阴影模糊半径
     */
    int shadowBlurRadius() const;
    
    /**
     * @brief 获取间距（小）
     */
    int spacingSmall() const;
    
    /**
     * @brief 获取间距（中）
     */
    int spacingMedium() const;
    
    /**
     * @brief 获取间距（大）
     */
    int spacingLarge() const;

    // ===== 字体定义 =====
    
    /**
     * @brief 获取标题字体
     */
    QFont titleFont() const;
    
    /**
     * @brief 获取正文字体
     */
    QFont bodyFont() const;
    
    /**
     * @brief 获取小字体
     */
    QFont smallFont() const;

    // ===== 样式表生成 =====
    
    /**
     * @brief 获取全局样式表
     */
    QString globalStyleSheet() const;
    
    /**
     * @brief 获取工具栏样式表
     */
    QString toolBarStyleSheet() const;
    
    /**
     * @brief 获取主按钮样式表
     */
    QString primaryButtonStyleSheet() const;
    
    /**
     * @brief 获取次要按钮样式表
     */
    QString secondaryButtonStyleSheet() const;
    
    /**
     * @brief 获取危险按钮样式表
     */
    QString dangerButtonStyleSheet() const;
    
    /**
     * @brief 获取提示框样式表
     */
    QString tooltipStyleSheet() const;

    // ===== StartWindow 样式 =====
    
    /**
     * @brief 获取启动窗口图标样式
     */
    QString startWindowIconStyleSheet() const;
    
    /**
     * @brief 获取启动窗口标题样式
     */
    QString startWindowTitleStyleSheet() const;
    
    /**
     * @brief 获取启动窗口副标题样式
     */
    QString startWindowSubtitleStyleSheet() const;
    
    /**
     * @brief 获取启动窗口开始按钮样式
     */
    QString startWindowCaptureButtonStyleSheet() const;
    
    /**
     * @brief 获取启动窗口退出按钮样式
     */
    QString startWindowQuitButtonStyleSheet() const;
    
    /**
     * @brief 获取关闭按钮样式
     */
    QString closeButtonStyleSheet() const;

    // ===== ToolBar 样式 =====
    
    /**
     * @brief 获取工具栏圆形按钮样式
     * @param type 按钮类型 (primary/success/danger/secondary)
     * @param radius 按钮圆角半径
     */
    QString toolBarButtonStyleSheet(const QString &type, int radius) const;
    
    /**
     * @brief 获取工具栏标签样式
     */
    QString toolBarLabelStyleSheet() const;

    // ===== 渐变色 =====
    
    /**
     * @brief 获取主题渐变色
     */
    QLinearGradient primaryGradient(const QPointF &start, const QPointF &end) const;
    
    /**
     * @brief 获取玻璃拟态背景渐变
     */
    QLinearGradient glassGradient(const QPointF &start, const QPointF &end) const;

signals:
    /**
     * @brief 主题变更信号
     * @param theme 新主题
     */
    void themeChanged(Theme theme);

private:
    explicit StyleManager(QObject *parent = nullptr);
    ~StyleManager() override = default;
    
    StyleManager(const StyleManager&) = delete;
    StyleManager& operator=(const StyleManager&) = delete;

private:
    static StyleManager *s_instance;
    Theme m_theme;
};

#endif // STYLEMANAGER_H
