/**
 * @file StartWindow.h
 * @brief 启动窗口类 - 应用程序的初始界面
 * 
 * 提供现代化的启动界面，包含：
 * - 自定义无边框窗口
 * - 拖拽移动支持
 * - 开始截图按钮
 */

#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QWidget>
#include <QPoint>

class QVBoxLayout;
class QPushButton;
class QLabel;

/**
 * @class StartWindow
 * @brief 启动窗口类
 * 
 * 显示应用程序的初始界面，用户可以从这里开始截图操作
 */
class StartWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit StartWindow(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~StartWindow() override = default;

signals:
    /**
     * @brief 请求开始截图信号
     */
    void startCaptureRequested();

protected:
    /**
     * @brief 绘制事件 - 自定义绘制窗口背景和阴影
     */
    void paintEvent(QPaintEvent *event) override;
    
    /**
     * @brief 鼠标按下事件 - 用于窗口拖拽
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief 鼠标移动事件 - 用于窗口拖拽
     */
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    /**
     * @brief 初始化UI组件
     */
    void setupUI();
    
    /**
     * @brief 创建标题区域
     * @return 标题布局
     */
    QWidget* createTitleSection();
    
    /**
     * @brief 创建开始按钮
     * @return 按钮指针
     */
    QPushButton* createCaptureButton();
    
    /**
     * @brief 创建底部区域
     * @return 底部布局
     */
    QWidget* createBottomSection();

private:
    QPoint m_dragPos;           ///< 拖拽起始位置
    QPushButton *m_captureBtn;  ///< 开始截图按钮
    QPushButton *m_closeBtn;    ///< 关闭按钮
    QPushButton *m_quitBtn;     ///< 退出按钮
};

#endif // STARTWINDOW_H
