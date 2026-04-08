/**
 * @file ToolBar.h
 * @brief 工具栏类 - 提供截图操作按钮（美化版）
 */

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFrame>
#include <QLoggingCategory>

// 日志分类声明
Q_DECLARE_LOGGING_CATEGORY(logToolBar)

/**
 * @class ToolBar
 * @brief 截图操作工具栏，提供确认、保存、取消等按钮
 * 
 * 采用玻璃拟态设计风格：
 * - 半透明背景
 * - 柔和阴影
 * - 渐变按钮
 * - 悬浮动画效果
 */
class ToolBar : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 按钮类型枚举
     */
    enum class ButtonType {
        Primary,    ///< 主要按钮（蓝色）
        Success,    ///< 成功按钮（绿色）
        Danger,     ///< 危险按钮（红色）
        Secondary   ///< 次要按钮（灰色）
    };

    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit ToolBar(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ToolBar() override;
    
    /**
     * @brief 获取推荐尺寸
     * @return 推荐的工具栏尺寸
     */
    QSize sizeHint() const override;

signals:
    /**
     * @brief 确认按钮点击信号（复制到剪贴板）
     */
    void confirmClicked();
    
    /**
     * @brief 保存按钮点击信号
     */
    void saveClicked();
    
    /**
     * @brief 取消按钮点击信号
     */
    void cancelClicked();
    
    /**
     * @brief 矩形标注工具点击信号
     */
    void rectangleToolClicked();
    
    /**
     * @brief 箭头标注工具点击信号
     */
    void arrowToolClicked();
    
    /**
     * @brief 选择工具点击信号（用于移动选区）
     */
    void selectToolClicked();

protected:
    /**
     * @brief 绘制事件处理
     * @param event 绘制事件
     */
    void paintEvent(QPaintEvent *event) override;

private:
    /**
     * @brief 初始化UI
     */
    void setupUI();
    
    /**
     * @brief 创建工具按钮
     * @param text 按钮文本/图标
     * @param tooltip 提示文本
     * @param type 按钮类型
     * @return 创建的按钮指针
     */
    QPushButton* createButton(const QString &text, 
                              const QString &tooltip,
                              ButtonType type = ButtonType::Secondary);
    
    /**
     * @brief 创建操作按钮
     * @param text 按钮文本
     * @param type 按钮类型
     * @return 创建的按钮指针
     */
    QPushButton* createActionButton(const QString &text, ButtonType type);
    
    /**
     * @brief 设置阴影效果
     */
    void setupShadow();

private:
    QHBoxLayout *m_layout;       ///< 布局管理器
    QPushButton *m_confirmBtn;   ///< 确认按钮
    QPushButton *m_saveBtn;      ///< 保存按钮
    QPushButton *m_cancelBtn;    ///< 取消按钮
    QPushButton *m_selectBtn;    ///< 选择工具按钮
    QPushButton *m_rectBtn;      ///< 矩形标注按钮
    QPushButton *m_arrowBtn;     ///< 箭头标注按钮
};

#endif // TOOLBAR_H
